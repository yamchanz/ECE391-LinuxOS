#include "paging.h"

uint32_t video_page_table[_1_KB] __attribute__((aligned(_4_KB)));
uint32_t page_table[_1_KB] __attribute__((aligned(_4_KB)));
uint32_t page_dir[_1_KB] __attribute__((aligned(_4_KB)));

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
    page_table[VIDEO_MEM_ADDR] |= RW | PR;
    // preceding 3 pages reserved for terminal buffers
    page_table[TERMINAL0_BUFF] |= RW | PR;
    page_table[TERMINAL1_BUFF] |= RW | PR;
    page_table[TERMINAL2_BUFF] |= RW | PR;

    // first page is reserved for 4KB video
    page_dir[K_VIDEO_IDX] = ((uint32_t)page_table) | RW | PR;
    // second page is reserved for 4MB Kernel page
    page_dir[KERNEL_IDX] = _4_MB | PAGE_4MB | RW | PR;
    
    // - set CR3 using address of page_directory
    // - set CR4.PSE bit (to enable 4MB pages)
    // - set CR0.PG bit, CR0.PE bit?
    asm volatile("                                                \n\
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
void map_program(int32_t pid) {
    uint32_t addr = _8_MB + _4_MB * pid;
    page_dir[PROGRAM_IDX] = addr | PAGE_4MB | USR | RW | PR;
    flush();
}

/* map_video - CP4
 * maps video memory page in virtual address
 * parameter - none
 * return - none
 */
void map_video(void) {
    page_dir[U_VIDEO_IDX] = (uint32_t)video_page_table | USR | RW | PR;
    // VIDEO_MEM_ADDR = 184 = 0xB8, VIDEO_MEM_ADDR << 12 = 0xB8000 = VID_MEM
    video_page_table[0] = (VIDEO_MEM_ADDR << 12) | USR | RW | PR;
}

/* unmap_video - CP4
 * Unmaps the video memory page. Called in halt when the video memory flag is set
 * parameter - void
 * return - none
 */
void unmap_video(void) {
    page_dir[U_VIDEO_IDX] = 0;
    video_page_table[0] = 0;

    flush();
}

/* terminal_switch - CP5
 * 
 * parameter - none
 * return - none
 */
void terminal_switch(int32_t tid) {
    // if current process is process to be outputted
    if(tid == t_visible) {
        page_table[VIDEO_MEM_ADDR] = (uint32_t)VID_MEM | RW | PR;
    // if current process should not be outputted to screen, write to buffer
    } else {
        page_table[VIDEO_MEM_ADDR] = (uint32_t)(VID_MEM + (tid + 1) * _4_KB) | RW | PR;
    }
    flush();
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
