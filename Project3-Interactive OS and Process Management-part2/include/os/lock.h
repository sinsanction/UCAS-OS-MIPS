/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                                   Thread Lock
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#ifndef INCLUDE_LOCK_H_
#define INCLUDE_LOCK_H_

#include "queue.h"

#define NUM_MAX_TASK 32
#define NUM_MAX_LOCK 32

#define BINSEM_OP_LOCK 0
#define BINSEM_OP_UNLOCK 1

typedef enum {
    UNLOCKED,
    LOCKED,
} lock_status_t;

typedef struct spin_lock
{
    lock_status_t status;
} spin_lock_t;

typedef struct mutex_lock
{
    lock_status_t status;
    pid_t holder;
    pid_t block_task[NUM_MAX_TASK];
    uint32_t block_task_count;

} mutex_lock_t;

typedef struct binsem
{
    int binsem;
    pid_t block_task[NUM_MAX_TASK];
    uint32_t block_task_count;

} binsem_t;

typedef int binsem_id_t;
typedef int lock_id_t;

extern mutex_lock_t lock_queue[NUM_MAX_LOCK];
extern binsem_t binsem_queue[NUM_MAX_LOCK];

/* init lock */
void spin_lock_init(spin_lock_t *lock);
void spin_lock_acquire(spin_lock_t *lock);
void spin_lock_release(spin_lock_t *lock);

void do_mutex_lock_init(lock_id_t lock_id);
void do_mutex_lock_acquire(lock_id_t lock_id);
void do_mutex_lock_release(lock_id_t lock_id);

void do_binsem_init(binsem_id_t binsem_id);
void do_binsem_acquire(binsem_id_t binsem_id);
void do_binsem_release(binsem_id_t binsem_id);
int do_binsemget(int key);

#endif
