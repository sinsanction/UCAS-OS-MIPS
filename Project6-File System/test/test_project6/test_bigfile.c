#include "type.h"
#include "stdio.h"
#include "syscall.h"
#include "string.h"

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3

static char buff[4096] = "test big file: \0    \0";

void __attribute__((section(".entry_function"))) _start(void)
{
    int i, j;
    int *num;
    int fd = sys_fopen("bigfile.txt", O_RDWR);

    if(fd == -1){
        sys_move_cursor(0, 7);
        printf("open file fail");
        sys_exit();
    }
    sys_move_cursor(0, 5);
    printf("open file success, fd: %d", fd);

    // write 'hello world!' * 10
    for (i = 0; i < 2 * 1024; i++){
        num = (int *)&(buff[16]);
        *num = i;
        int ret = sys_fwrite(fd, buff, 4096);
        sys_move_cursor(0, 6);
        printf("write %d: %d ", i, ret);
    }

    // read
    for (i = 0; i < 2 * 1024; i++){
        int ret = sys_fread(fd, buff, 4096);
        num = (int *)&(buff[16]);
        sys_move_cursor(0, 7);
        printf("read %d: %d ", i, ret);
        sys_move_cursor(0, 8);
        printf("%s %d", buff, *num);
    }

    sys_close(fd);
    sys_exit();
}