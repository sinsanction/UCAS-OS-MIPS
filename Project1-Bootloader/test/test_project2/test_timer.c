#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "test.h"
#include "time.h"

void timer_task(void)
{
    int count = 0;
    int print_location = 9;

    while (1)
    {
        /* call get_timer() to get time */
        uint32_t time = get_timer();
        sys_move_cursor(1, print_location);
        printf("> [TASK] This is a thread to timing! (%u/%u seconds).\n", time, time_elapsed);
    }
}
