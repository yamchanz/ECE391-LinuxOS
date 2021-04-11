#include "paging.h"

/* paging_init - CP1
 * Initializes and enables paging. This includes the 4KB video memory inside
 * the first 4MB page, the 4MB Kernal page, as well as 1022 "not present"
 * 4MB pages.
 * parameter - none
 * return - none
 */
void paging_init(void) {
    int i, j;
    //  init page table
    for(j = 0; j < MAX_PAGE_NUMBER; j++) {
        if(j == VIDEO_MEM_PAGE_ADDR) {
            pt[j].page.present = 1;
            pt[j].page.read_write = 1;
            pt[j].page.user_sup = 0;
            pt[j].page.pwt = 0;
            pt[j].page.pcd = 0;
            pt[j].page.accessed = 0;
            pt[j].page.dirty = 0;
            pt[j].page.pat = 0;
            pt[j].page.glob = 0;
            pt[j].page.ignored3 = 0;
            pt[j].page.page_addr = VIDEO_MEM_PAGE_ADDR;
          }
        else {
            pt[j].not_present.present = 0;
            pt[j].not_present.ignored31 = 0;
        }
    }
    // init page directory
    for(i = 0; i < MAX_PAGE_NUMBER; i++) {
        // initalize first 4MB and video memory page (must be 4KB)
        if(i == 0) {
            pd[i].table.present = 1;
            pd[i].table.read_write = 1;
            pd[i].table.user_sup = 0;
            pd[i].table.pwt = 0;
            pd[i].table.pcd = 0;
            pd[i].table.accessed = 0;
            pd[i].table.ignored1 = 0;
            pd[i].table.ps = 0;
            pd[i].table.ignored4 = 0;
            pd[i].table.pt_addr = (unsigned long) pt >> 12;
        }
        // initialize 4MB kernel page
        else if(i == 1) {
            pd[i].page.present = 1;
            pd[i].page.read_write = 1;
            pd[i].page.user_sup = 0;
            pd[i].page.pwt = 0;
            pd[i].page.pcd = 0;
            pd[i].page.accessed = 0;
            pd[i].page.dirty = 0;
            pd[i].page.ps = 1;
            pd[i].page.glob = 0;
            pd[i].page.ignored3 = 0;
            pd[i].page.pat = 0;
            pd[i].page.exaddr = 0;
            pd[i].page.reserved5 = 0;
            pd[i].page.page_addr = KERNEL_PAGE_ADDR;
        }
        // initialize rest of 4MB pages
        else {
            pd[i].not_present.present = 0;
            pd[i].not_present.ignored31 = 0;
        }
    }
    // to turn on paging:
    // - set CR3 using mask 0xFFFFFC00 for address of page_directory,
    // (we want top 20 bits)
    // - set CR4.PSE bit (to enable 4MB pages)
    // - set CR0.PG bit, CR0.PE bit?
    asm volatile ("                                               \n\
        movl $pd, %%eax                                           \n\
        movl %%eax, %%cr3                                         \n\
        movl %%cr4, %%eax                                         \n\
        orl  $0x00000010, %%eax                                   \n\
        movl %%eax, %%cr4                                         \n\
        movl %%cr0, %%eax                                         \n\
        orl  $0x80000001, %%eax                                   \n\
        movl %%eax, %%cr0                                         \n\
        "                                                           \
        : /* no outputs */                                          \
        : /* no inputs */                                           \
        : "eax"                                                     \
    );
}

/* map_program - CP3
 * Maps the program that is currently running to the correct process given 
 * by the process number. 
 * parameter - process_number
 * return - none
 */
void map_program(uint32_t pid) {
    pd[ONETE].page.present = 1;
    // pd[0].page.present = 1;
    pd[ONETE].page.read_write = 1;
    pd[ONETE].page.user_sup = 1;
    pd[ONETE].page.pwt = 0;
    pd[ONETE].page.pcd = 0;
    pd[ONETE].page.accessed = 0;
    pd[ONETE].page.dirty = 0;
    pd[ONETE].page.ps = 1;
    pd[ONETE].page.glob = 0;
    pd[ONETE].page.ignored3 = 0;
    pd[ONETE].page.pat = 0;
    pd[ONETE].page.exaddr = 0;
    pd[ONETE].page.reserved5 = 0;
    pd[ONETE].page.page_addr = (_8_MB + _4_MB * pid) >> 22; // macro in filesys.h

    flush();
}

/* flush - CP2
 * Flushes TLB when altering paging
 * parameter - none
 * return - none
 */
void flush(void) {
  asm volatile ("                                               \n\
                                                                \n\
      movl %%cr3, %%eax                                         \n\
      movl %%eax, %%cr3                                         \n\
      "                                                           \
      : /* no outputs */                                          \
      : /* no inputs */                                           \
      : "eax"                                                     \
  );
}
