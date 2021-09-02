#include "barrier.h"

void do_barrier_init(barrier_t *barrier, int goal)
{
    barrier->now = 0;
    barrier->goal = goal;
    barrier->block_task_count = 0;
}

void do_barrier_wait(barrier_t *barrier)
{
    (barrier->now)++;
    if(barrier->now < barrier->goal){
        do_block(barrier->block_task, &(barrier->block_task_count));
    }
    else{
        barrier->now = 0;
        do_unblock_all(barrier->block_task, &(barrier->block_task_count));
    }
}
