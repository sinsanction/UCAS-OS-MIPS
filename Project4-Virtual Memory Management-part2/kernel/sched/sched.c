#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"
#include "irq.h"
#include "mm.h"
#include "test.h"

#define KERNEL_SP_BASE 0xffffffffa0f00000
//#define USER_SP_BASE 0xffffffffa1f00000
//#define USER_SP_BASE 0x10000000
#define USER_SP_BASE 0x70000000

pcb_t pcb[NUM_MAX_TASK];

uint32_t priority_timeslice[NUM_MAX_PRIORITY] = {0, 8, 4, 2};

/* current running task PCB */
pcb_t *current_running;

/* global process id */
pid_t process_id = 1;

/* kernel stack ^_^ */
#define NUM_KERNEL_STACK 32

static uint64_t kernel_stack[NUM_KERNEL_STACK];
static uint32_t kernel_stack_free[NUM_KERNEL_STACK];

static uint64_t user_stack[NUM_KERNEL_STACK];
static uint32_t user_stack_free[NUM_KERNEL_STACK];

void init_stack(void)
{
    int i;
    for(i=0; i<NUM_KERNEL_STACK; i++){
        kernel_stack[i] = KERNEL_SP_BASE + (i+1) * 0x2000;
        user_stack[i] = USER_SP_BASE + (i+1) * 0x2000;
        kernel_stack_free[i] = 1;
        user_stack_free[i] = 1;
    }
}

void init_kernel_stack(pid_t pid, pcb_t *pcb, uint64_t argv1, uint64_t argv2)
{
    pcb[pid].pcb_sp = pcb[pid].kernel_sp - 0x1000 - sizeof(regs_context_t);
    uint64_t *reg = (uint64_t *)pcb[pid].pcb_sp;
    memset(reg, 0, sizeof(regs_context_t));
    
    reg[4]  = argv1;
    reg[5]  = argv2;
    reg[28] = get_reg_gp();
    reg[29] = pcb[pid].user_sp;
    reg[31] = reg[37] = pcb[pid].entry_point;
    reg[32] = initial_cp0_status;
    reg[33] = initial_cp0_cause;
    reg[39] = pcb[pid].kernel_state;
}

uint64_t new_kernel_stack(void)
{
    int i;
    for(i=0; i<NUM_KERNEL_STACK; i++){
        if(kernel_stack_free[i]){
            kernel_stack_free[i] = 0;
            return kernel_stack[i];
        }
    }
}

uint64_t new_user_stack(void)
{
    int i;
    for(i=0; i<NUM_KERNEL_STACK; i++){
        if(user_stack_free[i]){
            user_stack_free[i] = 0;
            return user_stack[i];
        }
    }
}

static void free_kernel_stack(uint64_t stack_addr)
{
    int i = (stack_addr - KERNEL_SP_BASE)/0x2000 - 1;
    kernel_stack_free[i] = 1;
}

static void free_user_stack(uint64_t stack_addr)
{
    int i = (stack_addr - USER_SP_BASE)/0x2000 - 1;
    user_stack_free[i] = 1;
}

static pid_t get_new_pcb(void)
{
    int i;
    for(i=1; i<NUM_MAX_TASK; i++){
        if(pcb[i].pid == -1){
            return i;
        }
    }
    return -1;
}

static pid_t get_pcb(pid_t pid)
{
    int i;
    for(i=1; i<NUM_MAX_TASK; i++){
        if(pcb[i].pid == pid){
            return i;
        }
    }
    return -1;
}

/* Process Control Block */
int set_pcb(pid_t pid, pcb_t *pcb, task_info_t *task_info, uint64_t argv1, uint64_t argv2)
{
    pid_t proc_id = get_new_pcb();
    proc_ptn[pid] = get_page_table();
    if(proc_ptn[pid] == -1){
        return -1;
    }
    if(init_user_tlb(pid, task_info) == -1){
        return -1;
    }

    pcb[proc_id].pid = pid;
    pcb[proc_id].kernel_sp = new_kernel_stack();
    pcb[proc_id].user_sp = new_user_stack();
    pcb[proc_id].entry_point = task_info->entry_point;

    pcb[proc_id].type = task_info->type;
    if(task_info->type == KERNEL_PROCESS || task_info->type == KERNEL_THREAD)
        pcb[proc_id].kernel_state = TRUE;
    else
        pcb[proc_id].kernel_state = FALSE;
    pcb[proc_id].status = TASK_READY;
    pcb[proc_id].inqueue = READY;
    strcpy(pcb[proc_id].name, task_info->name);
    init_kernel_stack(proc_id, pcb, argv1, argv2);

    pcb[proc_id].priority = task_info->priority;
    pcb[proc_id].runtime_priority = task_info->priority;
    pcb[proc_id].timeslice = priority_timeslice[task_info->priority];
    pcb[proc_id].runtimes = 0;
    pcb[proc_id].prev = NULL;
    pcb[proc_id].next = NULL;
    pcb[proc_id].block_task_count = 0;
    pcb[proc_id].hold_lock_count = 0;
    queue_push(&ready_queue[pcb[proc_id].priority], &pcb[proc_id]);
    return 1;
}

/* ready queue to run */
queue_t ready_queue[NUM_MAX_PRIORITY];

/* block queue to wait */
queue_t block_queue;

/* block queue to wait IO*/
queue_t io_block_queue;

static void check_sleeping(pcb_t *now)
{
    uint32_t begin_time = get_timer();
    if(now->status != TASK_BLOCKED)
        return;
    
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
    //vt100_move_cursor(1, 15);
    //printk("[TEST] refill ready queue:  %d    times", refill_num);

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
    do_scheduler2();
}

void do_exit(void)
{
    current_running->status = TASK_EXITED;
    current_running->inqueue = 0;
    free_kernel_stack(current_running->kernel_sp);
    free_user_stack(current_running->user_sp);
    do_unblock_all(current_running->block_task, &(current_running->block_task_count));
    while(current_running->hold_lock_count){
        do_mutex_lock_release(current_running->hold_lock[current_running->hold_lock_count]);
    }
    release_page_table(current_running->pid);
    current_running->pid = -1;
    do_scheduler2();
}

void do_block(pid_t *block_task_queue, uint32_t *block_task_count)
{
    current_running->status = TASK_BLOCKED;
    current_running->inqueue = BLOCK;
    block_task_queue[++(*block_task_count)] = current_running->pid;
    do_scheduler();
}

void block_self(void)
{
    current_running->status = TASK_BLOCKED;
    current_running->inqueue = BLOCK;
    do_scheduler2();
}

void do_unblock_one(pcb_t *unblock_one)
{
    if(unblock_one->status == TASK_BLOCKED){
        unblock_one->status = TASK_READY;
        unblock_one->inqueue = READY;
        queue_push(&ready_queue[unblock_one->runtime_priority], unblock_one);
    }
}

void do_unblock_all(pid_t *block_task_queue, uint32_t *block_task_count)
{
    pcb_t *unblock_one;
    int i;
    for(i=1; i<=(*block_task_count); i++){
        pid_t unblock_one_pid = get_pcb(block_task_queue[i]);
        if(unblock_one_pid > 0){
            unblock_one = &pcb[unblock_one_pid];
            do_unblock_one(unblock_one);
        }
    }
    (*block_task_count) = 0;
}

pid_t do_spawn(int task_id, uint64_t argv1, uint64_t argv2)
{
    if(task_id < 0 || task_id+2 >= num_user_tasks)
        return -1;
    
    pid_t pid = process_id++;
    int succ = set_pcb(pid, pcb, user_tasks[task_id+2], argv1, argv2);
    if(succ == -1)
        return 0;
    else
        return pid;
}

int do_kill(pid_t pid)
{
    int i, find;
    pcb_t *one;
    pid_t proc_id = get_pcb(pid);

    if(pid <= 2){
        return 0;
    }

    if(proc_id == -1 || pcb[proc_id].status == TASK_EXITED){
        return -1;
    }

    if(pcb[proc_id].status = TASK_READY){
        i = NUM_MAX_PRIORITY - 1;
        find = 0;
        for(; i>=0 ; i--){
            if(!queue_is_empty(&ready_queue[i])){
                one = (pcb_t*)(ready_queue[i].head);
                while(one != NULL){
                    if(one == &pcb[proc_id]){
                        find = 1;
                        goto FIND_PROC;
                    }
                    one = (pcb_t*)(one->next);
                }
            }
        }
        FIND_PROC:
        if(find == 1){
            queue_remove(&ready_queue[i], &pcb[proc_id]);
        }
    }
    else if(pcb[proc_id].status = TASK_BLOCKED){
        find = 0;
        if(!queue_is_empty(&block_queue)){
            one = (pcb_t*)(block_queue.head);
            while(one != NULL){
                if(one == &pcb[proc_id]){
                    find = 1;
                    break;
                }
                one = (pcb_t*)(one->next);
            }
        }
        if(find == 1){
            queue_remove(&block_queue, &pcb[proc_id]);
        }
    }

    pcb[proc_id].status = TASK_EXITED;
    pcb[proc_id].inqueue = 0;
    pcb[proc_id].pid = -1;
    free_kernel_stack(pcb[proc_id].kernel_sp);
    free_user_stack(pcb[proc_id].user_sp);
    do_unblock_all(pcb[proc_id].block_task, &(pcb[proc_id].block_task_count));
    i = pcb[proc_id].hold_lock_count;
    while(i){
        lock_id_t lock_id = pcb[proc_id].hold_lock[i];
        do_unblock_all(lock_queue[lock_id].block_task, &(lock_queue[lock_id].block_task_count));
        lock_queue[lock_id].status = UNLOCKED;
        lock_queue[lock_id].holder = 0;
        i--;
    }
    pcb[proc_id].hold_lock_count = 0;
    release_page_table(pid);
    return pid;
}

int do_waitpid(pid_t pid)
{
    pid_t proc_id = get_pcb(pid);
    if(proc_id == -1 || pcb[proc_id].status == TASK_EXITED){
        return -1;
    }
    current_running->status = TASK_BLOCKED;
    current_running->inqueue = BLOCK;
    pcb[proc_id].block_task[++(pcb[proc_id].block_task_count)] = current_running->pid;
    do_scheduler();
    return 0;
}

// process show
int do_process_show(int *ps_base)
{
    int i, num=0;
    for(i=1; i<NUM_MAX_TASK; i++){
        if(pcb[i].status != TASK_EXITED){
            *ps_base = pcb[i].pid;
            ps_base++;
            *ps_base = pcb[i].status;
            ps_base++;
            *ps_base = pcb[i].priority;
            ps_base++;
            num++;
        }
    }
    return num;
}

pid_t do_getpid(void)
{
    return current_running->pid;
}

void do_getpriority(priority_info_t *prio)
{
    prio->priority = current_running->priority;
    prio->runtime_priority = current_running->runtime_priority;
    prio->runtimes = current_running->runtimes;
}
