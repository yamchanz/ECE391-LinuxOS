#include "paging.h"

// linker issue?
#define VIDEO_MEM_PAGE_ADDR  0
#define PAGE_TABLE_ADDR      0
#define PAGE_DIREC_ADDR      0
#define KERNEL_PAGE_ADDR     0
// CR3_ADDR_MASK     0xFFFFF000
// CR4_PAE           0x00000010
// CR0_PG            0x80000000

page_directory pd __attribute__((aligned(BYTES_TO_ALIGN)));
page_table pt __attribute__((aligned(BYTES_TO_ALIGN)));
int pd_addr;

/* init_paging - CP1
 * Initializes and enables paging. This includes the 4KB video memory inside
 * the first 4MB page, the 4MB Kernal page, as well as 1022 "not present"
 * 4MB pages.
 * parameter - none
 * return - none
 */
void init_paging(void) {
    int i, j;
    for(i = 0; i < MAX_PAGE_NUMBER; i++) {
        /* initalize first 4MB and video memory page (must be 4KB) */
        if(i == 0) {
            for(j = 0; j < MAX_PAGE_NUMBER; j++) {
                if(1) { // video memory portion
                    pt.entry[j].page.present = 1;
                    pt.entry[j].page.read_write = 0;
                    pt.entry[j].page.user_sup = 0;
                    pt.entry[j].page.pwt = 0;
                    pt.entry[j].page.pcd = 0;
                    pt.entry[j].page.accessed = 0;
                    pt.entry[j].page.dirty = 0;
                    pt.entry[j].page.pat = 0;
                    pt.entry[j].page.glob = 0;
                    pt.entry[j].page.ignored3 = 0;
                    pt.entry[j].page.page_addr = VIDEO_MEM_PAGE_ADDR;
                  }
                else {
                    pt.entry[j].not_present.present = 0;
                    pt.entry[j].not_present.ignored31 = 0;
                }
            }
            pd.entry[i].table.present = 1;
            pd.entry[i].table.read_write = 0;
            pd.entry[i].table.user_sup = 0;
            pd.entry[i].table.pwt = 0;
            pd.entry[i].table.pcd = 0;
            pd.entry[i].table.accessed = 0;
            pd.entry[i].table.ignored1 = 0;
            pd.entry[i].table.pt_addr = PAGE_TABLE_ADDR;
        }
        /* initialize 4MB kernel page */
        else if(i == 1) {
            pd.entry[i].page.present = 1;
            pd.entry[i].page.read_write = 0;
            pd.entry[i].page.user_sup = 0;
            pd.entry[i].page.pwt = 0;
            pd.entry[i].page.pcd = 0;
            pd.entry[i].page.accessed = 0;
            pd.entry[i].page.dirty = 0;
            pd.entry[i].page.ps = 1;
            pd.entry[i].page.glob = 1;
            pd.entry[i].page.ignored3 = 0;
            pd.entry[i].page.pat = 0;
            pd.entry[i].page.exaddr = 0;
            pd.entry[i].page.reserved5 = 0;
            pd.entry[i].page.page_addr = KERNEL_PAGE_ADDR;
        }
        /* initialize rest of 4MB pages */
        else {
            pd.entry[i].not_present.present = 0;
            pd.entry[i].not_present.ignored31 = 0;
        }
    }

    // to turn on paging:
    // - set CR3
    // - set CR4.PAE bit
    // - set CR0.PG bit
    asm volatile ("                                               \n\
        movl $pd_addr, %%eax                                      \n\
        andl $0xFFFFF000, %%eax                                   \n\
        movl %%eax, %%cr3                                         \n\
        movl $0x00000010, %%eax                                   \n\
        movl %%eax, %%cr4                                         \n\
        movl $0x80000000, %%eax                                   \n\
        movl %%eax, %%cr0                                         \n\
        "                                                           \
        : /* no outputs */                                          \
        : /* no inputs */                                           \
        : "eax", "cc"                                               \
    );
}

void flush(void) {
  asm volatile ("                                               \n\
                                                                \n\
      movl %%cr3, %%eax                                         \n\
      movl %%eax, %%cr3                                         \n\
      "                                                           \
      : /* no outputs */                                          \
      : /* no inputs */                                           \
      : "eax", "cc"                                               \
  );
}
