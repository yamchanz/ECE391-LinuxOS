/* from intel IA-32 architecture manual
 *
 * paging behavior defined by following control bits:
 * - WP(b16) and PG(b31) flags in CR0
 * - PSE(b4), PAE(b5), PGE(b7), PCIDE(b17), SMEP(b20),
 *   SMAP(b21), PKE(b22) flags in CR4
 * - LME(b8), NXE(b11) flags in IA32_EFER_MSR
 * - AC(b18) flag in EFLAGS
 * Must flush CR3 to indiciate we need to refresh TLB every time there is
 * change in address space
 */

 /* when CPU is asked to access a virtual mem address, uses highest 10b
  * [31:22] to index into directory, next 10b [21:12] to index into the
  * page table, lowest 12 bits [11:0] are used as offset to index a
  * byte in a page
  */

#include "types.h"

// #define NUM_PROCESSES     6
#define MAX_PAGE_NUMBER   1024
#define BYTES_TO_ALIGN    4096
#define VIDEO_MEM_PAGE_ADDR  0  // need to fix
#define KERNEL_PAGE_ADDR     1

/* PDE: page table
 * |  31  :  12  | 11:8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *   addr of PT    xxxx   0   x   A  PCD PWT U/S R/W  1
 */
typedef union pde_PT {
    uint32_t val;
    struct {
        uint32_t present : 1;    // 0 present. must be 1 to reference page table
        uint32_t read_write : 1; // 1 read/write. 0 if writes not allowed
        uint32_t user_sup : 1;   // 2 user/supervisor. 0 if user entry not allowed
        uint32_t pwt : 1; // 3 page-level write thorough
        uint32_t pcd : 1; // 4 page-level cache disable
        uint32_t accessed : 1;   // 5 indicates if entry used for linear-address translation
        uint32_t ignored1 : 1;   // 6 ignored
        uint32_t ps : 1;         // 7 if CR4.PSE = 1, must be 0
        uint32_t ignored4 : 4;   // 8:11 ignored
        uint32_t pt_addr : 20;   // 12:31 physical addr of 4KB aligned page table reference
    } __attribute__ ((packed));
} pde_PT;

/* PDE: 4MB page
 * | 31:22 | 21:17 | 16:13 | 12 | 11:9 |8|7|6|5|4|3|2|1|0|
 *   addr  reserved ex-addr PAT   xxx   G 1 D A p p u r 1
 */
typedef union pde_4MB {
    uint32_t val;
    struct {
        uint32_t present : 1;    // 0 present. must be 1 to map 4MB page
        uint32_t read_write : 1; // 1 read/write. 0 if writes not allowed
        uint32_t user_sup : 1;   // 2 user/supervisor. 0 if user entry not allowed
        uint32_t pwt : 1; // 3 page-level write thorough
        uint32_t pcd : 1; // 4 page-level cache disable
        uint32_t accessed : 1;   // 5 indicates if entry used for linear-address translation
        uint32_t dirty : 1; // 6 dirty; indicates whether if software has written to page referenced
        uint32_t ps : 1;    // 7 page size; must be 1
        uint32_t glob : 1;  // 8 global; if CR4.PSE = 1 determines whether the trans is global
        uint32_t ignored3 : 3;  // 11:9 ignored
        uint32_t pat : 1; // 12 if PAT supported, indirectly determines memory type, otherwise 0
        uint32_t exaddr : 4;    // 13:16 row does not apply unless PSE-36 supported
        uint32_t reserved5 : 5; // 17:21 must be 0
        uint32_t page_addr : 10;// 22:31 bits [31:22] of physical address of 4MB page reference
    } __attribute__ ((packed));
} pde_4MB;

/* PTE: 4KB page
 * |  31  :  12  |11:9| 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *      addr       xx   G  PAT  D   A  PCD PWT U/S R/W  1
 */
typedef union pte_4KB {
    uint32_t val;
    struct {
        uint32_t present : 1;    // 0 present. must be 1 to map 4KB page
        uint32_t read_write : 1; // 1 read/write. 0 if writes not allowed
        uint32_t user_sup : 1;   // 2 user/supervisor. 0 if user entry not allowed
        uint32_t pwt : 1; // 3 page-level write thorough
        uint32_t pcd : 1; // 4 page-level cache disable
        uint32_t accessed : 1;   // 5 accessed; indicates whether software has accessed page referenced
        uint32_t dirty : 1; // 6 dirty; indicates whether if software has written to page referenced
        uint32_t pat : 1;   // 7 if PAT supported, indirectly determines memory type, otherwise 0.
        uint32_t glob : 1;  // 8 global; if CR4.PSE = 1 determines whether the trans is global
        uint32_t ignored3 : 3;  // 9:11 ignored
        uint32_t page_addr : 20;// 12:31 physical address of 4KB page referenced by entry
    } __attribute__ ((packed));
} pte_4KB;

/* PTE / PDE: not present
 * |  31           :             1 | 0 |
 *  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  0
 */
typedef union pe_NA {
    uint32_t val;
    struct {
        uint32_t present : 1;    // 0 present. 0 means page not there
        uint32_t ignored31 : 31; // 1:31 ignored
    } __attribute__ ((packed));
} pe_NA;

/* page directory has three possibilities */
typedef union pde {
    pde_PT table;
    pde_4MB page;
    pe_NA not_present;
} pde;

/* page table has two possiblities */
typedef union pte {
    pte_4KB page;
    pe_NA not_present;
} pte;

/* page directory skeleton */
typedef struct page_directory {
    pde entry[MAX_PAGE_NUMBER];
} page_directory;

/* page table skeleton */
typedef struct page_table {
    pte entry[MAX_PAGE_NUMBER];
} page_table;

extern page_directory pd __attribute__((aligned(BYTES_TO_ALIGN)));
extern page_table pt __attribute__((aligned(BYTES_TO_ALIGN)));

/* initializes pages */
extern void init_paging(void);

/* flushes TLB when memory map altered */
extern void flush(void);
