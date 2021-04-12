#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H
#include "lib.h"
#include "filesys.h"
#include "rtc.h"
#include "terminal.h"
#include "x86_desc.h"
#include "paging.h"
#include "context_switch.h"

#define PCB_ADDR_MASK   0xFFFFE000
#define PROG_IMG_ADDR   0x8048000
#define ONETE           0X8000001
#define BOTTOM_USER_STACK 0x8400000
#define ENTRY_POINT_START   24
#define CMD_MAX_LEN     32
#define FOUR_BYTE       4
#define FD_START        2
#define FD_MAX          8
#define RTC_FTYPE       0
#define DIR_FTYPE       1
#define FILE_FTYPE      2

typedef struct __attribute__((packed)){
    file_desc_t fd_table[FD_MAX];
    // insert information needed to go back to parent program below
    // get ESP and EBP from address
    uint32_t parent_esp;
    uint32_t parent_ebp;
    uint32_t esp;
    uint32_t ebp;
    uint8_t pid;
    uint8_t parent_pid; // we may need this?
    uint16_t ss0;
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
