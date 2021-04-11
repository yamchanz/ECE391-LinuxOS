#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H
#include "lib.h"
#include "filesys.h"
#include "rtc.h"
#include "terminal.h"
#include "x86_desc.h"
#include "paging.h"

#define PCB_ADDR_MASK   0xFFFFE000
#define PROG_IMG_ADDR   0x8048000
#define ONETE           0X8000001

typedef struct __attribute__((packed)) pcb {
    file_desc_t fd_table[8];
    // insert information needed to go back to parent program below
    // get ESP and EBP from address
    uint32_t parent_esp;
    uint32_t parent_ebp;
    uint32_t esp;
    uint32_t ebp;
    uint8_t cur_pid;
    uint8_t parent_pid; // we may need this?
    uint32_t esp0;
} pcb_t;

extern file_ops_t fops_rtc;
extern file_ops_t fops_dir;
extern file_ops_t fops_file;
extern file_ops_t std_in;
extern file_ops_t std_out;

extern int32_t bad_call();
extern int32_t halt (uint8_t status);
extern int32_t execute (const uint8_t* command);
extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open (const uint8_t* filename);
extern int32_t close (int32_t fd);
extern int32_t getargs (uint8_t* buf, int32_t nbytes);
extern int32_t vidmap (uint8_t** screen_start);
extern int32_t set_handler (int32_t signum, void* handler_address);
extern int32_t sigreturn (void);


#endif
