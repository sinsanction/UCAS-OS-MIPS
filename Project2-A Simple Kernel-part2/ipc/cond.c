#include "cond.h"
#include "lock.h"

/* clear queue */
void do_condition_init(condition_t *condition)
{
    
}

/* The lock has been obtained before the task calls the function */
void do_condition_wait(mutex_lock_t *lock, condition_t *condition)
{
    /* [1] release the lock so other task can operate on the condition */
    

    /* [2] do scheduler */
   

    /* [3] acquire lock */
    
}

/* unblock one task */
void do_condition_signal(condition_t *condition)
{
     
}

/* unblock all task */
void do_condition_broadcast(condition_t *condition)
{
     
}