#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H
#include "lib.h"
#include "filesys.h"
#include "rtc.h"
#include "terminal.h"

typedef struct pcb {
    struct {
        file_desc_t fd_table[8];
        // insert information needed to go back to parent program below
        
    } __attribute__ ((packed));
} pcb;

extern file_ops_t fops_rtc;
extern file_ops_t fops_dir;
extern file_ops_t fops_file;
extern file_ops_t std_in;
extern file_ops_t std_out;


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
