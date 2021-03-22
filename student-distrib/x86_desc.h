/* x86_desc.h - Defines for various x86 descriptors, descriptor tables,
 * and selectors
 * vim:ts=4 noexpandtab
 */

#ifndef _X86_DESC_H
#define _X86_DESC_H

#include "types.h"

/* Segment selector values */
#define KERNEL_CS   0x0010
#define KERNEL_DS   0x0018
#define USER_CS     0x0023
#define USER_DS     0x002B
#define KERNEL_TSS  0x0030
#define KERNEL_LDT  0x0038

/* Size of the task state segment (TSS) */
#define TSS_SIZE    104

/* Number of vectors in the interrupt descriptor table (IDT) */
#define NUM_VEC     256

/* Paging values */
#define MAX_PAGE_NUMBER   1024
#define BYTES_TO_ALIGN    4096

#ifndef ASM

/* This structure is used to load descriptor base registers
 * like the GDTR and IDTR */
typedef struct x86_desc {
    uint16_t padding;
    uint16_t size;
    uint32_t addr;
} x86_desc_t;

/* This is a segment descriptor.  It goes in the GDT. */
typedef struct seg_desc {
    union {
        uint32_t val[2];
        struct {
            uint16_t seg_lim_15_00;
            uint16_t base_15_00;
            uint8_t  base_23_16;
            uint32_t type          : 4;
            uint32_t sys           : 1;
            uint32_t dpl           : 2;
            uint32_t present       : 1;
            uint32_t seg_lim_19_16 : 4;
            uint32_t avail         : 1;
            uint32_t reserved      : 1;
            uint32_t opsize        : 1;
            uint32_t granularity   : 1;
            uint8_t  base_31_24;
        } __attribute__ ((packed));
    };
} seg_desc_t;

/* TSS structure */
typedef struct __attribute__((packed)) tss_t {
    uint16_t prev_task_link;
    uint16_t prev_task_link_pad;

    uint32_t esp0;
    uint16_t ss0;
    uint16_t ss0_pad;

    uint32_t esp1;
    uint16_t ss1;
    uint16_t ss1_pad;

    uint32_t esp2;
    uint16_t ss2;
    uint16_t ss2_pad;

    uint32_t cr3;

    uint32_t eip;
    uint32_t eflags;

    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;

    uint16_t es;
    uint16_t es_pad;

    uint16_t cs;
    uint16_t cs_pad;

    uint16_t ss;
    uint16_t ss_pad;

    uint16_t ds;
    uint16_t ds_pad;

    uint16_t fs;
    uint16_t fs_pad;

    uint16_t gs;
    uint16_t gs_pad;

    uint16_t ldt_segment_selector;
    uint16_t ldt_pad;

    uint16_t debug_trap : 1;
    uint16_t io_pad     : 15;
    uint16_t io_base_addr;
} tss_t;

/* PDE: page table
 * |  31  :  12  | 11:8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *   addr of PT    xxxx   0   x   A  PCD PWT U/S R/W  1
 */
typedef struct pde_PT {
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
typedef struct pde_4MB {
    struct {
        uint32_t present : 1;    // 0 present. must be 1 to map 4MB page
        uint32_t read_write : 1; // 1 read/write. 0 if writes not allowed
        uint32_t user_sup : 1;   // 2 user/supervisor. 0 if user entry not allowed
        uint32_t pwt : 1; // 3 page-level write thorough
        uint32_t pcd : 1; // 4 page-level cache disable
        uint32_t accessed : 1;   // 5 indicates if entry used for linear-address translation
        uint32_t dirty : 1; // 6 dirty; indicates whether if software has written to page referenced
        uint32_t ps : 1;    // 7 page size; must be 1
        uint32_t glob : 1;  // 8 global; if CR4.PGE = 1 determines whether the trans is global, otherwise ignore
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
typedef struct pte_4KB {
    struct {
        uint32_t present : 1;    // 0 present. must be 1 to map 4KB page
        uint32_t read_write : 1; // 1 read/write. 0 if writes not allowed
        uint32_t user_sup : 1;   // 2 user/supervisor. 0 if user entry not allowed
        uint32_t pwt : 1; // 3 page-level write thorough
        uint32_t pcd : 1; // 4 page-level cache disable
        uint32_t accessed : 1;   // 5 accessed; indicates whether software has accessed page referenced
        uint32_t dirty : 1; // 6 dirty; indicates whether if software has written to page referenced
        uint32_t pat : 1;   // 7 if PAT supported, indirectly determines memory type, otherwise 0.
        uint32_t glob : 1;  // 8 global; if CR4.PGE = 1 determines whether the trans is global
        uint32_t ignored3 : 3;  // 9:11 ignored
        uint32_t page_addr : 20;// 12:31 physical address of 4KB page referenced by entry
    } __attribute__ ((packed));
} pte_4KB;

/* PTE / PDE: not present
 * |  31           :             1 | 0 |
 *  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  0
 */
typedef struct pe_NA {
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

/* Some external descriptors declared in .S files */
extern x86_desc_t gdt_desc;

extern uint16_t ldt_desc;
extern uint32_t ldt_size;
extern seg_desc_t ldt_desc_ptr;
extern seg_desc_t gdt_ptr;
extern uint32_t ldt;

extern uint32_t tss_size;
extern seg_desc_t tss_desc_ptr;
extern tss_t tss;

page_directory pd __attribute__((aligned(BYTES_TO_ALIGN)));
page_table pt __attribute__((aligned(BYTES_TO_ALIGN)));

/* Sets runtime-settable parameters in the GDT entry for the LDT */
#define SET_LDT_PARAMS(str, addr, lim)                          \
do {                                                            \
    str.base_31_24 = ((uint32_t)(addr) & 0xFF000000) >> 24;     \
    str.base_23_16 = ((uint32_t)(addr) & 0x00FF0000) >> 16;     \
    str.base_15_00 = (uint32_t)(addr) & 0x0000FFFF;             \
    str.seg_lim_19_16 = ((lim) & 0x000F0000) >> 16;             \
    str.seg_lim_15_00 = (lim) & 0x0000FFFF;                     \
} while (0)

/* Sets runtime parameters for the TSS */
#define SET_TSS_PARAMS(str, addr, lim)                          \
do {                                                            \
    str.base_31_24 = ((uint32_t)(addr) & 0xFF000000) >> 24;     \
    str.base_23_16 = ((uint32_t)(addr) & 0x00FF0000) >> 16;     \
    str.base_15_00 = (uint32_t)(addr) & 0x0000FFFF;             \
    str.seg_lim_19_16 = ((lim) & 0x000F0000) >> 16;             \
    str.seg_lim_15_00 = (lim) & 0x0000FFFF;                     \
} while (0)

/* An interrupt descriptor entry (goes into the IDT) */
typedef union idt_desc_t {
    uint32_t val[2];
    struct {
        uint16_t offset_15_00;
        uint16_t seg_selector;
        uint8_t  reserved4;
        uint32_t reserved3 : 1;
        uint32_t reserved2 : 1;
        uint32_t reserved1 : 1;
        uint32_t size      : 1;
        uint32_t reserved0 : 1;
        uint32_t dpl       : 2;
        uint32_t present   : 1;
        uint16_t offset_31_16;
    } __attribute__ ((packed));
} idt_desc_t;

/* The IDT itself (declared in x86_desc.S */
extern idt_desc_t idt[NUM_VEC];
/* The descriptor used to load the IDTR */
extern x86_desc_t idt_desc_ptr;

/* Sets runtime parameters for an IDT entry */
#define SET_IDT_ENTRY(str, handler)                              \
do {                                                             \
    str.offset_31_16 = ((uint32_t)(handler) & 0xFFFF0000) >> 16; \
    str.offset_15_00 = ((uint32_t)(handler) & 0xFFFF);           \
} while (0)

/* Load task register.  This macro takes a 16-bit index into the GDT,
 * which points to the TSS entry.  x86 then reads the GDT's TSS
 * descriptor and loads the base address specified in that descriptor
 * into the task register */
#define ltr(desc)                       \
do {                                    \
    asm volatile ("ltr %w0"             \
            :                           \
            : "r" (desc)                \
            : "memory", "cc"            \
    );                                  \
} while (0)

/* Load the interrupt descriptor table (IDT).  This macro takes a 32-bit
 * address which points to a 6-byte structure.  The 6-byte structure
 * (defined as "struct x86_desc" above) contains a 2-byte size field
 * specifying the size of the IDT, and a 4-byte address field specifying
 * the base address of the IDT. */
#define lidt(desc)                      \
do {                                    \
    asm volatile ("lidt (%0)"           \
            :                           \
            : "g" (desc)                \
            : "memory"                  \
    );                                  \
} while (0)

/* Load the local descriptor table (LDT) register.  This macro takes a
 * 16-bit index into the GDT, which points to the LDT entry.  x86 then
 * reads the GDT's LDT descriptor and loads the base address specified
 * in that descriptor into the LDT register */
#define lldt(desc)                      \
do {                                    \
    asm volatile ("lldt %%ax"           \
            :                           \
            : "a" (desc)                \
            : "memory"                  \
    );                                  \
} while (0)

#endif /* ASM */

#endif /* _x86_DESC_H */
