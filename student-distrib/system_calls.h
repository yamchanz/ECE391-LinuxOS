#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H
#include "lib.h"
#include "filesys.h"
#include "rtc.h"
#include "terminal.h"
#include "x86_desc.h"
#include "paging.h"
#include "system_calls_wrapper.h"
#include "scheduler.h"

#define PCB_ADDR_MASK        0xFFFFE000
#define PROG_IMG_ADDR        0x8048000
#define ONETE                0x8000001
#define BOTTOM_USER_STACK    0x8400000 - 4

#define ENTRY_POINT_START    24
#define CMD_MAX_LEN          32

#define FOUR_BYTE            4
#define MAX_KBUFF_LEN        128

#define FD_START             2
#define FD_MAX               8

#define RTC_FTYPE            0
#define DIR_FTYPE            1
#define FILE_FTYPE           2

// file operations containing pointers to functions for that type of file
typedef struct __attribute__((packed)) {
    int32_t(* open)(const uint8_t* filename);
    int32_t(* close)(int32_t fd);
    int32_t(* read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t(* write)(int32_t fd, const void* buf, int32_t nbytes);
} file_ops_t;

// file descriptor - used in PCB to store FDs
typedef struct __attribute__((packed)) {
    file_ops_t *fops_ptr;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
} file_desc_t;

// PCB
typedef struct __attribute__((packed)){
    file_desc_t fd_table[FD_MAX];
    // insert information needed to go back to parent program below
    // get ESP and EBP from address
    uint32_t esp;
    uint32_t ebp;
    uint32_t pid;
    uint32_t parent_pid; // we may need this?
    uint16_t ss0;
    uint32_t esp0;
    uint8_t arg[MAX_KBUFF_LEN];
} pcb_t;

// count of processes running (previously pid)
int num_processes;
// array of free processes. -1 if free, otherwise stores terminal id (only 1 for now)
int process_status[PROCESS_COUNT];

extern file_ops_t fops_rtc;
extern file_ops_t fops_dir;
extern file_ops_t fops_file;
extern file_ops_t std_in;
extern file_ops_t std_out;

extern pcb_t* get_pcb(int pid_in);
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
