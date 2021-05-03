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
#include "idt_handlers.h"

#define PROG_IMG_ADDR        0x8048000
#define PROCESS_COUNT        6
#define MAX_PROC_PER_TERM    3

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
    uint32_t cur_esp;
    uint32_t cur_ebp;
    uint32_t pid;
    uint32_t parent_pid; // we may need this?
    uint16_t ss0;
    uint32_t esp0;
    uint8_t arg[MAX_KBUFF_LEN];
} pcb_t;

// array of free processes. -1 if free, otherwise stores terminal id (only 1 for now)
int process_status[PROCESS_COUNT];

// file operations set for rtc
extern file_ops_t fops_rtc;
// file operations set for directory
extern file_ops_t fops_dir;
// file operations set for files
extern file_ops_t fops_file;
// file operations set for terminal read
extern file_ops_t std_in;
// file operations set for terminal write
extern file_ops_t std_out;

// gets the pcb address of where the given pid is
extern pcb_t* get_pcb(int pid_in);
// place holder for non-existent system calls in the function jumptable
extern int32_t bad_call();
// After execute is called, must call halt. Halts the program.
extern int32_t halt (uint8_t status);
// Executes a file
extern int32_t execute (const uint8_t* command);
// reads data from keyboard, file, RTC, or directory
extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
// write data to either terminal or RTC
extern int32_t write (int32_t fd, const void* buf, int32_t nbytes);
// provides access to file system - adds file descriptor in PCB, sets up data needed to handle file
extern int32_t open (const uint8_t* filename);
// closes specified fd from fd table
extern int32_t close (int32_t fd);
// copies arguments passed in from execute in the pcb into a user-level buffer
extern int32_t getargs (uint8_t* buf, int32_t nbytes);
// maps text mode video memory into user space at virtual address 140MB and creates page
extern int32_t vidmap (uint8_t** screen_start);
// not used
extern int32_t set_handler (int32_t signum, void* handler_address);
// not used
extern int32_t sigreturn (void);


#endif
