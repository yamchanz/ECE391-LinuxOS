#include "lib.h"
#include "system_calls.h"
#include "filesys.h"

int pid[6];

int32_t pid_init(){
    int i;
    for(i =0;i<6;i++){
        pid[i] = 0;
    }

    return 0;
}

void get_pcb(pcb_t* address){
    asm volatile("andl %%esp, %%ebx\n"
                    :"=b"(address)
                    :"b"(PCB_ADDR_MASK)
                    :"cc"
                    );
}

int32_t halt (uint8_t status) {
    return 0;
}
int32_t execute (const uint8_t* command) {
    return 0;
}
int32_t read (int32_t fd, void* buf, int32_t nbytes) {
    // error handling - fd index not in array
    if(fd >= 8 || fd < 2) {
        return -1;
    }
    // make sure the file is opened before read and buffer is not empty
    if()

    // find fd in fd_table
    return pcb.file_table[fd].file_ops_pointer.read(fd, buf, nbytes);
    
}

// write data to either terminal or RTC
int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
    // error handling - fd index not in array
    if(fd >= 8 || fd < 2) {
        return -1;
    }
    // find fd in fd_table
    return pcb.file_table[fd].file_ops_pointer.write(fd, buf, nbytes);
}
int32_t open (const uint8_t* filename) {
    // open file with error handling
    file_open(filename);
    // setup file descriptor
    file_desc_t file_desc;

    // find index to put file descriptor in
    int idx = 0;
    
    // add file descriptor into fd_table in PCB
    pcb.file_table[idx] = file_desc;

    return 0;
}
int32_t close (int32_t fd) {
    // error handling - fd index not in array
    if(fd >= 8 || fd < 2) {
        return -1;
    }
    // find fd in fd_table

    // close fd by removing

    return 0;
}
int32_t getargs (uint8_t* buf, int32_t nbytes) {
    return 0;
}
int32_t vidmap (uint8_t** screen_start) {
    return 0;
}
int32_t set_handler (int32_t signum, void* handler_address) {
    return -1;
}
int32_t sigreturn (void) {
    return -1;
}