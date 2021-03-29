#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"
#include "lib.h"

#define BOOT_RESERVE     52
#define MAX_FILE_COUNT   63
#define NAME_SIZE        32
#define DENTRY_RESERVE   24
#define MAX_INODE_BLOCK  1023
#define FOUR_KILOBYTES   4096

// single 64B directory entry within the boot block
typedef struct dentry_t {
    struct {
        unsigned char file_name[NAME_SIZE];
        uint32_t file_type;
        uint32_t inode;
        uint8_t reserved[DENTRY_RESERVE];
    } __attribute__((packed));
} dentry_t;

// boot block - 1st 4KB block in file system
typedef struct bootblk_t {
    struct {
        uint32_t num_of_dirE;
        uint32_t num_of_inodes;
        uint32_t num_of_dblks;
        uint8_t reserved[BOOT_RESERVE];
        dentry_t dir_entries[MAX_FILE_COUNT]; //63 dir.entries
    } __attribute__ ((packed));
} bootblk_t;

// inode block - 4KB block which contains length and data blocks used in order
typedef struct inode_t {
    struct {
        uint32_t length; // length in bytes
        uint32_t dblk[MAX_INODE_BLOCK];
    } __attribute__((packed));
} inode_t;

// data block - 4KB that contains actual data
typedef struct dblk_t {
    uint8_t data[FOUR_KILOBYTES];
} dblk_t;

// global vars
void* filesystem;
bootblk_t* boot;
dentry_t* den_arr;
inode_t* inode_arr;
dblk_t* data_arr;
uint32_t inode_num;
uint32_t file_pos;
uint32_t filesize;
uint32_t dir_offset;

// init and helper functions
void fs_init(void* fs);
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

// read, write, open, close for files and directories
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_open(const uint8_t* filename);
int32_t dir_close(int32_t fd);

#endif

// int32_t FileSys_init(uint32_t start, uint32_t end);

/* bootblk_t* filesystem = NULL;
int32_t FileSys_init(uint32_t start, uint32_t end) {
    bootblk_t* temp_fs = (bootblk_t*) start;
    if(temp_fs->num_of_dirE > MAX_FILE_COUNT) {
        return -1;
    }
    if((bootblk_t*) end != temp_fs + (1 + temp_fs->num_of_inodes) + temp_fs->num_of_dblks) { // +1 for the length 4B
        return -1;
    }
    filesystem = temp_fs;
    return 0;
} */
