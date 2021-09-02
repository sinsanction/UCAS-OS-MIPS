#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"
#include "irq.h"

#define KERNEL_SP_BASE 0xffffffffa0900000
#define USER_SP_BASE 0xffffffffa0f00000

pcb_t pcb[NUM_MAX_TASK];

uint32_t priority_timeslice[NUM_MAX_PRIORITY] = {0, 8, 4, 2};

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
    reg[32] = initial_cp0_status;
    reg[33] = initial_cp0_cause;
    reg[39] = pcb[pid].kernel_state;
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

    pcb[pid].type = task_info->type;
    if(task_info->type == KERNEL_PROCESS || task_info->type == KERNEL_THREAD)
        pcb[pid].kernel_state = TRUE;
    else
        pcb[pid].kernel_state = FALSE;
    pcb[pid].status = TASK_READY;
    pcb[pid].inqueue = READY;
    strcpy(pcb[pid].name, task_info->name);
    init_kernel_stack(pid, pcb);

    pcb[pid].priority = task_info->priority;
    pcb[pid].runtime_priority = task_info->priority;
    pcb[pid].timeslice = priority_timeslice[task_info->priority];
    pcb[pid].runtimes = 0;
    pcb[pid].prev = NULL;
    pcb[pid].next = NULL;
    pcb[pid].holdlock = 0;
    queue_push(&ready_queue[pcb[pid].priority], &pcb[pid]);
}

/* ready queue to run */
queue_t ready_queue[NUM_MAX_PRIORITY];

/* block queue to wait */
queue_t block_queue;

static void check_sleeping(pcb_t *now)
{
    uint32_t begin_time = get_timer();
    if(begin_time - now->begin_time >= now->sleep_time){
        now->status = TASK_READY;
        now->inqueue = READY;
        now->begin_time = 0;
        now->sleep_time = 0;
        queue_push(&ready_queue[now->runtime_priority], now);
    }
    else{
        queue_push(&block_queue, now);
    }
}

int refill_num = 0;
static void refill_ready_queue(void)
{
    refill_num++;
    vt100_move_cursor(1, 15);
    printk("[TEST] refill ready queue:  %d    times", refill_num);

    pcb_t *now;
    while(!queue_is_empty(&ready_queue[0])){
        now = (pcb_t *)queue_dequeue(&ready_queue[0]);
        now->status = TASK_READY;
        now->runtime_priority = now->priority;
        now->timeslice = priority_timeslice[now->priority];
        queue_push(&ready_queue[now->priority], now);
    }
}

void scheduler(void)
{
    if(current_running->kernel_state == FALSE)
        print_error();
    
    current_running->cursor_x = screen_cursor_x;
    current_running->cursor_y = screen_cursor_y;
    current_running->timeslice--;
    /* when the timeslice is exhausted, reduce the priority and give a new timeslice */
    if(current_running->timeslice == 0){
        current_running->runtime_priority--;
        current_running->timeslice = priority_timeslice[current_running->runtime_priority];
    }
    if(current_running->status == TASK_RUNNING){
        current_running->status = TASK_READY;
        queue_push(&ready_queue[current_running->runtime_priority], current_running);
    }

    if(!queue_is_empty(&block_queue)){
        pcb_t *block_one;
        block_one = (pcb_t *)queue_dequeue(&block_queue);
        check_sleeping(block_one);
    }

    int i;
    FIND_NEXT_PROC:
    /* select the next process from the highest priority queue */
    i = NUM_MAX_PRIORITY - 1;
    for(; i>0 ; i--){
        if(!queue_is_empty(&ready_queue[i])){
            current_running = (pcb_t *)queue_dequeue(&ready_queue[i]);
            current_running->status = TASK_RUNNING;
            current_running->runtimes++;
            screen_cursor_x = current_running->cursor_x;
            screen_cursor_y = current_running->cursor_y;
            return;
        }
    }
    /* the processes in ready_queue[0] can not be scheduled */
    /* if the high priority queue has no schedulable processes, */
    /* then refill the entire priority queue according to the ready_queue[0] and starting priority */
    refill_ready_queue();
    goto FIND_NEXT_PROC;
    return;
}

void do_sleep(uint32_t sleep_time)
{
    current_running->status = TASK_BLOCKED;
    current_running->inqueue = BLOCK;
    current_running->begin_time = get_timer();
    current_running->sleep_time = sleep_time;
    queue_push(&block_queue, current_running);
    do_scheduler();
}

void do_exit(void)
{
    
}

void do_block(pid_t *block_task_queue, uint32_t *block_task_count)
{
    current_running->status = TASK_BLOCKED;
    current_running->inqueue = BLOCK;
    block_task_queue[++(*block_task_count)] = current_running->pid;
    do_scheduler();
}

void do_unblock_one(pcb_t *unblock_one)
{
    unblock_one->status = TASK_READY;
    unblock_one->inqueue = READY;
    queue_push(&ready_queue[unblock_one->runtime_priority], unblock_one);
}

void do_unblock_all(pid_t *block_task_queue, uint32_t *block_task_count)
{
    pcb_t *unblock_one;
    while(*block_task_count){
        unblock_one = &pcb[block_task_queue[*block_task_count]];
        do_unblock_one(unblock_one);
        (*block_task_count)--;
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
    return current_running->pid;
}

void do_getpriority(priority_info_t *prio)
{
    prio->priority = current_running->priority;
    prio->runtime_priority = current_running->runtime_priority;
    prio->runtimes = current_running->runtimes;
}
