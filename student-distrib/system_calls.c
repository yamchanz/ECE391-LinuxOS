#include "lib.h"
#include "system_calls.h"
#include "filesys.h"

int32_t bad_call();

// static tables with function pointers for each file type
file_ops_t fops_rtc = {rtc_open, rtc_close, rtc_read, rtc_write};
file_ops_t fops_dir = {dir_open, dir_close, dir_read, dir_write};
file_ops_t fops_file = {file_open, file_close, file_read, file_write};
file_ops_t std_in = {bad_call, terminal_read, bad_call, bad_call};
file_ops_t std_out = {bad_call, bad_call, terminal_write, bad_call};

int32_t bad_call() {
    return -1;
}
int pid;

void pcb_init(pcb_t* pcb, int32_t pid) {
    file_desc_t stdin;
    stdin.fops_ptr = &std_in;
    stdin.inode = NULL;
    stdin.file_pos = NULL;
    stdin.flags = 1;

    file_desc_t stdout;
    stdout.fops_ptr = &std_out;
    stdout.inode = NULL;
    stdout.file_pos = NULL;
    stdout.flags = 1;

    pcb->fd_table[0] = stdin;
    pcb->fd_table[1] = stdout;
    
    pcb->cur_pid = pid;
    pcb->parent_pid = 0;
    // pcb->esp0 = tss.esp0;

    // write esp and ebp into pcb struct        NEED TO DOUBLE CHECK SYNTAX
    asm volatile("                  \n\
                    movl %%esp, %0  \n\
                    movl %%ebp, %1  \n\
                    "
                :"=r" (pcb->esp), "=r" (pcb->ebp)
    );

    return;
}

void get_pcb(pcb_t* address){
    asm volatile("andl %%esp, %%ebx\n"
                    :"=b"(address)
                    :"b"(PCB_ADDR_MASK)
                    :"cc"
                    );
}

int32_t halt (uint8_t status) {
    int i;
    pcb_t* pcb;
    
    cli();
    get_pcb(pcb);

    // restore parent data
    pid--;

    // restore parent paging
    map_program(pcb->parent_pid); // flushes tlb

    // clear fd
    for(i = 2; i < 8; i++) { 
        close(i);
    }

    // write parent process' info back to TSS(esp0)
    // tss.esp0 = parent->esp0;
    sti();
    
    // jump to execute return
    asm volatile("                  \n\
                    movl %0, %%esp  \n\
                    movl %1, %%ebp  \n\
                    "
                :"=r" (pcb->esp), "=r" (pcb->ebp)
    );
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

    while (command[cmd_idx]!= ' ' || command[cmd_idx] != '\0') {
        cmd_idx++;
    }

    if(cmd_idx >32){
        return -1;
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

    // getting the entry point from 24 - 27 (27 -> 24)
    read_data(search->inode, 24,buffer,4);
    entry_point = *((uint32_t*)buffer);

    //set up paging
    map_program(pid);

    // write file data into program image (virtual address)
    // read_data(search->inode,0,(uint8_t *)PROG_IMG_ADDR, search->inode.length); inode is not an object? did you mean to use inode_t?
    read_data(search->inode,0,(uint8_t *)PROG_IMG_ADDR, search->inode);

    // create pcb for this process
    pcb_t* pcb;
    pcb_init(pcb, pid);
    

    pid++;

    sti();
    // prepare for context switch
    // push DS, ESP, EFLAGS, CS, EIP
    asm volatile(
        "cli; \n\
        # push DS                       \n\
        xorl %%eax, %%eax                 \n\
        movw $USER_DS, %%ax              \n\
        pushl %%eax                      \n\
                                        \n\
        # push ESP                      \n\
        pushl %0                        \n\
                                        \n\
        # push EFLAGS                   \n\
        pushfl                          \n\
                                        \n\
        # push CS                       \n\
        xorl %%eax, %%eax                 \n\
        movw $USER_CS, %%ax              \n\
        pushl %%eax                      \n\
                                        \n\
        # push EIP (entry_point)        \n\
        pushl %1                        \n\
                                        \n\
        // push IRET context to kernel stack \n\
        iret                            \n\
        "
        :"=r" (pcb->esp), "=r" (entry_point)
    );
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
    return (int32_t)readpcb->fd_table[fd].fops_ptr->read(fd, buf, nbytes);
}

// write data to either terminal or RTC
int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
    pcb_t *writepcb;
    get_pcb(writepcb);
    // error handling - FD in array, buf not empty, nbytes >= 0
    if(fd >= 8 || fd < 0 || buf == NULL || nbytes <= 0) {
        return -1;
    }
    // find fd in fd_table
    return (int32_t)writepcb->fd_table[fd].fops_ptr->write(fd, buf, nbytes);
}
int32_t open (const uint8_t* filename) {
    pcb_t *pcb;
    get_pcb(pcb);
    // open file with read_dentry_by_name - writes file type into file_block
    dentry_t* file_block;
    if(read_dentry_by_name(filename, file_block) != 0) {
        return -1;
    }
    uint32_t f_type = file_block->file_type;

    // find index to put file descriptor in
    int32_t fd = 2;
    while(fd < 8) {
        // write if empty fd or if fd not occupied
        // if(pcb->fd_table[fd] == NULL || pcb->fd_table[fd].flags == 0) {
        // NULL is set to 0 so can't be compared to fd_table object, we should use other methods instead
        if(pcb->fd_table[fd].flags == 0) {
            switch(f_type){
                case 0: //rtc
                    pcb->fd_table[fd].fops_ptr = &fops_rtc;
                    pcb->fd_table[fd].inode = NULL;
                case 1: // dir
                    pcb->fd_table[fd].fops_ptr = &fops_dir;
                    pcb->fd_table[fd].inode = NULL;
                case 2: // file
                    pcb->fd_table[fd].fops_ptr = &fops_file;
                    pcb->fd_table[fd].inode = file_block->inode;
                default:
                    return -1;
                pcb->fd_table[fd].file_pos = 0;
                pcb->fd_table[fd].flags = 1; // set to occupied

                return 0;
            }

        }
        fd++;
    }

    return -1;
}
int32_t close (int32_t fd) {
    // error handling - fd index not in array
    // fd < 2?
    if(fd > 7 || fd < 0) {
        return -1;
    }
    pcb_t *pcb;
    get_pcb(pcb);

    //already not in use we dont need to close
    if(pcb->fd_table[fd].flags == 0){
        return -1;
    } else {
        pcb->fd_table[fd].flags = 0;
    }
    // find fd in fd_table and close
    pcb->fd_table[fd].fops_ptr->close(fd);

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
