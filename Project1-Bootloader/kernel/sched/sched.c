#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"

pcb_t pcb[NUM_MAX_TASK];

/* current running task PCB */
pcb_t *current_running;

/* global process id */
pid_t process_id = 1;

/* kernel stack ^_^ */
#define NUM_KERNEL_STACK 20

static uint64_t kernel_stack[NUM_KERNEL_STACK];
static int kernel_stack_count;

static uint64_t user_stack[NUM_KERNEL_STACK];
static int user_stack_count;

void init_stack()
{
     
}

uint64_t new_kernel_stack()
{
     
}

uint64_t new_user_stack()
{
     
}

static void free_kernel_stack(uint64_t stack_addr)
{
     
}

static void free_user_stack(uint64_t stack_addr)
{
     
}

/* Process Control Block */
void set_pcb(pid_t pid, pcb_t *pcb, task_info_t *task_info)
{
     
}

/* ready queue to run */
queue_t ready_queue ;

/* block queue to wait */
queue_t block_queue ;

static void check_sleeping()
{
     
}

void scheduler(void)
{
     
}

void do_sleep(uint32_t sleep_time)
{
     
}

void do_exit(void)
{
    
}

void do_block(queue_t *queue)
{
    
}

void do_unblock_one(queue_t *queue)
{
     
}

void do_unblock_all(queue_t *queue)
{
    
}

int do_spawn(task_info_t *task)
{
     
}

int do_kill(pid_t pid)
{
    
}

int do_waitpid(pid_t pid)
{
    
}

// process show
void do_process_show()
{

     
}

pid_t do_getpid()
{
     
}