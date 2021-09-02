#include "type.h"
#include "stdio.h"
#include "syscall.h"
#include "string.h"

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3

static char buff[64];

void __attribute__((section(".entry_function"))) _start(void)
{
    int i, j;
    int fd = sys_fopen("1.txt", O_RDWR);

    if(fd == -1){
        sys_move_cursor(0, 7);
        printf("open file fail");
        sys_exit();
    }
    sys_move_cursor(0, 4);
    printf("open file success, fd: %d", fd);

    // write 'hello world!' * 10
    for (i = 0; i < 10; i++){
        int ret = sys_fwrite(fd, "hello world!\n\r", 14);
        //sys_move_cursor(0, 7+i);
        //printf("write %d: %d ", i, ret);
    }

    // read
    for (i = 0; i < 10; i++){
        int ret = sys_fread(fd, buff, 14);
        sys_move_cursor(0, 5+i);
        //printf("read %d: %d ", i, ret);
        for (j = 0; j < 14; j++){
            printf("%c", buff[j]);
        }
    }

    sys_close(fd);
    sys_exit();
}