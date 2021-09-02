#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"

#define KERNEL_SP_BASE 0xffffffffa0900000
#define USER_SP_BASE 0xffffffffa0f00000

pcb_t pcb[NUM_MAX_TASK];

/* current running task PCB */
pcb_t *current_running;

/* global process id */
pid_t process_id = 1;

/* kernel stack ^_^ */
#define NUM_KERNEL_STACK 32

static uint64_t kernel_stack[NUM_KERNEL_STACK];
static int kernel_stack_count = 0;

static uint64_t user_stack[NUM_KERNEL_STACK];
static int user_stack_count = 0;

void init_stack(void)
{
    int i;
    for(i=0; i<NUM_KERNEL_STACK; i++){
        kernel_stack[i] = KERNEL_SP_BASE + (i+1) * 0x1000;
        user_stack[i] = USER_SP_BASE + (i+1) * 0x1000;
    }
}

void init_kernel_stack(pid_t pid, pcb_t *pcb)
{
    regs_context_t *pt_regs;
    pcb[pid].kernel_sp -= sizeof(regs_context_t);
    pt_regs = (regs_context_t *)pcb[pid].kernel_sp;
    memset(pt_regs, 0, sizeof(regs_context_t));
    
    uint64_t *reg = (uint64_t *)pt_regs;
    reg[29] = pcb[pid].user_sp;
    reg[31] = reg[37] = pcb[pid].entry_point;
    //reg[32] = initial_cp0_status;
    //reg[33] = initial_cp0_cause;
}

uint64_t new_kernel_stack(void)
{
    if(kernel_stack_count < NUM_KERNEL_STACK)
    return kernel_stack[kernel_stack_count++];
}

uint64_t new_user_stack(void)
{
    if(user_stack_count < NUM_KERNEL_STACK)
    return user_stack[user_stack_count++];
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
    pcb[pid].pid = pid;
    pcb[pid].kernel_sp = new_kernel_stack();
    pcb[pid].user_sp = new_user_stack();
    pcb[pid].entry_point = task_info->entry_point;
    init_kernel_stack(pid, pcb);

    pcb[pid].type = task_info->type;
    pcb[pid].status = TASK_READY;
    pcb[pid].inqueue = READY;
    strcpy(pcb[pid].name, task_info->name);

    pcb[pid].priority = 0;
    pcb[pid].prev = NULL;
    pcb[pid].next = NULL;
    pcb[pid].holdlock = 0;
    queue_push(&ready_queue, &pcb[pid]);
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
    current_running->cursor_x = screen_cursor_x;
    current_running->cursor_y = screen_cursor_y;

    if(current_running->status == TASK_RUNNING){
        current_running->status = TASK_READY;
        queue_push(&ready_queue, current_running);
    }
    if(!queue_is_empty(&ready_queue)){
        current_running = (pcb_t *)queue_dequeue(&ready_queue);
        current_running->status = TASK_RUNNING;
        screen_cursor_x = current_running->cursor_x;
        screen_cursor_y = current_running->cursor_y;
    }
}

void do_sleep(uint32_t sleep_time)
{
     
}

void do_exit(void)
{
    
}

void do_block(queue_t *queue, mutex_lock_t *lock)
{
    current_running->status = TASK_BLOCKED;
    current_running->inqueue = BLOCK;
    lock->block_task[++(lock->block_task_count)] = current_running->pid;
    //queue_push(&block_queue, current_running);
    do_scheduler();
}

void do_unblock_one(queue_t *queue, pcb_t *unblock_one)
{
    unblock_one->status = TASK_READY;
    unblock_one->inqueue = READY;
    //queue_remove(queue, unblock_one);
    queue_push(&ready_queue, unblock_one);
}

void do_unblock_all(queue_t *queue, mutex_lock_t *lock)
{
    pcb_t *unblock_one;
    while(lock->block_task_count){
        unblock_one = &pcb[lock->block_task[lock->block_task_count]];
        do_unblock_one(queue, unblock_one);
        (lock->block_task_count)--;
    }
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