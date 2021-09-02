#include "type.h"
#include "stdio.h"
#include "syscall.h"

#define NUM_PF 14

int pf_swap_num[NUM_PF];

void __attribute__((section(".entry_function"))) _start(void)
{
    uint64_t swap_task_base = 0x100000;
    int i, j, k;
    uint32_t print_location = 1;

    for(i=0; i<500; i++){
        for(k=0; k<10; k++){
            *(int *)(swap_task_base + k*0x2000) = 1;
        }
        for(j=0; j<NUM_PF; j++){
            sys_get_pfnum(pf_swap_num);
            sys_move_cursor(1, print_location + j);
            printf("physical frame[%d]: swap times: %d ", j, pf_swap_num[j]);
        }
    }
    sys_exit();
}
