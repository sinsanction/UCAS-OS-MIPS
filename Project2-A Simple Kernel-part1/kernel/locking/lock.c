#include "lock.h"
#include "sched.h"
#include "syscall.h"

void spin_lock_init(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void spin_lock_acquire(spin_lock_t *lock)
{
    while (LOCKED == lock->status)
    {
    };
    lock->status = LOCKED;
}

void spin_lock_release(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void do_mutex_lock_init(mutex_lock_t *lock)
{
    lock->status = UNLOCKED;
    lock->holder = 0;
    lock->block_task_count = 0;
}

void do_mutex_lock_acquire(mutex_lock_t *lock)
{
    while(lock->status == LOCKED){
        do_block(&block_queue, lock);
    }
    lock->status = LOCKED;
    lock->holder = current_running->pid;
    (current_running->holdlock)++;
}

void do_mutex_lock_release(mutex_lock_t *lock)
{
    do_unblock_all(&block_queue, lock);
    lock->status = UNLOCKED;
    lock->holder = 0;
    (current_running->holdlock)--;
}
