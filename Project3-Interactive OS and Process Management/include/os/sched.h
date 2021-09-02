/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *        Process scheduling related content, such as: scheduler, process blocking, 
 *                 process wakeup, process creation, process kill, etc.
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

#ifndef INCLUDE_SCHEDULER_H_
#define INCLUDE_SCHEDULER_H_

#include "type.h"
#include "lock.h"
#include "queue.h"
#include "mailbox.h"

#define NUM_MAX_TASK 32
#define CORE_NUM 2
#define READY 1
#define BLOCK 2
#define NUM_MAX_PRIORITY 4
/* used to save register infomation */
typedef struct regs_context
{
    uint64_t common[32];
    uint64_t cp0[7];
    uint64_t pre_kernel_state;
    
} regs_context_t; /* 256 + 56 = 312B */

typedef enum
{
    TASK_BLOCKED,
    TASK_RUNNING,
    TASK_READY,
    TASK_EXITED,
} task_status_t;

typedef enum
{
    KERNEL_PROCESS,
    KERNEL_THREAD,
    USER_PROCESS,
    USER_THREAD,
} task_type_t;

/* Process Control Block */
typedef struct pcb
{
    /* register context */
    uint64_t pcb_sp;
    uint64_t kernel_sp;
    uint64_t user_sp;
    uint64_t entry_point;

    /* state */
    uint64_t kernel_state;

    /* previous, next pointer */
    struct pcb_t *prev;
    struct pcb_t *next;
     
    /* task in which queue */
    uint32_t inqueue;

    /* What tasks are blocked by me, the tasks in this 
     * queue need to be unblocked when I do_exit(). */
    pid_t block_task[NUM_MAX_TASK];
    uint32_t block_task_count;

    /* holding lock */
    lock_id_t hold_lock[NUM_MAX_LOCK];
    uint32_t hold_lock_count;

    /* sleep related */
    uint32_t begin_time;
    uint32_t sleep_time;

    /* priority */
    uint32_t priority;
    uint32_t runtime_priority;
    int timeslice;
    int runtimes;

    /* name */
    char name[32];
    /* process id */
    pid_t pid;
    /* kernel/user thread/process */
    task_type_t type;
    /* BLOCK | READY | RUNNING */
    task_status_t status;
    /* cursor position */
    int cursor_x;
    int cursor_y;

} pcb_t;

/* task information, used to init PCB */
typedef struct task_info
{
    char name[32];
    uint64_t entry_point;
    task_type_t type;
    uint32_t priority;
} task_info_t;

/* task priority information, used to test priority scheduling */
typedef struct priority_info
{
    uint32_t priority;
    uint32_t runtime_priority;
    int runtimes;
} priority_info_t;

/* ready queue to run */
extern queue_t ready_queue[NUM_MAX_PRIORITY];

/* block queue to wait */
extern queue_t block_queue ;

/* current running task PCB */
extern pcb_t *current_running;
extern pid_t process_id;

extern pcb_t pcb[NUM_MAX_TASK];
extern uint32_t initial_cp0_status;

extern uint32_t priority_timeslice[NUM_MAX_PRIORITY];

void do_scheduler(void);
void scheduler(void);

pid_t do_spawn(task_info_t *);
void do_exit(void);
void do_sleep(uint32_t);

int do_kill(pid_t pid);
int do_waitpid(pid_t pid);

void do_block(pid_t *, uint32_t *);
void do_unblock_one(pcb_t *);
void do_unblock_all(pid_t *, uint32_t *);

void init_stack(void);
void set_pcb(pid_t, pcb_t *, task_info_t *);

int do_process_show(int *);
pid_t do_getpid(void);
uint64_t get_cpu_id(void);
void do_getpriority(priority_info_t *);

#endif