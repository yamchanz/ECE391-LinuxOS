#include "system_calls.h"

// flag to remove vidmap page
int vidmap_page_flag = 0;

// static tables with function pointers for each file type
file_ops_t fops_rtc = {rtc_open, rtc_close, rtc_read, rtc_write};
file_ops_t fops_dir = {dir_open, dir_close, dir_read, dir_write};
file_ops_t fops_file = {file_open, file_close, file_read, file_write};
file_ops_t std_in = {bad_call, bad_call, terminal_read, bad_call};
file_ops_t std_out = {bad_call, bad_call, bad_call, terminal_write};

/* bad_call - CP3
 * Returns -1 for a bad call.
 * parameters - none
 * returns - none
 */
int32_t bad_call(void) {
    return -1;
}

/* pcb_init - CP3
 * Initaliazes a new pcb struct every time a new process has started.
 * parameters - pcb : pointer to struct we want to fill in
 * returns - none
 * side effects - fills in the pcb struct
 */
void pcb_init(pcb_t *pcb) {
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

    //update pid in the pcb
    pcb->pid = t[t_run].cur_pid;
// FIX! check if current process is base shell
    if(t[t_run].status == 0) {
        t[t_run].status = 1;
        pcb->parent_pid = pcb->pid;
    } else{
        pcb->parent_pid = t[t_run].cur_pid;
        t[t_run].cur_pid = pcb->pid;
    } 
    
    // previously
    // if(shell_init || t[t_run].cur_pid_idx == 0) pcb->parent_pid = pcb->pid;
    // else pcb->parent_pid = t[t_run].pid_[t[t_run].cur_pid_idx-1];

    pcb->esp0 = _8_MB - _8_KB * pcb->parent_pid - FOUR_BYTE;
    pcb->ss0 = KERNEL_DS;

    return;
}

/* get_pcb - CP3
 * Fills in given struct with correct pcb info
 * parameters - pid_in : pid num of pcb struct to find
 * returns - none
 */
pcb_t* get_pcb(int pid_in) {
   return (pcb_t*)(_8_MB - _8_KB * (pid_in + 1));
}

/* execute - CP3
 * Executes a file.
 * parameters - command : pointer to char array that contains command.
 * returns - none
 * side effects - context switch from Kernel space to user space
 */
int32_t execute (const uint8_t* command) {
    // if all processes are filled, return -1
    int p, found;                                       
    for(p = 0, found = 0; p < PROCESS_COUNT; ++p){
        if(process_status[p] == -1){
            found = 1;
            break;
        }
    }
    if(!found) return -1;

    // clear interrupts
    cli();
    uint8_t buffer[FOUR_BYTE], exec[CMD_MAX_LEN+1],argb[CMD_MAX_LEN+1];
    uint8_t cmd_idx = 0;
    uint8_t arg_idx, cmd_start = 0;
    uint32_t entry_point;
    int i;
    // sanity check
    if (command == NULL){
        return -1;
    }
    // to account for spaces before the executable
    while (command[cmd_start] == ' '){
        cmd_start++;
    }
    cmd_idx = cmd_start;
    // loop through the command until we reach the end of the executable
    while (command[cmd_idx] != ' ' && command[cmd_idx] != '\0' && command[cmd_idx] != '\n') {
        cmd_idx++;
    }
    // if the executable is larger than the limit
    if(cmd_idx > CMD_MAX_LEN){
        return -1;
    }
    // copy the executable from the user typed in command
    for(i = cmd_start; i <cmd_idx; i++) {
        exec[i-cmd_start] = command[i];
    }
    // set end NULL byte
    exec[cmd_idx] = '\0';
    // to account for spaces between the executable and the arguments
    while(command[cmd_idx] == ' '){
        cmd_idx++;
    }
    arg_idx = cmd_idx;
    // determining the length of the argument(s)
    while (command[arg_idx] != '\0' && command[arg_idx] != '\n') {
        arg_idx++;
    }
    // if over the limit then return
    if(arg_idx > MAX_KBUFF_LEN){
        return -1;
    }
    // copying the argument into argument buffer to be stored in pcb later
    for(i = cmd_idx ; i < arg_idx; i++) {
        argb[i-cmd_idx] = command[i];
    }
    argb[arg_idx-cmd_idx] = '\0';
    // checking the magic number to make sure its executable.
    dentry_t search;
    if(read_dentry_by_name((uint8_t*)exec, &search) == 0){
        read_data(search.inode, 0, buffer, FOUR_BYTE);
        // check for magic numbers at beginning of executable as defined by the documentation: 0x7F, 0x45, 0x4C, and 0x46 should be the first
        // four chars of any executable. If not this, then invalid ex
        if(buffer[0] != 0x7f || buffer[1]!= 0x45 || buffer[2]!= 0x4c || buffer[3]!=0x46){
            return -1;
        }
    } else {
        return -1;
    }
    // getting the entry point from 24 - 27
    read_data(search.inode, ENTRY_POINT_START, buffer, FOUR_BYTE);
    entry_point = *((uint32_t*)buffer); //byte manipulation; shell val: 0x080482E8

    if (p < 3) 
        t_run = (t_cur + 2) % 3;
    else {
        t_run = t_visible;
    }

    // update running process in terminal
    t[t_run].cur_pid = p;
    process_status[t[t_run].cur_pid] = 1;

    //set up paging
    map_program(t[t_run].cur_pid);

    // write file data into program image (virtual address)
    inode_t* inode = &(inode_arr[search.inode]);
    read_data(search.inode,0,(uint8_t *)PROG_IMG_ADDR, inode->length); // get length here

    // create pcb for this process
    pcb_t *pcb;
    pcb = get_pcb(t[t_run].cur_pid);
   
    pcb_init(pcb);

    // storing the argument to a buffer in pcb for getargs fn
    strcpy((int8_t*)pcb->arg, (int8_t*)argb);

    asm volatile(
        "movl %%esp, %0   \n\
        movl %%ebp, %1"
        :"=r"(pcb->esp), "=r"(pcb->ebp)
    );

    // update task segment
    tss.esp0 = _8_MB - _8_KB * pcb->pid - FOUR_BYTE;
    tss.ss0 = KERNEL_DS;

    context_switch(entry_point);

    return 0;
}

/* halt - CP3
 * After execute is called, must call halt. Halts the program.
 * parameters - status : return value to send back to parent program.
 * returns - none
 * side effects - context switches and jumps to execute return.
 */
int32_t halt (uint8_t status) {
    int i;
    pcb_t *pcb;

    // get current process block and current process' parent block
    pcb = get_pcb(t[t_run].cur_pid);

    // clear all file descriptors
    for(i = FD_START; i < FD_MAX; ++i)
        close(i);

    // free up process, set running process back to parent
    process_status[t[t_run].cur_pid] = -1;
    t[t_run].cur_pid = pcb->parent_pid;

    // if current process block is base shell, re-execute shell
    if (pcb->pid == pcb->parent_pid){
        execute((uint8_t*)"shell");
        t[t_run].status = 0;
    }

    if (vidmap_page_flag) {
        unmap_video();
        vidmap_page_flag = 0;
    }
    // restore parent paging
    map_program(pcb->parent_pid); // flushes tlb

    // write parent process' info back to TSS(esp0)
    tss.esp0 = pcb->esp0;
    tss.ss0 = KERNEL_DS;

    halt_ret((uint32_t)status, pcb->ebp, pcb->esp);

    return 0; // doesn't reach here
}

/* read - CP3
 * reads data from keyboard, file, RTC, or directory
 * parameter - fd - file descriptor index
 *             buf - buffer which we read into
 *             nbytes - number of bytes to be read
 * return - 0 on success, 1 on failure
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes) {
    // get a pcb to perform read operation
    pcb_t *pcb = get_pcb(t[t_run].cur_pid);

    // error handling - FD in array, buf not empty, nbytes >= 0
    if(fd >= FD_MAX || fd < 0 || buf == NULL || nbytes < 0 || pcb->fd_table[fd].flags == 0) {
        return -1;
    }

    // find fd in fd_table
    return (int32_t)pcb->fd_table[fd].fops_ptr->read(fd, buf, nbytes);
}

/* write - CP3
 * write data to either terminal or RTC
 * parameter - fd - file descriptor index
 *             buf - buffer with data we use (ex. RTC frequency)
 *             nbytes - number of bytes to be read
 * return - 0 on success, 1 on failure
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
    pcb_t *pcb = get_pcb(t[t_run].cur_pid);
    // error handling - FD in array, buf not empty, nbytes >= 0
    if(fd >= FD_MAX || fd < 0 || buf == NULL || nbytes < 0 || pcb->fd_table[fd].flags == 0) {
        return -1;
    }
    // find fd in fd_table

    return (int32_t)pcb->fd_table[fd].fops_ptr->write(fd, buf, nbytes);
}

/* open - CP3
 * provides access to file system - adds file descriptor in PCB, sets up data needed to handle file
 * parameter - filename - name of file in filesystem
 * return - 0 on success, 1 on failure
 */
int32_t open (const uint8_t* filename) {
    pcb_t *pcb = get_pcb(t[t_run].cur_pid);

    // input error handling
    if(filename == NULL || strlen((int8_t*)filename) == 0) {
        return -1;
    }

    // open file with read_dentry_by_name - writes file type into file_block
    dentry_t file_block;
    if(read_dentry_by_name(filename, &file_block) != 0) {
        return -1;
    }
    uint32_t f_type = file_block.file_type;

    // find index to put file descriptor in
    int type_found = 0;
    int32_t fd = FD_START;
    while(fd < FD_MAX) {
        // write if empty fd or if fd not occupied
        // if(pcb->fd_table[fd] == NULL || pcb->fd_table[fd].flags == 0) {
        // NULL is set to 0 so can't be compared to fd_table object, we should use other methods instead
        if(pcb->fd_table[fd].flags == 0) {
            switch(f_type){
                case RTC_FTYPE:
                    pcb->fd_table[fd].fops_ptr = &fops_rtc;
                    pcb->fd_table[fd].inode = NULL;
                    type_found = 1;
                    break;
                case DIR_FTYPE:
                    pcb->fd_table[fd].fops_ptr = &fops_dir;
                    pcb->fd_table[fd].inode = NULL;
                    type_found = 1;
                    break;
                case FILE_FTYPE:
                    pcb->fd_table[fd].fops_ptr = &fops_file;
                    pcb->fd_table[fd].inode = file_block.inode;
                    type_found = 1;
                    break;
                default:
                    return -1;
            }
            if(type_found) {
                pcb->fd_table[fd].file_pos = 0;
                pcb->fd_table[fd].flags = 1; // set to occupied
                // call open for our file type
                pcb->fd_table[fd].fops_ptr->open(filename);
                return fd;
            }
        }
        ++fd;
    }

    return -1;
}

/* close - CP3
 * closes specified fd from fd table
 * parameter - filename - name of file in filesystem
 * return - 0 on success, 1 on failure
 */
int32_t close (int32_t fd) {
    // error handling - fd index not in array
    // user cannot close default descriptors
    if(fd >= FD_MAX || fd < FD_START) {
        return -1;
    }
    pcb_t* pcb = get_pcb(t[t_run].cur_pid);

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

/* getargs - CP3
 * copies arguments passed in from execute in the pcb into a user-level buffer
 * parameter - buf : user level buffer that we copy the data into
 *             nbytes : number of bytes to be copied (sometimes more than the size of the buffer)
 * return - 0 on success, -1 on failure
 */
int32_t getargs (uint8_t* buf, int32_t nbytes) {
    pcb_t* pcb = get_pcb(t[t_run].cur_pid);

    if(buf == NULL || nbytes <= 0 || pcb->arg == '\0' || strlen((int8_t*)pcb->arg) + 1 > nbytes) {
        return -1;
    }

    // copy into our user buffer from pcb arg
    strncpy((int8_t*)buf, (int8_t*)pcb->arg, nbytes);
    buf[strlen((int8_t*)pcb->arg)] = '\0';
    return 0;

}

/* vidmap - CP3
 * maps text mode video memory into user space at virtual address 140MB. Creates page
 * parameter - screen_start : double pointer to 140MB screen start
 * return - 0 on success, -1 on failure
 */
int32_t vidmap (uint8_t** screen_start) {
    if(screen_start == NULL || screen_start < (uint8_t**)_128_MB || screen_start > (uint8_t**)(_132_MB - FOUR_BYTE)){ // account for pointer size
        return -1;
    }

    // create page and set screen start to 140MB pointer
    map_video();
    *screen_start = (uint8_t*) _140_MB;

    // set vidmap_page flag
    vidmap_page_flag = 1;

    return 0;

}

/* set_handler - CP3
 * Not used yet.
 * parameter - signum :
 *             handler_address :
 * return - none
 */
int32_t set_handler (int32_t signum, void* handler_address) {
    return -1;
}

/* sigreturn - CP3
 * Not used yet.
 * parameter - none
 * return - none
 */
int32_t sigreturn (void) {
    return -1;
}
