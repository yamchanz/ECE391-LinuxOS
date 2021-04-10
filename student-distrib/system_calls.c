#include "lib.h"
#include "system_calls.h"
#include "filesys.h"

// static tables with function pointers for each file type
file_ops_t fops_rtc[] = {rtc_open, rtc_close, rtc_read, rtc_write};
file_ops_t fops_dir[] = {dir_open, dir_close, dir_read, dir_write};
file_ops_t fops_file[] = {file_open, file_close, file_read, file_write};
file_ops_t std_in[] = {bad_call, read_terminal, bad_call, bad_call};
file_ops_t std_out[] = {bad_call, bad_call, write_terminal, bad_call};

int32_t bad_call() {
    return -1;
}
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
    cli();

    return 0;
}
int32_t execute (const uint8_t* command) {
    cli();
    uint8_t buffer[4], exec[5];
    uint8_t cmd_idx = 0;
    uint32_t entry_point;
    int i;
    if (command == NULL){
        return -1;
    }

    while (command[cmd_idx]!= ' ') {
        cmd_idx++;
    }

    for(i = 0; i<cmd_idx; i++) {
        exec[i] = command[i];
    } 

    // checking the magic number to make sure its executable.
    dentry_t *search;
    if(read_dentry_by_name((uint8_t*)exec, search)==0){
        read_data(search->inode,0,buffer, 4);
        if(buffer[0] != 0x7f || buffer[1]!= 0x45 || buffer[2]!= 0x4c || buffer[3]!=0x46){
            return -1;
        }
    } else {
        return -1;
    }

    // getting the entry point from 24 - 27
    read_data(search->inode, 24,buffer,4);
    entry_point = *((uint32_t*)buffer);
    

    sti();
    return 0;
}
int32_t read (int32_t fd, void* buf, int32_t nbytes) {
    // get a pcb to perform read operation
    pcb_t *readpcb;
    get_pcb(readpcb);
    
    // error handling - FD in array, buf not empty, nbytes >= 0
    if(fd > 7 || fd < 0 || buf == NULL || nbytes < 0) {
        return -1;
    }

    // find fd in fd_table
    return (int32_t)readpcb->file_table[fd].fops_ptr.read(fd, buf, nbytes);
}

// write data to either terminal or RTC
int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
    pcb_t *writepcb;
    get_pcb(writepcb);
    // error handling - FD in array, buf not empty, nbytes >= 0
    if(fd >= 8 || fd < 2 || buf == NULL || nbytes <= 0) {
        return -1;
    }
    // find fd in fd_table
    return (int32_t)writepcb->file_table[fd].fops_ptr.write(fd, buf, nbytes);
}
int32_t open (const uint8_t* filename) {
    // open file with error handling
    if(file_open(filename) != 0){
        return -1;
    }

    // find index to put file descriptor in
    int32_t fd = 2;
    while(fd < 8) {
        // empty fd, add data in
        if(pcb->file_table[fd].fops_ptr == NULL) {
            pcb->file_table[fd].fops_ptr = fops_file;

        }
        fd++;
    }

    return 0;
}
int32_t close (int32_t fd) {
    // error handling - fd index not in array
    if(fd > 7 || fd < 0) {
        return -1;
    }
    pcb_t *pcb;
    get_pcb(pcb);

    //already not in use we dont need to close
    if(pcb->file_table[fd].flags == 0){
        return -1;
    } else {
        pcb->file_table[fd].flags = 0;
    }
    // find fd in fd_table and close
    pcb->file_table[fd].fops_ptr.close(fd);

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