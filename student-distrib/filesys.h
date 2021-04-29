#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"
#include "lib.h"
#include "system_calls.h"
#include "terminal.h"

#define BOOT_RESERVE     52
#define MAX_FILE_COUNT   63
#define NAME_SIZE        32
#define DENTRY_RESERVE   24
#define MAX_INODE_BLOCK  1023
#define _1_KB            0x400
#define _4_KB            0x1000
#define _8_KB            0x2000
#define _4_MB            0x400000
#define _8_MB            0x800000
#define _128_MB          0x8000000
#define _132_MB          0x8400000
#define _140_MB          0X8C00000
#define VID_MEM          0xB8000

// single 64B directory entry within the boot block
typedef struct __attribute__((packed)) {
    unsigned char file_name[NAME_SIZE];
    uint32_t file_type;
    uint32_t inode;
    uint8_t reserved[DENTRY_RESERVE];
} dentry_t;

// boot block - 1st 4KB block in file system
typedef struct __attribute__((packed)) {
    uint32_t num_of_dirE;
    uint32_t num_of_inodes;
    uint32_t num_of_dblks;
    uint8_t reserved[BOOT_RESERVE];
    dentry_t dir_entries[MAX_FILE_COUNT]; //63 dir.entries
} bootblk_t;

// inode block - 4KB block which contains length and data blocks used in order
typedef struct __attribute__((packed)) {
    uint32_t length; // length in bytes
    uint32_t dblk[MAX_INODE_BLOCK];
} inode_t;

// data block - 4KB that contains actual data
typedef struct dblk_t {
    uint8_t data[_4_KB];
} dblk_t;

// global vars
void* filesystem;
bootblk_t* boot;
dentry_t* den_arr;
inode_t* inode_arr;
dblk_t* data_arr;
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

