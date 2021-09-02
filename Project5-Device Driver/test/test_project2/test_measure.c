#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "test.h"

void measure_task1(void)
{
    int i;
    uint32_t average_ticks;
    uint32_t time_ticks = 0;
    uint32_t begin_time;
    uint32_t end_time;
    disable_interrupt();
    set_cp0_count(0);
    begin_time = get_cp0_count();
    end_time = get_cp0_count();
    vt100_move_cursor(1, 2);
    printk("> [TASK] This error ticks is: %u ticks \n", (end_time - begin_time)*2);

    for(i=0; i<1000; i++){
        begin_time = get_cp0_count();
        do_scheduler();
        end_time = get_cp0_count();
        time_ticks += (end_time - begin_time)*2;

        vt100_move_cursor(1, 5);
        printk("> [TASK] This total ticks of do_scheduler is: %u ticks  (%d) \n", time_ticks, i);
    }

    average_ticks = time_ticks / 1000;
    vt100_move_cursor(1, 3);
    printk("> [TASK] This average ticks of do_scheduler is: %u ticks\n", average_ticks);

    while(1){
        ;
    }
}
