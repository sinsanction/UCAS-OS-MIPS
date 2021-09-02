#include "barrier.h"

void do_barrier_init(barrier_t *barrier, int goal)
{
    barrier->goal = goal;
    barrier->block_task_count = 0;
}

void do_barrier_wait(barrier_t *barrier)
{
    (barrier->goal)--;
    if(barrier->goal){
        do_block(barrier->block_task, &(barrier->block_task_count));
    }
    else{
        do_unblock_all(barrier->block_task, &(barrier->block_task_count));
    }
    (barrier->goal)++;
}
