#include "fs.h"
#include "screen.h"
#include "string.h"
#include "common.h"

static uint8_t zero[SECTOR_SIZE];            // always zero
static uint8_t sup[SECTOR_SIZE];             // super block buffer
static uint8_t imap_buf[SECTOR_SIZE];        // imap buffer
static uint8_t bmap_buf[16][SECTOR_SIZE];    // block map buffer
static uint8_t inode_buf[16][SECTOR_SIZE];   // inode buffer
static uint8_t buf[SECTOR_SIZE];             // sector buffer
static uint8_t bbuf[BLOCK_SIZE];             // block buffer
static uint8_t name_buf[BLOCK_SIZE];         // name buffer
static super_block_t *super_block = NULL;
static inode_entry_t *inode_array[128];

static fd_t fds[NUM_FD];
static int fds_free[NUM_FD];
static inode_entry_t current_inode_entry;
static inode_entry_t *new_inode_entry;
static dir_entry_t current_dir_entry;
static dir_entry_t root_dir_entry;
static dir_entry_t *new_dir_entry;
char path_buf[128];   //cd path buffer
int path_ptr = 0;
int fs_setup = 0;
int path_begin[6];
int path_end[6];
int path_num;
int path_abs;


static void write_a_sector(char *buff, uint32_t sector_offset)
{
    uint32_t base = OFFSET_FS + sector_offset * SECTOR_SIZE;
    sdwrite(buff, base, SECTOR_SIZE);
}

static void read_a_sector(char *buff, uint32_t sector_offset)
{
    uint32_t base = OFFSET_FS + sector_offset * SECTOR_SIZE;
    sdread(buff, base, SECTOR_SIZE);
}

static void write_a_block(char *buff, uint32_t block_offset)
{
    uint32_t base = OFFSET_DATA + block_offset * BLOCK_SIZE;
    sdwrite(buff, base, BLOCK_SIZE);
}

static void read_a_block(char *buff, uint32_t block_offset)
{
    uint32_t base = OFFSET_DATA + block_offset * BLOCK_SIZE;
    sdread(buff, base, BLOCK_SIZE);
}

static uint32_t find_free_block(void)
{
    int i, j, k, block_id;
    uint8_t mask;
    for(i=0; i<16; i++){
        for(j=0; j<SECTOR_SIZE; j++){
            if(bmap_buf[i][j] != 0xff){
                mask = 1;
                for(k=0; k<8; k++){
                    if((bmap_buf[i][j] & mask) == 0){
                        bmap_buf[i][j] = bmap_buf[i][j] | mask;
                        block_id = i * 8 * SECTOR_SIZE + j * 8 + k;
                        super_block->block_used++;
                        write_a_sector((char *)bmap_buf[i], super_block->block_map_offset+i);
                        return block_id;
                    }
                    else{
                        mask = mask << 1;
                    }
                }
            }
        }
    }
    return -1;
}

static uint32_t find_free_inode(void)
{
    int i, j, ino;
    uint8_t mask;
    for(i=0; i<SECTOR_SIZE; i++){
        if(imap_buf[i] != 0xff){
            mask = 1;
            for(j=0; j<8; j++){
                if((imap_buf[i] & mask) == 0){
                    imap_buf[i] = imap_buf[i] | mask;
                    ino = i * 8 + j;
                    super_block->inode_used++;
                    memset(inode_array[ino], 0, sizeof(inode_entry_t));
                    write_a_sector((char *)imap_buf, 1);
                    return ino;
                }
                else{
                    mask = mask << 1;
                }
            }
        }
    }
    return -1;
}

static uint32_t find_free_fd(void)
{
    int i;
    int free_fd = -1;
    for(i=0; i<NUM_FD; i++){
        if(fds_free[i] == 0){
            free_fd = i;
            fds_free[i] = 1;
            fds[i].base = FILE_CACHE_BASE + i * 8 * 1024 * 1024;
            break;
        }
    }
    return free_fd;
}

static void release_a_block(int block_id)
{
    int i, j, k, l;
    uint8_t mask;

    i = block_id / (8 * SECTOR_SIZE);
    block_id = block_id % (8 * SECTOR_SIZE);
    j = block_id / 8;
    k = block_id % 8;

    mask = 1;
    for(l=0; l<k; l++){
        mask = mask << 1;
    }

    if((bmap_buf[i][j] & mask) == 0){
        printk("release a free block: %d\n\r", block_id);
    }
    else{
        mask = ~mask;
        bmap_buf[i][j] = bmap_buf[i][j] & mask;
        super_block->block_used--;
        write_a_sector((char *)bmap_buf[i], super_block->block_map_offset+i);
    }
}

static void release_a_inode(int ino)
{
    int i, j, k;
    uint8_t mask;

    i = ino / 8;
    j = ino % 8;

    mask = 1;
    for(k=0; k<j; k++){
        mask = mask << 1;
    }

    if((imap_buf[i] & mask) == 0){
        printk("release a free inode: %d\n\r", ino);
    }
    else{
        mask = ~mask;
        imap_buf[i] = imap_buf[i] & mask;
        super_block->inode_used--;
        memset(inode_array[ino], 0, sizeof(inode_entry_t));
        write_a_sector((char *)imap_buf, 1);
    }
}

static void release_a_fd(int fd_id)
{
    fds_free[fd_id] = 0;
    fds[fd_id].base = 0;
}

static void rewrite_remind_str(char *remind_str)
{
    if(path_ptr == 1){
        remind_str[0] = '$';
        remind_str[1] = ' ';
        remind_str[2] = '\0';
    }
    else{
        strcpy(remind_str, path_buf);
        remind_str[path_ptr] = '$';
        remind_str[path_ptr + 1] = ' ';
        remind_str[path_ptr + 2] = '\0';
    }
}

static int analysis_path(void)
{
    int i, j;
    int length, begin, flag = 0;

    length = strlen((char*)name_buf);
    if(name_buf[0] == '/'){
        path_abs = 1;
        begin = 1;
    }
    else{
        path_abs = 0;
        begin = 0;
    }

    path_num = 0;
    for(i=begin; i<length; i++){
        if(name_buf[i] != '/'){
            if(name_buf[i] == '\0'){
                break;
            }
            else{
                path_begin[path_num] = i;
                for(j=i+1; j<=length; j++){
                    if(name_buf[j] == '\0'){
                        path_end[path_num] = j;
                        path_num++;
                        i = length;
                        if(path_num >= 6){
                            flag = 2;  //path too long
                            goto OUT;
                        }
                        break;
                    }
                    if(name_buf[j] == '/'){
                        path_end[path_num] = j;
                        path_num++;
                        i = j;
                        if(path_num >= 6){
                            flag = 2;  //path too long
                            goto OUT;
                        }
                        break;
                    }
                }
            }
        }
        else{
            flag = 1;  //two consecutive slashes
        }
    }
    OUT:
    if(flag != 0)
    return flag;

    for(i=0; i<path_num; i++){
        name_buf[path_end[i]] = '\0';
    }
    return 0;
}

int readdir(char *name)
{
    int cursor_x = 1;
    int cursor_y = SHELL_BEGIN + 1;
    int i, j, flag;
    dir_entry_t *now_dir_entry;
    int now_ino;

    screen_clear(SHELL_BEGIN + 1, SCREEN_HEIGHT - 1);
    screen_move_cursor(1, SHELL_BEGIN + 1);
    cursor_y++;

    if(fs_setup == 0){
        kprintf("ls: fail, filesystem has not set up.");
        return cursor_y;
    }

    if(name == NULL){
        kprintf("[FS] ls current dir");
        now_dir_entry = &current_dir_entry;
        goto PRIT;
    }
    
    kprintf("[FS] ls %s", name);
    strcpy((char *)name_buf, name);
    int ret = analysis_path();
    if(ret != 0){
        screen_move_cursor(cursor_x, cursor_y);
        cursor_y++;
        if(ret == 1){
            kprintf("ls: fail, the path has two consecutive slashes.");
        }
        else if(ret == 2){
            kprintf("ls: fail, the path is too long.");
        }
        else{
            kprintf("ls: fail, the path does not exist.(0)");
        }
        return cursor_y;
    }

    if(path_abs)
        now_dir_entry = &root_dir_entry;
    else
        now_dir_entry = &current_dir_entry;
    vt100_move_cursor(1, 2);
    printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num; i++){
        printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            screen_move_cursor(cursor_x, cursor_y);
            cursor_y++;
            kprintf("ls: fail, the path does not exist.(1)");
            return cursor_y;
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                if(i < path_num-1){
                    screen_move_cursor(cursor_x, cursor_y);
                    cursor_y++;
                    kprintf("ls: fail, the path does not exist.(2)");
                    return cursor_y;
                }
                else{
                    screen_move_cursor(cursor_x, cursor_y);
                    cursor_y++;
                    kprintf(" name: %s  ino: %d  type: %d  size: %d B", now_dir_entry->name[j], now_dir_entry->ino[j], inode_array[now_dir_entry->ino[j]]->type, inode_array[now_dir_entry->ino[j]]->size);
                    return cursor_y;
                }
            }
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }
    }

    PRIT:
    for(i=0; i<now_dir_entry->num_now; i++){
        screen_move_cursor(cursor_x, cursor_y);
        cursor_y++;
        kprintf(" [%d] name: %s  ino: %d  type: %d  size: %d B", i, now_dir_entry->name[i], now_dir_entry->ino[i], inode_array[now_dir_entry->ino[i]]->type, inode_array[now_dir_entry->ino[i]]->size);
    }
    return cursor_y;
}

int mkdir(char *name)
{
    int i, j, flag;
    dir_entry_t *now_dir_entry;
    int now_ino;

    if(fs_setup == 0){
        return -1;
    }

    strcpy((char *)name_buf, name);
    int ret = analysis_path();
    if(ret != 0){
        return ret;
    }
    if(path_num == 0){
        return 3;  //invalid path
    }

    if(path_abs)
        now_dir_entry = &root_dir_entry;
    else
        now_dir_entry = &current_dir_entry;
    vt100_move_cursor(1, 2);
    printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num-1; i++){
        printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            return 4;  //path does not exist
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                return 5;  //path does not exist
            }
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }
    }
    if(path_num == 1){
        if(path_abs)
            now_ino = 0;
        else
            now_ino = current_dir_entry.ino[0];
        read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
        printk("read block: %d\n\r", inode_array[now_ino]->direct_block[0]);
        now_dir_entry = (dir_entry_t *)bbuf;
    }

    if(now_dir_entry->num_now == now_dir_entry->num_max){
        return 6;  //dir too many
    }
    for(j=0; j<now_dir_entry->num_now; j++){
        if(strcmp(&name_buf[path_begin[path_num-1]], now_dir_entry->name[j]) == 0){
            return 7;  //already have same dir
        }
    }

    int new_ino = find_free_inode();
    int new_block = find_free_block();
    now_dir_entry->ino[now_dir_entry->num_now] = new_ino;
    strcpy(now_dir_entry->name[now_dir_entry->num_now], &name_buf[path_begin[path_num-1]]);
    printk("path[%d]: ino: %d, block: %d, name: %s\n\r", i, new_ino, new_block, now_dir_entry->name[now_dir_entry->num_now]);
    now_dir_entry->num_now++;
    write_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
    if(inode_array[now_ino]->leaf == TRUE){
        inode_array[now_ino]->leaf = FALSE;
        write_a_sector((char *)inode_buf[now_ino/8], super_block->inode_offset + now_ino/8);
    }
    if(now_ino == 0){
        root_dir_entry = *now_dir_entry;
    }
    if(now_ino == current_dir_entry.ino[0]){
        current_dir_entry = *now_dir_entry;
    }

    inode_array[new_ino]->size = sizeof(dir_entry_t);
    inode_array[new_ino]->type = TYPE_DIR;
    inode_array[new_ino]->leaf = TRUE;
    inode_array[new_ino]->direct_block[0] = new_block;
    write_a_sector((char *)inode_buf[new_ino/8], super_block->inode_offset + new_ino/8);
    printk("write inode array %d\n\r", new_ino/8);

    memset(bbuf, 0, sizeof(bbuf));
    new_dir_entry = (dir_entry_t*)bbuf;
    new_dir_entry->num_now = 2;
    new_dir_entry->num_max = 12;
    new_dir_entry->ino[0] = new_ino;
    new_dir_entry->ino[1] = now_ino;
    new_dir_entry->name[0][0] = '.';
    new_dir_entry->name[0][1] = '\0';
    new_dir_entry->name[1][0] = '.';
    new_dir_entry->name[1][1] = '.';
    new_dir_entry->name[1][2] = '\0';
    write_a_block((char *)bbuf, new_block);
    write_a_sector((char *)sup, 0);
    write_a_sector((char *)zero, 10000);
    return 0;
}

int rmdir(char *name)
{
    int i, j, flag;
    dir_entry_t *now_dir_entry;
    int now_ino;

    if(fs_setup == 0){
        return -1;
    }

    strcpy((char *)name_buf, name);
    int ret = analysis_path();
    if(ret != 0){
        return ret;
    }
    if(path_num == 0){
        return 3;  //invalid path
    }

    if(path_abs)
        now_dir_entry = &root_dir_entry;
    else
        now_dir_entry = &current_dir_entry;
    vt100_move_cursor(1, 2);
    printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num-1; i++){
        printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            return 4;  //path does not exist
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                return 5;  //path does not exist
            }
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }
    }
    if(path_num == 1){
        if(path_abs)
            now_ino = 0;
        else
            now_ino = current_dir_entry.ino[0];
        read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
        printk("read block: %d\n\r", inode_array[now_ino]->direct_block[0]);
        now_dir_entry = (dir_entry_t *)bbuf;
    }
    
    int rm_id = -1;
    int rm_ino;
    int rm_block;
    for(j=0; j<now_dir_entry->num_now; j++){
        if(strcmp(&name_buf[path_begin[path_num-1]], now_dir_entry->name[j]) == 0){
            rm_id = j;
            break;
        }
    }
    if(rm_id == -1){
        return 6;   //last level dir not find
    }
    else if(rm_id == 0 || rm_id == 1){
        return 7;   //rm . or .. dir
    }

    rm_ino = now_dir_entry->ino[rm_id];
    rm_block = inode_array[rm_ino]->direct_block[0];
    if(inode_array[rm_ino]->type != TYPE_DIR){
        return 8;   //rm not a dir
    }
    if(inode_array[rm_ino]->leaf != TRUE){
        return 9;   //rm not a leaf dir
    }
    printk("path[%d]: ino: %d, block: %d, name: %s\n\r", i, rm_ino, rm_block, now_dir_entry->name[rm_id]);
    printk("num_now: %d\n\r", now_dir_entry->num_now);
    for(j=rm_id; j<now_dir_entry->num_now-1; j++){
        now_dir_entry->ino[j] = now_dir_entry->ino[j+1];
        strcpy(now_dir_entry->name[j], now_dir_entry->name[j+1]);
    }
    now_dir_entry->num_now--;
    write_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);

    if(now_dir_entry->num_now == 2){
        inode_array[now_ino]->leaf = TRUE;
        write_a_sector((char *)inode_buf[now_ino/8], super_block->inode_offset + now_ino/8);
    }
    if(now_ino == 0){
        root_dir_entry = *now_dir_entry;
    }
    if(now_ino == current_dir_entry.ino[0]){
        current_dir_entry = *now_dir_entry;
    }

    release_a_block(rm_block);
    release_a_inode(rm_ino);
    write_a_sector((char *)inode_buf[rm_ino/8], super_block->inode_offset + rm_ino/8);
    printk("write inode array %d\n\r", rm_ino/8);

    write_a_sector((char *)sup, 0);
    write_a_sector((char *)zero, 10000);
    printk("write sp\n\r");
    return 0;
}

int enterdir(char *name, char *remind_str)
{
    int i, j, flag;
    dir_entry_t *now_dir_entry;
    int now_ino;

    if(fs_setup == 0){
        return -1;
    }

    strcpy((char *)name_buf, name);
    int ret = analysis_path();
    if(ret != 0){
        return ret;
    }
    if(path_num == 0){
        path_buf[0] = '/';
        path_buf[1] = '\0';
        path_ptr = 1;
        current_dir_entry = root_dir_entry;
        rewrite_remind_str(remind_str);
        return 0;
    }

    char cd_path_buf[128];   //cd path buffer
    int cd_ptr;
    dir_entry_t cd_dir_entry;

    if(path_abs){
        now_dir_entry = &root_dir_entry;
        cd_dir_entry = root_dir_entry;
        cd_path_buf[0] = '/';
        cd_path_buf[1] = '\0';
        cd_ptr = 1;
    }
    else{
        now_dir_entry = &current_dir_entry;
        cd_dir_entry = current_dir_entry;
        strcpy(cd_path_buf, path_buf);
        cd_ptr = path_ptr;
    }
    vt100_move_cursor(1, 2);
    printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num; i++){
        printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            return 3;  //path does not exist
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                return 4;  //path does not exist
            }

            if(strcmp(now_dir_entry->name[j], ".") == 0){
                ;
            }
            else if(strcmp(now_dir_entry->name[j], "..") == 0){
                if(cd_ptr == 1){
                    ;
                }
                else{
                    int k;
                    for(k=cd_ptr-2; k>=0; k--){
                        if(cd_path_buf[k] == '/'){
                            cd_ptr = k+1;
                            cd_path_buf[cd_ptr] = '\0';
                            break;
                        }
                    }
                }
            }
            else{
                strcpy(&cd_path_buf[cd_ptr], now_dir_entry->name[j]);
                cd_ptr += strlen(now_dir_entry->name[j]);
                cd_path_buf[cd_ptr++] = '/';
                cd_path_buf[cd_ptr] = '\0';
            }

            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
            cd_dir_entry = *now_dir_entry;
        }
    }

    current_dir_entry = cd_dir_entry;
    strcpy(path_buf, cd_path_buf);
    path_ptr = cd_ptr;
    rewrite_remind_str(remind_str);
    return 0;
}

void read_super_block(void)
{
    int i, j;

    read_a_sector((char *)buf, 0);
    uint32_t *magic = (uint32_t*)buf;
    if(*magic != KFS_MAGIC){
        printk("> [INIT] Filesystem has not set up.\n\r");
        return;
    }

    printk("> [INIT] Filesystem has already set up, reading...\n\r");
    read_a_sector((char*)sup, 0);
    super_block = (super_block_t*)sup;
    printk(" [FS] magic: 0x%x\n\r", super_block->magic);
    printk(" [FS] seg0: start sector: %d (0x%x), used sector: %d/%d\n\r", super_block->start_sector, OFFSET_FS, super_block->sector_used, super_block->total_sector);
    printk(" [FS]       inode map offset: %d, size: %d sector, used: %d/%d\n\r", super_block->inode_map_offset, super_block->inode_map_size, super_block->inode_used, super_block->inode_map_size*512*8);
    printk(" [FS]       block map offset: %d, size: %d sector\n\r", super_block->block_map_offset, super_block->block_map_size);
    printk(" [FS]       inode offset: %d, size: %d sector\n\r", super_block->inode_offset, super_block->inode_array_size);
    printk(" [FS] seg1: start block: %d (0x%x), used block: %d/%d\n\r", super_block->data_start_block, OFFSET_DATA, super_block->block_used, super_block->data_total_block);
    printk(" [FS]       data offset: %d, size: %d block\n\r", 0, super_block->data_total_block);
    printk(" [FS] inode entry size: %dB, dir entry size: %dB\n\r", super_block->inode_entry_size, super_block->dir_entry_size);

    memset((char*)zero, 0, sizeof(zero));
    fs_setup = 1;

    read_a_sector((char*)imap_buf, 1);
    for(i=0; i<16; i++){
        read_a_sector((char*)bmap_buf[i], super_block->block_map_offset+i);
    }
    for(i=0; i<16; i++){
        read_a_sector((char*)inode_buf[i], super_block->inode_offset+i);
        for(j=0; j<8; j++){
            inode_array[i * 8 + j] = (inode_entry_t*)(&inode_buf[i][j * 64]);
        }
    }
    current_inode_entry = *(inode_array[0]);

    read_a_block((char*)bbuf, 0);
    new_dir_entry = (dir_entry_t *)bbuf;
    root_dir_entry = current_dir_entry = *new_dir_entry;
    path_buf[0] = '/';
    path_buf[1] = '\0';
    path_ptr = 1;
    for(i=0; i<NUM_FD; i++){
        fds_free[i] = 0;
    }
    printk("> [INIT] Filesystem initialization succeeded.\n\r");
}

int mknod(char *name)
{
    int i, j, flag;
    dir_entry_t *now_dir_entry;
    int now_ino;

    if(fs_setup == 0){
        return -1;
    }

    strcpy((char *)name_buf, name);
    int ret = analysis_path();
    if(ret != 0){
        return ret;
    }
    if(path_num == 0){
        return 3;  //invalid path
    }

    if(path_abs)
        now_dir_entry = &root_dir_entry;
    else
        now_dir_entry = &current_dir_entry;
    vt100_move_cursor(1, 2);
    printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num-1; i++){
        printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            return 4;  //path does not exist
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                return 5;  //path does not exist
            }
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }
    }
    if(path_num == 1){
        if(path_abs)
            now_ino = 0;
        else
            now_ino = current_dir_entry.ino[0];
        read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
        printk("read block: %d\n\r", inode_array[now_ino]->direct_block[0]);
        now_dir_entry = (dir_entry_t *)bbuf;
    }

    if(now_dir_entry->num_now == now_dir_entry->num_max){
        return 6;  //dir too many
    }
    for(j=0; j<now_dir_entry->num_now; j++){
        if(strcmp(&name_buf[path_begin[path_num-1]], now_dir_entry->name[j]) == 0){
            return 7;  //already have same file
        }
    }

    int new_ino = find_free_inode();
    int new_block = find_free_block();
    now_dir_entry->ino[now_dir_entry->num_now] = new_ino;
    strcpy(now_dir_entry->name[now_dir_entry->num_now], &name_buf[path_begin[path_num-1]]);
    printk("path[%d]: ino: %d, block: %d, name: %s\n\r", i, new_ino, new_block, now_dir_entry->name[now_dir_entry->num_now]);
    now_dir_entry->num_now++;
    write_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
    if(inode_array[now_ino]->leaf == TRUE){
        inode_array[now_ino]->leaf = FALSE;
        write_a_sector((char *)inode_buf[now_ino/8], super_block->inode_offset + now_ino/8);
    }
    if(now_ino == 0){
        root_dir_entry = *now_dir_entry;
    }
    if(now_ino == current_dir_entry.ino[0]){
        current_dir_entry = *now_dir_entry;
    }

    inode_array[new_ino]->size = 0;
    inode_array[new_ino]->type = TYPE_FILE;
    inode_array[new_ino]->mode = O_RDWR;
    inode_array[new_ino]->leaf = TRUE;
    inode_array[new_ino]->direct_block[0] = new_block;
    write_a_sector((char *)inode_buf[new_ino/8], super_block->inode_offset + new_ino/8);
    printk("write inode array %d\n\r", new_ino/8);

    memset(bbuf, 0, sizeof(bbuf));
    write_a_block((char *)bbuf, new_block);
    write_a_sector((char *)sup, 0);
    write_a_sector((char *)zero, 10000);
    return 0;
}

int open(char *name, uint32_t access)
{
    int i, j, flag;
    dir_entry_t *now_dir_entry;
    int now_ino;

    if(fs_setup == 0){
        return -1;
    }

    strcpy((char *)name_buf, name);
    int ret = analysis_path();
    if(ret != 0){
        return -1;
    }
    if(path_num == 0){
        return -1;  //invalid path
    }

    if(path_abs)
        now_dir_entry = &root_dir_entry;
    else
        now_dir_entry = &current_dir_entry;
    vt100_move_cursor(1, 2);
    printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num-1; i++){
        printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            return -1;  //path does not exist
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                return -1;  //path does not exist
            }
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }
    }
    if(path_num == 1){
        if(path_abs)
            now_ino = 0;
        else
            now_ino = current_dir_entry.ino[0];
        read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
        printk("read block: %d\n\r", inode_array[now_ino]->direct_block[0]);
        now_dir_entry = (dir_entry_t *)bbuf;
    }

    int file_id = -1;
    int file_ino;
    int *file_block;
    int file_fd;
    uint64_t file_offset;
    for(j=0; j<now_dir_entry->num_now; j++){
        if(strcmp(&name_buf[path_begin[path_num-1]], now_dir_entry->name[j]) == 0){
            file_id = j;
            break;
        }
    }
    if(file_id == -1){
        return -1;   //last level file not find
    }
    else if(file_id == 0 || file_id == 1){
        return -1;   //open . or .. dir
    }

    file_ino = now_dir_entry->ino[file_id];
    if(inode_array[file_ino]->type != TYPE_FILE){
        return -1;   //open not a file
    }
    if((inode_array[file_ino]->mode & access) == 0){
        return -1;   //file can not access
    }
    file_fd = find_free_fd();
    if(file_fd == -1){
        return -1;   //fd not enough
    }
    fds[file_fd].ino = file_ino;
    fds[file_fd].mode = inode_array[file_ino]->mode;
    fds[file_fd].wpos = 0;
    fds[file_fd].rpos = 0;
    fds[file_fd].size = inode_array[file_ino]->size;
    inode_array[file_ino]->mode = O_OPEN;
    write_a_sector((char *)inode_buf[file_ino/8], super_block->inode_offset + file_ino/8);

    printk("path[%d]: ino: %d, fd: %d, name: %s\n\r", i, file_ino, file_fd, now_dir_entry->name[file_id]);
    file_block = inode_array[file_ino]->direct_block;
    file_offset = fds[file_fd].base;
    if(inode_array[file_ino]->size <= 8 * BLOCK_SIZE){
        int bk_num = (inode_array[file_ino]->size) / BLOCK_SIZE + ((inode_array[file_ino]->size % BLOCK_SIZE) != 0);
        if(inode_array[file_ino]->size == 0)
            bk_num = 1;
        for(i=0; i<bk_num; i++){
            read_a_block((char *)file_offset, *file_block);
            release_a_block(*file_block);
            file_block ++;
            file_offset += BLOCK_SIZE;
        }
    }
    else{
        for(i=0; i<8; i++){
            read_a_block((char *)file_offset, *file_block);
            release_a_block(*file_block);
            file_block ++;
            file_offset += BLOCK_SIZE;
        }
        read_a_block((char *)bbuf, inode_array[file_ino]->indirect_block[0]);
        release_a_block(inode_array[file_ino]->indirect_block[0]);
        file_block = (int *)bbuf;
        int block_num;
        if(inode_array[file_ino]->size <= (8 + 1024) * BLOCK_SIZE)
            block_num = (inode_array[file_ino]->size) / BLOCK_SIZE + ((inode_array[file_ino]->size % BLOCK_SIZE) != 0) - 8;
        else
            block_num = 1024;
        
        for(i=0; i<block_num; i++){
            read_a_block((char *)file_offset, *file_block);
            release_a_block(*file_block);
            file_block ++;
            file_offset += BLOCK_SIZE;
        }

        if(inode_array[file_ino]->size > (8 + 1024) * BLOCK_SIZE){
            read_a_block((char *)bbuf, inode_array[file_ino]->indirect_block[1]);
            release_a_block(inode_array[file_ino]->indirect_block[1]);
            file_block = (int *)bbuf;
            block_num = (inode_array[file_ino]->size) / BLOCK_SIZE + ((inode_array[file_ino]->size % BLOCK_SIZE) != 0) - 8 - 1024;
            for(i=0; i<block_num; i++){
                read_a_block((char *)file_offset, *file_block);
                release_a_block(*file_block);
                file_block ++;
                file_offset += BLOCK_SIZE;
            }
        }
    }
    write_a_sector((char *)sup, 0);
    write_a_sector((char *)zero, 10000);
    return file_fd;
}

int write(uint32_t fd, char *buff, uint32_t size)
{
    uint64_t base;

    if(fd < 0 || fd > NUM_FD){
        return -1;
    }
    if(8 * 1024 * 1024 < fds[fd].wpos + size){
        return -1;
    }

    base = fds[fd].base + fds[fd].wpos;
    memcpy((char *)base, buff, size);
    if(fds[fd].wpos + size > fds[fd].size)
        fds[fd].size = (fds[fd].wpos + size);
    fds[fd].wpos += size;
    return 0;
}

int read(uint32_t fd, char *buff, uint32_t size)
{
    uint64_t base;

    if(fd < 0 || fd > NUM_FD){
        return -1;
    }
    if(fds[fd].size < fds[fd].rpos + size){
        return -1;
    }

    base = fds[fd].base + fds[fd].rpos;
    memcpy(buff, (char *)base, size);
    fds[fd].rpos += size;
    return 0;
}

int close(uint32_t fd)
{
    if(fd < 0 || fd > NUM_FD){
        return -1;
    }

    int file_ino;
    int *file_block;
    uint64_t file_offset;
    int i;

    file_ino = fds[fd].ino;
    inode_array[file_ino]->mode = fds[fd].mode;
    inode_array[file_ino]->size = fds[fd].size;

    file_block = inode_array[file_ino]->direct_block;
    file_offset = fds[fd].base;
    if(inode_array[file_ino]->size <= 8 * BLOCK_SIZE){
        int bk_num = (inode_array[file_ino]->size) / BLOCK_SIZE + ((inode_array[file_ino]->size % BLOCK_SIZE) != 0);
        if(inode_array[file_ino]->size == 0)
            bk_num = 1;
        for(i=0; i<bk_num; i++){
            *file_block = find_free_block();
            write_a_block((char *)file_offset, *file_block);
            file_block ++;
            file_offset += BLOCK_SIZE;
        }
    }
    else{
        for(i=0; i<8; i++){
            *file_block = find_free_block();
            write_a_block((char *)file_offset, *file_block);
            file_block ++;
            file_offset += BLOCK_SIZE;
        }

        inode_array[file_ino]->indirect_block[0] = find_free_block();
        memset(bbuf, 0, sizeof(bbuf));
        file_block = (int *)bbuf;
        int block_num;
        if(inode_array[file_ino]->size <= (8 + 1024) * BLOCK_SIZE)
            block_num = (inode_array[file_ino]->size) / BLOCK_SIZE + ((inode_array[file_ino]->size % BLOCK_SIZE) != 0) - 8;
        else
            block_num = 1024;
        
        for(i=0; i<block_num; i++){
            *file_block = find_free_block();
            write_a_block((char *)file_offset, *file_block);
            file_block ++;
            file_offset += BLOCK_SIZE;
        }
        write_a_block((char *)bbuf, inode_array[file_ino]->indirect_block[0]);

        if(inode_array[file_ino]->size > (8 + 1024) * BLOCK_SIZE){
            inode_array[file_ino]->indirect_block[1] = find_free_block();
            memset(bbuf, 0, sizeof(bbuf));
            file_block = (int *)bbuf;
            block_num = (inode_array[file_ino]->size) / BLOCK_SIZE + ((inode_array[file_ino]->size % BLOCK_SIZE) != 0) - 8 - 1024;
            for(i=0; i<block_num; i++){
                *file_block = find_free_block();
                write_a_block((char *)file_offset, *file_block);
                file_block ++;
                file_offset += BLOCK_SIZE;
            }
            write_a_block((char *)bbuf, inode_array[file_ino]->indirect_block[1]);
        }
    }

    write_a_sector((char *)inode_buf[file_ino/8], super_block->inode_offset + file_ino/8);
    write_a_sector((char *)sup, 0);
    write_a_sector((char *)zero, 10000);
    release_a_fd(fd);
    return 0;
}

int cat(char *name)
{
    int i, j, flag;
    dir_entry_t *now_dir_entry;
    int now_ino;

    if(fs_setup == 0){
        return -1;
    }

    strcpy((char *)name_buf, name);
    int ret = analysis_path();
    if(ret != 0){
        return ret;
    }
    if(path_num == 0){
        return 3;  //invalid path
    }

    if(path_abs)
        now_dir_entry = &root_dir_entry;
    else
        now_dir_entry = &current_dir_entry;
    //vt100_move_cursor(1, 2);
    //printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num-1; i++){
        //printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            return 4;  //path does not exist
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                return 5;  //path does not exist
            }
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }
    }
    if(path_num == 1){
        if(path_abs)
            now_ino = 0;
        else
            now_ino = current_dir_entry.ino[0];
        read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
        //printk("read block: %d\n\r", inode_array[now_ino]->direct_block[0]);
        now_dir_entry = (dir_entry_t *)bbuf;
    }

    int file_id = -1;
    int file_ino;
    int *file_block;
    int file_fd;
    uint64_t file_offset;
    for(j=0; j<now_dir_entry->num_now; j++){
        if(strcmp(&name_buf[path_begin[path_num-1]], now_dir_entry->name[j]) == 0){
            file_id = j;
            break;
        }
    }
    if(file_id == -1){
        return 6;   //last level file not find
    }
    else if(file_id == 0 || file_id == 1){
        return 7;   //cat . or .. dir
    }

    file_ino = now_dir_entry->ino[file_id];
    if(!(inode_array[file_ino]->type == TYPE_FILE || inode_array[file_ino]->type == TYPE_LINK)){
        return 8;   //cat not a file or link
    }

    file_fd = find_free_fd();
    if(file_fd == -1){
        return 9;   //fd not enough
    }

    if(inode_array[file_ino]->type == TYPE_LINK){
        read_a_block((char *)bbuf, inode_array[file_ino]->direct_block[0]);
        strcpy((char *)name_buf, (char *)bbuf);
        ret = analysis_path();
        if(ret != 0){
            return ret + 10;
        }
        if(path_num == 0){
            return 13;  //invalid path
        }

        if(path_abs)
            now_dir_entry = &root_dir_entry;
        else
            now_dir_entry = &current_dir_entry;
        //printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
        for(i=0; i<path_num-1; i++){
            //printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
            flag = 0;
            for(j=0; j<now_dir_entry->num_now; j++){
                if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                    flag = 1;
                    break;
                }
            }
            if(flag == 0){
                return 14;  //path does not exist
            }
            else{
                now_ino = now_dir_entry->ino[j];
                if(inode_array[now_ino]->type != TYPE_DIR){
                    return 15;  //path does not exist
                }
                read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
                now_dir_entry = (dir_entry_t *)bbuf;
            }
        }
        if(path_num == 1){
            if(path_abs)
                now_ino = 0;
            else
                now_ino = current_dir_entry.ino[0];
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            //printk("read block: %d\n\r", inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }

        file_id = -1;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[path_num-1]], now_dir_entry->name[j]) == 0){
                file_id = j;
                break;
            }
        }
        if(file_id == -1){
            return 16;   //last level file not find
        }
        else if(file_id == 0 || file_id == 1){
            return 17;   //src . or .. dir
        }

        file_ino = now_dir_entry->ino[file_id];
        if(inode_array[file_ino]->type != TYPE_FILE){
            return 18;   //src not a file
        }
    }

    fds[file_fd].ino = file_ino;

    //printk("path[%d]: ino: %d, fd: %d, name: %s\n\r", i, file_ino, file_fd, now_dir_entry->name[file_id]);
    file_block = inode_array[file_ino]->direct_block;
    file_offset = fds[file_fd].base;
    if(inode_array[file_ino]->size <= 8 * BLOCK_SIZE){
        int bk_num = (inode_array[file_ino]->size) / BLOCK_SIZE + ((inode_array[file_ino]->size % BLOCK_SIZE) != 0);
        if(inode_array[file_ino]->size == 0)
            bk_num = 1;
        for(i=0; i<bk_num; i++){
            read_a_block((char *)file_offset, *file_block);
            file_block ++;
            file_offset += BLOCK_SIZE;
        }
    }
    else{
        for(i=0; i<8; i++){
            read_a_block((char *)file_offset, *file_block);
            file_block ++;
            file_offset += BLOCK_SIZE;
        }
        read_a_block((char *)bbuf, inode_array[file_ino]->indirect_block[0]);
        file_block = (int *)bbuf;
        int block_num;
        if(inode_array[file_ino]->size <= (8 + 1024) * BLOCK_SIZE)
            block_num = (inode_array[file_ino]->size) / BLOCK_SIZE + ((inode_array[file_ino]->size % BLOCK_SIZE) != 0) - 8;
        else
            block_num = 1024;
        
        for(i=0; i<block_num; i++){
            read_a_block((char *)file_offset, *file_block);
            file_block ++;
            file_offset += BLOCK_SIZE;
        }

        if(inode_array[file_ino]->size > (8 + 1024) * BLOCK_SIZE){
            read_a_block((char *)bbuf, inode_array[file_ino]->indirect_block[1]);
            file_block = (int *)bbuf;
            block_num = (inode_array[file_ino]->size) / BLOCK_SIZE + ((inode_array[file_ino]->size % BLOCK_SIZE) != 0) - 8 - 1024;
            for(i=0; i<block_num; i++){
                read_a_block((char *)file_offset, *file_block);
                file_block ++;
                file_offset += BLOCK_SIZE;
            }
        }
    }

    file_offset = fds[file_fd].base;
    vt100_move_cursor(1, 2);
    printk("%s", (void *)file_offset);
    release_a_fd(file_fd);
    return 0;
}

int hard_link(char *src_name, char *dest_name)
{
    int i, j, flag;
    dir_entry_t *now_dir_entry;
    int now_ino;

    if(fs_setup == 0){
        return -1;
    }

    //src path
    strcpy((char *)name_buf, src_name);
    int ret = analysis_path();
    if(ret != 0){
        return ret;
    }
    if(path_num == 0){
        return 3;  //invalid path
    }

    if(path_abs)
        now_dir_entry = &root_dir_entry;
    else
        now_dir_entry = &current_dir_entry;
    vt100_move_cursor(1, 2);
    printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num-1; i++){
        printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            return 4;  //path does not exist
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                return 5;  //path does not exist
            }
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }
    }
    if(path_num == 1){
        if(path_abs)
            now_ino = 0;
        else
            now_ino = current_dir_entry.ino[0];
        read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
        printk("read block: %d\n\r", inode_array[now_ino]->direct_block[0]);
        now_dir_entry = (dir_entry_t *)bbuf;
    }

    int file_id = -1;
    int file_ino;

    for(j=0; j<now_dir_entry->num_now; j++){
        if(strcmp(&name_buf[path_begin[path_num-1]], now_dir_entry->name[j]) == 0){
            file_id = j;
            break;
        }
    }
    if(file_id == -1){
        return 6;   //last level file not find
    }
    else if(file_id == 0 || file_id == 1){
        return 7;   //src . or .. dir
    }

    file_ino = now_dir_entry->ino[file_id];
    if(inode_array[file_ino]->type != TYPE_FILE){
        return 8;   //src not a file
    }


    //dest path
    strcpy((char *)name_buf, dest_name);
    ret = analysis_path();
    if(ret != 0){
        return ret + 10;
    }
    if(path_num == 0){
        return 13;  //invalid path
    }

    if(path_abs)
        now_dir_entry = &root_dir_entry;
    else
        now_dir_entry = &current_dir_entry;
    //vt100_move_cursor(1, 2);
    printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num-1; i++){
        printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            return 14;  //path does not exist
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                return 15;  //path does not exist
            }
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }
    }
    if(path_num == 1){
        if(path_abs)
            now_ino = 0;
        else
            now_ino = current_dir_entry.ino[0];
        read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
        printk("read block: %d\n\r", inode_array[now_ino]->direct_block[0]);
        now_dir_entry = (dir_entry_t *)bbuf;
    }

    if(now_dir_entry->num_now == now_dir_entry->num_max){
        return 16;  //dir too many
    }
    for(j=0; j<now_dir_entry->num_now; j++){
        if(strcmp(&name_buf[path_begin[path_num-1]], now_dir_entry->name[j]) == 0){
            return 17;  //already have same file
        }
    }

    now_dir_entry->ino[now_dir_entry->num_now] = file_ino;
    strcpy(now_dir_entry->name[now_dir_entry->num_now], &name_buf[path_begin[path_num-1]]);
    printk("path[%d]: ino: %d, name: %s\n\r", i, file_ino, now_dir_entry->name[now_dir_entry->num_now]);
    now_dir_entry->num_now++;
    write_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
    if(inode_array[now_ino]->leaf == TRUE){
        inode_array[now_ino]->leaf = FALSE;
        write_a_sector((char *)inode_buf[now_ino/8], super_block->inode_offset + now_ino/8);
    }
    if(now_ino == 0){
        root_dir_entry = *now_dir_entry;
    }
    if(now_ino == current_dir_entry.ino[0]){
        current_dir_entry = *now_dir_entry;
    }

    write_a_sector((char *)zero, 10000);
    return 0;
}

int soft_link(char *src_name, char *dest_name)
{
    int i, j, flag;
    dir_entry_t *now_dir_entry;
    int now_ino;

    if(fs_setup == 0){
        return -1;
    }

    strcpy((char *)name_buf, dest_name);
    int ret = analysis_path();
    if(ret != 0){
        return ret;
    }
    if(path_num == 0){
        return 3;  //invalid path
    }

    if(path_abs)
        now_dir_entry = &root_dir_entry;
    else
        now_dir_entry = &current_dir_entry;
    vt100_move_cursor(1, 2);
    printk("path_num: %d, path_abs: %d\n\r", path_num, path_abs);
    for(i=0; i<path_num-1; i++){
        printk("path[%d]: begin: %d, end: %d, name: %s\n\r", i, path_begin[i], path_end[i], &name_buf[path_begin[i]]);
        flag = 0;
        for(j=0; j<now_dir_entry->num_now; j++){
            if(strcmp(&name_buf[path_begin[i]], now_dir_entry->name[j]) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 0){
            return 4;  //path does not exist
        }
        else{
            now_ino = now_dir_entry->ino[j];
            if(inode_array[now_ino]->type != TYPE_DIR){
                return 5;  //path does not exist
            }
            read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
            now_dir_entry = (dir_entry_t *)bbuf;
        }
    }
    if(path_num == 1){
        if(path_abs)
            now_ino = 0;
        else
            now_ino = current_dir_entry.ino[0];
        read_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
        printk("read block: %d\n\r", inode_array[now_ino]->direct_block[0]);
        now_dir_entry = (dir_entry_t *)bbuf;
    }

    if(now_dir_entry->num_now == now_dir_entry->num_max){
        return 6;  //dir too many
    }
    for(j=0; j<now_dir_entry->num_now; j++){
        if(strcmp(&name_buf[path_begin[path_num-1]], now_dir_entry->name[j]) == 0){
            return 7;  //already have same file
        }
    }

    int new_ino = find_free_inode();
    int new_block = find_free_block();
    now_dir_entry->ino[now_dir_entry->num_now] = new_ino;
    strcpy(now_dir_entry->name[now_dir_entry->num_now], &name_buf[path_begin[path_num-1]]);
    printk("path[%d]: ino: %d, block: %d, name: %s\n\r", i, new_ino, new_block, now_dir_entry->name[now_dir_entry->num_now]);
    now_dir_entry->num_now++;
    write_a_block((char *)bbuf, inode_array[now_ino]->direct_block[0]);
    if(inode_array[now_ino]->leaf == TRUE){
        inode_array[now_ino]->leaf = FALSE;
        write_a_sector((char *)inode_buf[now_ino/8], super_block->inode_offset + now_ino/8);
    }
    if(now_ino == 0){
        root_dir_entry = *now_dir_entry;
    }
    if(now_ino == current_dir_entry.ino[0]){
        current_dir_entry = *now_dir_entry;
    }

    inode_array[new_ino]->size = strlen(src_name);
    inode_array[new_ino]->type = TYPE_LINK;
    inode_array[new_ino]->mode = O_RDWR;
    inode_array[new_ino]->leaf = TRUE;
    inode_array[new_ino]->direct_block[0] = new_block;
    write_a_sector((char *)inode_buf[new_ino/8], super_block->inode_offset + new_ino/8);
    printk("write inode array %d\n\r", new_ino/8);

    memset(bbuf, 0, sizeof(bbuf));
    strcpy((char *)bbuf, src_name);
    write_a_block((char *)bbuf, new_block);
    write_a_sector((char *)sup, 0);
    write_a_sector((char *)zero, 10000);
    return 0;
}

int print_fs(void)
{
    int cursor_x = 1;
    int cursor_y = SHELL_BEGIN + 1;
    int i;

    screen_clear(SHELL_BEGIN + 1, SCREEN_HEIGHT - 1);
    screen_move_cursor(1, SHELL_BEGIN + 1);
    cursor_y++;

    if(fs_setup == 0){
        kprintf("[FS] Filesystem has not set up!");
        return -1;
    }

    kprintf("magic: 0x%x", super_block->magic);
    screen_move_cursor(cursor_x, cursor_y);
    kprintf("seg0: start sector: %d (0x%x), used sector: %d/%d", super_block->start_sector, OFFSET_FS, super_block->sector_used, super_block->total_sector);
    cursor_y++;

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("      inode map offset: %d, size: %d sector, used: %d/%d", super_block->inode_map_offset, super_block->inode_map_size, super_block->inode_used, super_block->inode_map_size*512*8);
    cursor_y++;

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("      block map offset: %d, size: %d sector", super_block->block_map_offset, super_block->block_map_size);
    cursor_y++;

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("      inode offset: %d, size: %d sector", super_block->inode_offset, super_block->inode_array_size);
    cursor_y++;

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("seg1: start block: %d (0x%x), used block: %d/%d", super_block->data_start_block, OFFSET_DATA, super_block->block_used, super_block->data_total_block);
    cursor_y++;

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("      data offset: %d, size: %d block", 0, super_block->data_total_block);
    cursor_y++;

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("inode entry size: %dB, dir entry size: %dB", super_block->inode_entry_size, super_block->dir_entry_size);
    cursor_y++;
    return 0;
}

void clear_fs(void)
{
    int cursor_x = 1;
    int cursor_y = SHELL_BEGIN + 1;
    int i;

    screen_clear(SHELL_BEGIN + 1, SCREEN_HEIGHT - 1);
    screen_move_cursor(1, SHELL_BEGIN + 1);
    cursor_y++;

    if(fs_setup == 0){
        kprintf("[FS] Filesystem has not set up!");
        return;
    }

    write_a_sector((char *)zero, 0);
    fs_setup = 0;
    kprintf("[FS] Filesystem clear succeeded!");  
    return;
}

int init_fs(void)
{
    int cursor_x = 1;
    int cursor_y = SHELL_BEGIN + 1;
    int i, j;

    screen_clear(SHELL_BEGIN + 1, SCREEN_HEIGHT - 1);
    screen_move_cursor(1, SHELL_BEGIN + 1);
    cursor_y++;

    if(fs_setup == 1){
        kprintf("[FS] Filesystem has already set up!");
        return -1;
    }

    kprintf("[FS] Start initialize filesystem!");
    memset(sup, 0, sizeof(sup));
    memset(zero, 0, sizeof(zero));
    super_block = (super_block_t*)sup;
    super_block->magic = KFS_MAGIC;
    super_block->start_sector = OFFSET_FS / SECTOR_SIZE;
    super_block->total_sector = (OFFSET_DATA - OFFSET_FS) / SECTOR_SIZE;
    screen_move_cursor(cursor_x, cursor_y);
    kprintf("[FS] Setting superblock...");
    cursor_y++;
    screen_move_cursor(cursor_x, cursor_y);
    kprintf("     magic: 0x%x", super_block->magic);
    cursor_y++;
    screen_move_cursor(cursor_x, cursor_y);
    kprintf("seg0:start sector: %d, total sector: %d", super_block->start_sector, super_block->total_sector);
    cursor_y++;

    super_block->inode_map_offset = 1;
    super_block->inode_map_size = 1;
    super_block->inode_used = 1;
    screen_move_cursor(cursor_x, cursor_y);
    kprintf("     inode map offset: %d, size: %d", super_block->inode_map_offset, super_block->inode_map_size);
    cursor_y++;

    super_block->block_map_offset = 2;
    super_block->block_map_size = FS_SIZE / BLOCK_SIZE / 8 / SECTOR_SIZE; //(32)
    screen_move_cursor(cursor_x, cursor_y);
    kprintf("     block map offset: %d, size: %d", super_block->block_map_offset, super_block->block_map_size);
    cursor_y++;

    super_block->inode_offset = super_block->block_map_size + 2;
    super_block->inode_array_size = 512;
    screen_move_cursor(cursor_x, cursor_y);
    kprintf("     inode offset: %d, size: %d", super_block->inode_offset, super_block->inode_array_size);
    cursor_y++;

    super_block->data_start_block = OFFSET_DATA / BLOCK_SIZE;
    super_block->data_total_block = FS_SIZE / BLOCK_SIZE;
    screen_move_cursor(cursor_x, cursor_y);
    kprintf("seg1:data start block: %d, total block: %d", super_block->data_start_block, super_block->data_total_block);
    cursor_y++;

    super_block->sector_used = 1 + super_block->inode_map_size + super_block->block_map_size + super_block->inode_array_size;
    super_block->block_used = 1;

    super_block->inode_entry_size = sizeof(inode_entry_t);
    super_block->dir_entry_size = sizeof(dir_entry_t);
    screen_move_cursor(cursor_x, cursor_y);
    kprintf("     inode entry size: %dB, dir entry size: %dB", sizeof(inode_entry_t), sizeof(dir_entry_t));
    cursor_y++;
    write_a_sector((char *)sup, 0);

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("[FS] Setting inode-map...");
    cursor_y++;
    memset(imap_buf, 0, sizeof(imap_buf));
    imap_buf[0] = 0x1;
    write_a_sector((char *)imap_buf, 1);

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("[FS] Setting block-map...");
    cursor_y++;
    memset(bmap_buf, 0, sizeof(bmap_buf));
    /*for(i=0; i<SECTOR_SIZE; i++){
        if(use_sec < 8){
            int j;
            for(j=0; i<use_sec; j++){
                secmap_buf[0][i] = (secmap_buf[0][i]<<1) + 1;
            }
            break;
        }
        else{
            secmap_buf[0][i] = 0xff;
            use_sec -= 8;
        }
    }*/
    bmap_buf[0][0] = 0x1;
    write_a_sector((char *)bmap_buf[0], 2);
    for(i=3; i<super_block->inode_offset; i++){
        write_a_sector((char *)zero, i);
    }

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("[FS] Setting inode...");
    cursor_y++;
    memset(inode_buf, 0, sizeof(inode_buf));
    for(i=0; i<16; i++){
        for(j=0; j<8; j++){
            inode_array[i * 8 + j] = (inode_entry_t*)(&inode_buf[i][j * 64]);
        }
    }
    inode_array[0]->size = sizeof(dir_entry_t);
    inode_array[0]->type = TYPE_DIR;
    inode_array[0]->leaf = TRUE;
    inode_array[0]->direct_block[0] = 0;
    current_inode_entry = *(inode_array[0]);
    write_a_sector((char *)inode_buf[0], super_block->inode_offset);
    for(i=super_block->inode_offset+1; i<super_block->inode_offset+super_block->inode_array_size; i++){
        write_a_sector((char *)zero, i);
    }
    memset(bbuf, 0, sizeof(bbuf));
    new_dir_entry = (dir_entry_t*)bbuf;
    new_dir_entry->num_now = 2;
    new_dir_entry->num_max = 12;
    new_dir_entry->ino[0] = 0;
    new_dir_entry->ino[1] = 0;
    new_dir_entry->name[0][0] = '.';
    new_dir_entry->name[0][1] = '\0';
    new_dir_entry->name[1][0] = '.';
    new_dir_entry->name[1][1] = '.';
    new_dir_entry->name[1][2] = '\0';
    root_dir_entry = current_dir_entry = *new_dir_entry;
    write_a_block((char *)bbuf, 0);
    read_a_block((char *)bbuf, 0);
    new_dir_entry = (dir_entry_t*)bbuf;
    current_dir_entry = *new_dir_entry;
    path_buf[0] = '/';
    path_buf[1] = '\0';
    path_ptr = 1;
    for(i=0; i<NUM_FD; i++){
        fds_free[i] = 0;
    }

    screen_move_cursor(cursor_x, cursor_y);
    kprintf("[FS] Initialize filesystem finished!");
    fs_setup = 1;
    return 0;
}