#include "lock.h"
#include "sched.h"
#include "syscall.h"

mutex_lock_t lock_queue[NUM_MAX_LOCK];
binsem_t binsem_queue[NUM_MAX_LOCK];

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

void do_mutex_lock_init(lock_id_t lock_id)
{
    lock_queue[lock_id].status = UNLOCKED;
    lock_queue[lock_id].holder = 0;
    lock_queue[lock_id].block_task_count = 0;
}

void do_mutex_lock_acquire(lock_id_t lock_id)
{
    while(lock_queue[lock_id].status == LOCKED){
        do_block(lock_queue[lock_id].block_task, &(lock_queue[lock_id].block_task_count));
    }
    lock_queue[lock_id].status = LOCKED;
    lock_queue[lock_id].holder = current_running->pid;
    current_running->hold_lock[++(current_running->hold_lock_count)] = lock_id;
}

void do_mutex_lock_release(lock_id_t lock_id)
{
    do_unblock_all(lock_queue[lock_id].block_task, &(lock_queue[lock_id].block_task_count));
    lock_queue[lock_id].status = UNLOCKED;
    lock_queue[lock_id].holder = 0;
    int i;
    for(i=1; i<NUM_MAX_LOCK; i++){
        if(current_running->hold_lock[i] == lock_id)
            break;
    }
    if(i == current_running->hold_lock_count){
        (current_running->hold_lock_count)--;
    }
    else{
        for(; i<(current_running->hold_lock_count); i++){
            current_running->hold_lock[i] = current_running->hold_lock[i+1];
        }
        (current_running->hold_lock_count)--;
    }
}

void do_binsem_init(binsem_id_t binsem_id)
{
    binsem_queue[binsem_id].binsem = 1;
    binsem_queue[binsem_id].block_task_count = 0;
}

void do_binsem_acquire(binsem_id_t binsem_id)
{  
    while(binsem_queue[binsem_id].binsem <= 0){
        do_block(binsem_queue[binsem_id].block_task, &(binsem_queue[binsem_id].block_task_count));
    }
    (binsem_queue[binsem_id].binsem)--;
}

void do_binsem_release(binsem_id_t binsem_id)
{
    (binsem_queue[binsem_id].binsem)++;
    do_unblock_all(binsem_queue[binsem_id].block_task, &(binsem_queue[binsem_id].block_task_count));
}

int do_binsemget(int key)
{
    return hash((uint64_t)key);
}

int hash(uint64_t x)
{
    // a simple hash function
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ul;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebul;
    x = x ^ (x >> 31);
    return x % NUM_MAX_LOCK;
}
