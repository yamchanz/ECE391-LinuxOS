#ifndef _PAGING_H
#define _PAGING_H

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
#include "x86_desc.h"
#include "filesys.h"
#include "system_calls.h"
#include "terminal.h"

#define VIDEO_MEM_ADDR      184
#define K_VIDEO_IDX         0
#define KERNEL_IDX          1
#define PROGRAM_IDX         32
#define U_VIDEO_IDX         35
#define PR                  0x01
#define RW                  0x02
#define USR                 0x04
#define PAGE_4MB            0x80

/* maps running program to virutal address 128MB */
extern void map_program(uint32_t pid);
/* initializes pages */
extern void paging_init(void);
/* helps user program write to video memory */
extern void map_video(void);
/* unmaps page after program is finished writing */
extern void unmap_video(void);
/* flushes TLB when memory map altered */
void flush(void);

#endif
