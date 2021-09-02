#include "type.h"
#include "stdio.h"
#include "syscall.h"

#define SHM_KEY 1

void __attribute__((section(".entry_function"))) _start(void)
{
    uint64_t key, shmid;
    int i = 0;
    uint32_t print_location = 1;
   
    key = SHM_KEY;
    
    shmid = shmget(key);
    sys_move_cursor(0, print_location);
    if (shmid == -1)
    {
        printf("share memory task fault!\n");
        return;
    }
    
    char *addr = (char *)shmat(shmid);
    if ((int64_t)addr == -1)
    {
        printf("share memory task fault!\n");
        return;
    }
    sys_sleep(2);

    while (i < 10)
    {
        addr[i] = 'A' + i;
        i++;
        addr[i] = 0;
        sys_sleep(2);
    }
    shmdt((uint64_t)addr);
    sys_exit();
}
