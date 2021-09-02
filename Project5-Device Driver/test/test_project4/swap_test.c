#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "screen.h"
#include "mm.h"
#include "test4.h"

void swap_task(void)
{
    uint64_t swap_task_base = 0x100000;
    int i, j, k;
    uint32_t print_location = 1;
    
    for(i=0; i<500; i++){
        for(j=0; j<1; j++){
            for(k=0; k<12; k++){
                *(int *)(swap_task_base + k*0x2000) = 1;
            }
            for(k=0; k<5; k++){
                *(int *)(swap_task_base + k*0x2000) = 1;
            }
        }
        for(j=0; j<8; j++){
            sys_move_cursor(1, print_location + j);
            printf("physical frame[%d]: swap times: %d ", j, physical_frame[j].swap_times);
        }
    }
    sys_exit();
}
