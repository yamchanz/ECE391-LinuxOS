#include "filesys.h"

/* fs_init - CP2
 * Initializes global vars.
 * parameters - fs - mod->mod_start needs to be passed in here from kernel.c
 * returns - none
 */
void fs_init(void* fs) {
    filesystem = fs;
    boot = (bootblk_t*)filesystem;
    inode_arr = &((inode_t*)filesystem)[1]; // accessing first inode (4KB)
    data_arr = &((dblk_t*)filesystem)[1 + boot->num_of_inodes]; // accessing first data block (4KB)
    den_arr = &((dentry_t*)boot)[1]; // accessing first dentry by casting bootblock (64B)
    dir_offset = 0;
}

/* read_dentry_by_name - CP2
 * Fills dentry block with file name, file type, inode number using the
 * given fname if it exists.
 * parameters - fname, dentry
 * returns - 0 (success), -1 (failure)
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    int i;
    int name_len = strlen((int8_t*)fname);
    if(!filesystem) { // if no filesystem
        return -1;
    }
    if(name_len > NAME_SIZE) { // if name size invalid
        return -1;
    }
    if(name_len != NAME_SIZE){
        name_len += 1; // for null termination
    }

    for(i = 0; i < MAX_FILE_COUNT; i++) { // for all files, check for fname
        dentry_t* cur_dir = &(boot->dir_entries[i]);

        if(strncmp((int8_t*)fname, (int8_t*)cur_dir->file_name, name_len) == 0){
            *dentry = *cur_dir; // get block
            return 0;
        }
    }
    return -1; // if not found
}

/* read_dentry_by_index - CP2
 * Fills dentry block with file name, file type, inode number using the
 * index if it is in range.
 * parameters - index, dentry
 * returns - 0 (success), -1 (failure)
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    if(!filesystem) { // if no filesystem
        return -1;
    }
    if(index >= boot->num_of_dirE) { // if index invalid
        return -1;
    }
    *dentry = boot->dir_entries[index]; // get block
    return 0;
}

/* read_data - CP2
 * Function takes in information about which parts of which file to read from
 * to copy those parts into the given buffer.
 * parameters: inode - inode # of file to read
 *             offset - byte offset from where to start reading
 *             buf - buffer to copy to
 *             length - total number of bytes to copy
 * returns : number of bytes copied (success), -1 (failure)
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    if(inode >= boot->num_of_inodes) return -1; // check if inode valid

    inode_t* inode_blk = &(inode_arr[inode]); // instead of: (inode_t*) ((uint32_t)filesystem + inode + 1);
    uint32_t filesize = inode_blk->length;

    // if length is greater than what we have left in the file, set length to rest of file
    if(length + offset > filesize) length = filesize - offset;
    // if offset is greater than file size, nothing read
    if(offset >= filesize) return 0;

    uint32_t cur = offset / _4_KB; // init first data block
    uint32_t start_byte = offset % _4_KB; // init starting byte in first block
    uint32_t bytes_left = length; // init number of bytes to copy

    while(bytes_left > 0) {
        // check for bad block here
        uint32_t idx = inode_blk->dblk[cur]; // find data block to copy from
        dblk_t* data_blk =  &(data_arr[idx]); // instead of: (dblk_t*) ((uint32_t)filesystem + filesystem->num_of_inodes + 1 + idx);
        if(offset) { // for first time
            if((_4_KB - start_byte) >= bytes_left) {
                memcpy(buf, (uint8_t*)data_blk + start_byte, bytes_left);
                buf += bytes_left;
                return length;
            }

            memcpy(buf, (uint8_t*)data_blk + start_byte, _4_KB - start_byte);
            buf += _4_KB - start_byte;
            bytes_left -= _4_KB - start_byte;
            offset = 0;
        }
        else if(bytes_left >= _4_KB) { // for every block in between
            memcpy(buf, (uint8_t*)data_blk, _4_KB);
            buf += _4_KB;
            bytes_left -= _4_KB;
        }
        else{ // for last time
            memcpy(buf, (uint8_t*)data_blk, bytes_left);
            buf += bytes_left;
            return length;
        }
        cur++;
    }
    return 0; // we will not get here
}

/* file_read - CP2
 * Reads nbytes of data into buffer, keeping track of file pos if we do not
 * read entire file in one go.
 * parameters: fd, buf, nbytes
 * returns : 0 (success), -1 (failure)
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes) {
    uint32_t num_read;
    // now that we added pcb, must adjust this function with fd
    pcb_t* pcb = get_pcb(t[t_visible].cur_pid);
    // sanity check
    if(fd >= FD_MAX || fd < FD_START) return -1;
    if(!filesystem) {
        return -1;
    }
    // have to process length in this function,
    // read_data always puts length bytes into buffer (or 0).
    num_read = read_data(pcb->fd_table[fd].inode, pcb->fd_table[fd].file_pos, (uint8_t*)buf, nbytes);
    pcb->fd_table[fd].file_pos += num_read;
    return num_read;
}

/* file_write - CP2
 * Since file system is read-only, this function will always fail.
 * parameters: fd, buf, nbytes
 * returns : -1 (failure)
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/* file_open - CP2
 * Finds the file by name if it exists, then stores the inode number of
 * the file and filesize as globals
 * parameters: filename
 * returns : 0 (success), -1 (failure)
 */
int32_t file_open(const uint8_t* filename) {
    dentry_t entry_info;
    if(!filesystem) {
        return -1;
    }
    if(read_dentry_by_name(filename, &entry_info) != -1) {
        // inode_num = entry_info.inode;
        // file_pos = 0;
        // inode_t* inode_blk = &(inode_arr[inode_num]);
        // filesize = inode_blk->length;
        return 0;
    }
    return -1;
}

/* file_close - CP2
 * Invalidates variables related to the current file and closes it.
 * parameters: fd
 * returns : 0 (success)
 */
int32_t file_close(int32_t fd) {
    // inode_num = 0;
    // file_pos = 0;
    // filesize = 0;
    return 0;
}

/* dir_read - CP2
 * Puts the names of all filenames in the directory entries until the
 * last is reached, at point dir_read returns 0.
 * parameters: fd, buf, nbytes
 * returns : 0 (success), <size read>
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes) {
    int i;

    dentry_t entry_info;
    if(!filesystem) {
        return -1;
    }
    if(read_dentry_by_index(dir_offset, &entry_info) != -1) {
        for(i = 0;i <NAME_SIZE+1; i++){
            ((int8_t*)(buf))[i] = '\0';
        }
        uint32_t name_length = strlen((int8_t*)entry_info.file_name);
        if(name_length > NAME_SIZE) name_length = NAME_SIZE;
        memcpy((char*)buf, (char*)entry_info.file_name, name_length);
        //buf += name_length;
        dir_offset++;
        return name_length;
    }
    dir_offset = 0;
    return 0;
}

/* dir_write - CP2
 * Since file system is read-only, this function will always fail.
 * parameters: fd, buf, nbytes
 * returns : -1 (failure)
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}

/* dir_open - CP2
 * Initializes global var needed for dir_read.
 * parameters: filename
 * returns : 0 (success), -1 (failure)
 */
int32_t dir_open(const uint8_t* filename) {
    if(!filesystem) {
        return -1;
    }
    dir_offset = 0;
    return 0;
}

/* dir_close - CP2
 * Nothing to do here.
 * parameters: fd
 * returns : 0 (success)
 */
int32_t dir_close(int32_t fd) {
    return 0;
}
