#include "cond.h"
#include "lock.h"
#include "sched.h"

/* clear queue */
void do_condition_init(condition_t *condition)
{
    condition->block_task_count = 0;
}

/* The lock has been obtained before the task calls the function */
void do_condition_wait(lock_id_t lock_id, condition_t *condition)
{
    /* [1] release the lock so other task can operate on the condition */
    do_mutex_lock_release(lock_id);
    /* [2] do scheduler */
    do_block(condition->block_task, &(condition->block_task_count));
    /* [3] acquire lock */
    do_mutex_lock_acquire(lock_id);
}

/* unblock one task */
void do_condition_signal(condition_t *condition)
{
    pcb_t *unblock_one;
    if(condition->block_task_count){
        pid_t unblock_one_pid = get_pcb(condition->block_task[condition->block_task_count]);
        if(unblock_one_pid > 0){
            unblock_one = &pcb[unblock_one_pid];
            do_unblock_one(unblock_one);
        }
        (condition->block_task_count)--;
    }
}

/* unblock all task */
void do_condition_broadcast(condition_t *condition)
{
    do_unblock_all(condition->block_task, &(condition->block_task_count));
}
