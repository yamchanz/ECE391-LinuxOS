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
    page_table[VIDEO_MEM_ADDR] |= RW | PR;
    // kernel space video
    page_dir[K_VIDEO_IDX] = ((uint32_t)page_table) | RW | PR;
    page_dir[KERNEL_IDX] = _4_MB | PAGE_4MB | RW | PR;
    
    // to turn on paging:
    // - set CR3 using mask 0xFFFFFC00 for address of page_directory,
    // (we want top 20 bits)
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
    page_dir[U_VIDEO_IDX] = (uint32_t)video_page_table | USR | RW | PR;
    video_page_table[0] = (VIDEO_MEM_ADDR << 12) | USR | RW | PR;
}

/* unmap_video - CP4
 * Unmaps the video memory page. Called in halt when the video memory flag is set
 * parameter - void
 * return - none
 */
void unmap_video(void){
    page_dir[U_VIDEO_IDX] = 0;
    video_page_table[0] = 0;

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
