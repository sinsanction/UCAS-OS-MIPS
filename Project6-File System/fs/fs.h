#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include "type.h"

// ---------------
// | super block |
// ---------------
// |  inode map  |
// ---------------
// | sector map  |
// ---------------
// |    inode    |
// ---------------
// |    data     |
// ---------------

#define KFS_MAGIC (0x66666666)
#define OFFSET_256M (0X10000000)
#define OFFSET_512M (0x20000000)
#define OFFSET_1G (0x40000000)
#define OFFSET_2G (0x80000000)
#define OFFSET_3G (0xC0000000)

#define FS_SIZE (0x20000000)
//#define FS_SIZE (0x10000000)
#define OFFSET_FS OFFSET_256M
#define OFFSET_DATA (0x11000000)
#define NUM_IMAP_SECTOR 1
#define SECTOR_SIZE 512
#define BLOCK_SIZE 4096
#define NUM_FD 8
#define NUM_MAX_DIR 12

#define ENTRY_SECTOR 8

#define TYPE_FILE 1
#define TYPE_DIR  2
#define TYPE_LINK 3

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3
#define O_OPEN 4

#define FILE_CACHE_BASE 0xffffffffa6000000

typedef struct super_block
{
    uint32_t magic;

    // seg0: offset is sector offset
    uint32_t start_sector;
    uint32_t total_sector;
    uint32_t sector_used;

    uint32_t inode_map_offset;
    uint32_t inode_map_size;
    uint32_t inode_used;

    uint32_t block_map_offset;
    uint32_t block_map_size;
    uint32_t block_used;

    uint32_t inode_offset;
    uint32_t inode_array_size;

    // seg1: offset is block offset
    uint32_t data_start_block;
    uint32_t data_total_block;

    uint32_t inode_entry_size;
    uint32_t dir_entry_size;

} super_block_t;

typedef struct inode_entry 
{
    uint32_t size;
    uint32_t type;
    uint32_t mode;
    uint32_t leaf;
    uint32_t direct_block[8];
    uint32_t indirect_block[4];

} inode_entry_t;

 
typedef struct dir_entry  
{
    uint32_t num_now;
    uint32_t num_max;
    uint32_t ino[NUM_MAX_DIR];
    char name[NUM_MAX_DIR][16];

} dir_entry_t;

typedef struct fd
{
    uint32_t ino;
    uint32_t mode;
    uint32_t wpos;
    uint32_t rpos;
    uint32_t size;
    uint64_t base;

} fd_t;

// extern inode_entry_t current_dir_entry;

int init_fs(void);
int print_fs(void);
void clear_fs(void);
void read_super_block(void);

int readdir(char *name);
int mkdir(char *name);
int rmdir(char *name);
int enterdir(char *name, char *remind_str);

int mknod(char *name);
int open(char *name, uint32_t access);
int write(uint32_t fd, char *buff, uint32_t size);
int read(uint32_t fd, char *buff, uint32_t size);
int close(uint32_t fd);
int cat(char *name);

int hard_link(char *src_name, char *dest_name);
int soft_link(char *src_name, char *dest_name);

#endif