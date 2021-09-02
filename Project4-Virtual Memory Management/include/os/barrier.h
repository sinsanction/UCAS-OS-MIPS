#ifndef INCLUDE_BARRIER_H_
#define INCLUDE_BARRIER_H_

#include "queue.h"
#include "lock.h"

typedef struct barrier
{
    int goal;
    pid_t block_task[NUM_MAX_TASK];
    uint32_t block_task_count;

} barrier_t;

void do_barrier_init(barrier_t *, int);
void do_barrier_wait(barrier_t *);

#endif