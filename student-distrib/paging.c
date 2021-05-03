#include "paging.h"

/* paging_init - CP1
 * Initializes and enables paging. This includes the 4KB video memory inside
 * the first 4MB page, the 4MB Kernal page, as well as 1022 "not present"
 * 4MB pages.
 * parameter - none
 * return - none
 */
void paging_init(void) {
    int i;
    for (i = 0; i < _1_KB; ++i) {
        page_dir[i] = RW;
        page_table[i] = i * _4_KB | RW;
    }
    // 4KB page mapped to physical video memory
    page_table[VID_MEM_IDX] |= RW | PR;
    // preceding 3 pages reserved for terminal buffers
    page_table[TERM0_BUFF] |= RW | PR;
    page_table[TERM1_BUFF] |= RW | PR;
    page_table[TERM2_BUFF] |= RW | PR;
    // first page is reserved for video and buffers
    page_dir[K_VIDEO_IDX] = ((uint32_t)page_table) | RW | PR;
    // second page is reserved for 4MB Kernel page
    page_dir[KERNEL_IDX] = _4_MB | PAGE_4MB | RW | PR;

    // - set CR3 using address of page_directory,
    // - set CR4.PSE bit (to enable 4MB pages)
    // - set CR0.PG bit, CR0.PE bit?
    asm volatile("                                               \n\
        movl $page_dir, %%eax                                     \n\
        movl %%eax, %%cr3                                         \n\
        movl %%cr4, %%eax                                         \n\
        orl  $0x00000010, %%eax                                   \n\
        movl %%eax, %%cr4                                         \n\
        movl %%cr0, %%eax                                         \n\
        orl  $0x80000001, %%eax                                   \n\
        movl %%eax, %%cr0"                                        \
        :                                                         \
        : "g"(page_dir)                                           \
        : "eax");
}

/* map_program - CP3
 * Maps the program that is currently running to the correct process given
 * by the process number.
 * parameter - pid : pid is between 0 - 5, thus we point at
 *                   8MB, 12MB, ... and so on depending on the process.
 * return - none
 */
void map_program(uint32_t pid) {
    uint32_t addr = _8_MB + _4_MB * pid;
    page_dir[PROGRAM_IDX] = addr | PAGE_4MB | USR | RW | PR;
    flush();
}

/* map_video - CP4
 * maps video memory page in virtual address
 * parameter - void
 * return - none
 */
void map_video(void){
    page_dir[U_VIDEO_IDX] = (uint32_t)page_table | USR | RW | PR;
    page_table[0] = (uint32_t)VID_MEM | USR | RW | PR;
}

/* switch_display - CP5
 * Switches visible terminal from one to the other. First copies contents of
 * current video memory to the correct terminal video buffer,  and then
 * esp for process switching out of, and restores ebp and esp for process
 * we switch into. 
 * parameter - tid : terminal number to display to screen. Saves ebp and 
 * return - none
 * side-effect - global t_visible is changed to new visible terminal.
 */
void switch_display(int32_t tid) {
    // sanity check
    if (tid < 0 || tid > (TERMINAL_COUNT-1) || tid == t_visible) 
        return;
    
    int32_t prev_tid = t_visible;

    // save for process switch
    pcb_t *pcb = get_pcb(t[prev_tid].running_process); //old
    asm volatile(
        "movl %%esp, %0     \n"
        "movl %%ebp, %1     \n"
        :"=r"(pcb->cur_esp), "=r"(pcb->cur_ebp) // output
        : // input
    );

    memcpy((uint8_t*)(t[t_visible].video_mem), (uint8_t*)VID_MEM, _4_KB);
    // update t_visible to next process
    t_visible = tid;
    // copy next background buffer into VID_MEM so we can display
    memcpy((uint8_t*)VID_MEM, (uint8_t*)t[t_visible].video_mem, _4_KB);
    update_cursor();

    if(t[prev_tid].shell_flag == 0 && t[t_visible].shell_flag == 0)  {
        pcb_t *n_pcb = get_pcb(t[t_visible].running_process);  // new

        tss.ss0 = KERNEL_DS;
        tss.esp0 = _8_MB - _8_KB * n_pcb->pid - FOUR_BYTE;

        // remap + flush TLB 
        map_program(n_pcb->pid);

        // restore for process switch
        asm volatile(
            "movl %0, %%esp\n\
            movl %1, %%ebp"
            : //ouput
            :"r"(n_pcb->cur_esp), "r"(n_pcb->cur_ebp) //input
        );
    }
}


/* flush - CP2
 * Flushes TLB when altering paging
 * parameter - none
 * return - none
 */
void flush(void) {
    int32_t cr3;
    asm volatile(
        "movl %%cr3, %0     \n\
        movl %0, %%cr3"  \
        :"=r"(cr3)      \
        :"r"(cr3)       \
        :"memory", "cc");
}
