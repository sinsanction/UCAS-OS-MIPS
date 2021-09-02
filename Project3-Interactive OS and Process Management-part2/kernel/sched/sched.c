#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"
#include "irq.h"

#define KERNEL_SP_BASE 0xffffffffa0f00000
#define USER_SP_BASE 0xffffffffa1f00000

pcb_t pcb[NUM_MAX_TASK+1];

uint32_t priority_timeslice[NUM_MAX_PRIORITY] = {0, 8, 4, 2};

pid_t kill[CORE_NUM];

/* kernel lock */
int core_lock = 0;

/* current running task PCB */
pcb_t *current_running[CORE_NUM];

/* global process id */
pid_t process_id = 1;

/* global core id */
int core_id;

/* kernel stack ^_^ */
#define NUM_KERNEL_STACK 32

static uint64_t kernel_stack[NUM_KERNEL_STACK];
static uint32_t kernel_stack_free[NUM_KERNEL_STACK];
static uint32_t kernel_stack_count = 0;

static uint64_t user_stack[NUM_KERNEL_STACK];
static uint32_t user_stack_free[NUM_KERNEL_STACK];
static uint32_t user_stack_count = 0;

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

void init_kernel_stack(pid_t pid, pcb_t *pcb, int incore)
{
    pcb[pid].pcb_sp = pcb[pid].kernel_sp - 0x1000 - sizeof(regs_context_t);
    uint64_t *reg = (uint64_t *)pcb[pid].pcb_sp;
    memset(reg, 0, sizeof(regs_context_t));
    
    reg[28] = get_reg_gp();
    reg[29] = pcb[pid].user_sp;
    reg[31] = reg[37] = pcb[pid].entry_point;
    if(incore == 0)
        reg[32] = initial_cp0_status;
    else
        reg[32] = 0x10008003;
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

pid_t get_pcb(pid_t pid)
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
void set_pcb(pid_t pid, pcb_t *pcb, task_info_t *task_info, int incore)
{
    pid_t proc_id = get_new_pcb();
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
    pcb[proc_id].incore = incore;
    strcpy(pcb[proc_id].name, task_info->name);
    init_kernel_stack(proc_id, pcb, incore);

    pcb[proc_id].priority = task_info->priority;
    pcb[proc_id].runtime_priority = task_info->priority;
    pcb[proc_id].timeslice = priority_timeslice[task_info->priority];
    pcb[proc_id].runtimes = 0;
    pcb[proc_id].prev = NULL;
    pcb[proc_id].next = NULL;
    pcb[proc_id].block_task_count = 0;
    pcb[proc_id].hold_lock_count = 0;
    queue_push(&ready_queue[incore][pcb[proc_id].priority], &pcb[proc_id]);
}

/* ready queue to run */
queue_t ready_queue[CORE_NUM][NUM_MAX_PRIORITY];

/* block queue to wait */
queue_t block_queue;

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
        if(now->incore != 2)
        queue_push(&ready_queue[now->incore][now->runtime_priority], now);
        else
        queue_push(&ready_queue[core_id][now->runtime_priority], now);
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
    while(!queue_is_empty(&ready_queue[core_id][0])){
        now = (pcb_t *)queue_dequeue(&ready_queue[core_id][0]);
        now->status = TASK_READY;
        now->runtime_priority = now->priority;
        now->timeslice = priority_timeslice[now->priority];
        queue_push(&ready_queue[core_id][now->priority], now);
    }
}

void scheduler(void)
{
    core_id = get_cpu_id();
    if(current_running[core_id]->kernel_state == FALSE)
        print_error();

    if(kill[core_id] != 0){
        kill[core_id] = 0;
        do_exit();
    }
    
    current_running[core_id]->cursor_x = screen_cursor_x[core_id];
    current_running[core_id]->cursor_y = screen_cursor_y[core_id];
    current_running[core_id]->timeslice--;
    /* when the timeslice is exhausted, reduce the priority and give a new timeslice */
    if(current_running[core_id]->timeslice == 0 && current_running[core_id]->pid != 32){
        current_running[core_id]->runtime_priority--;
        current_running[core_id]->timeslice = priority_timeslice[current_running[core_id]->runtime_priority];
    }
    if(current_running[core_id]->status == TASK_RUNNING){
        current_running[core_id]->status = TASK_READY;
        switch(current_running[core_id]->incore){
            case 0:
                queue_push(&ready_queue[0][current_running[core_id]->runtime_priority], current_running[core_id]);
            break;
            case 1:
                queue_push(&ready_queue[1][current_running[core_id]->runtime_priority], current_running[core_id]);
            break;
            default:{
                if(core_id == 0)
                    queue_push(&ready_queue[1][current_running[core_id]->runtime_priority], current_running[core_id]);
                else
                    queue_push(&ready_queue[0][current_running[core_id]->runtime_priority], current_running[core_id]);
                break;
            } 
        }
    }

    if(!queue_is_empty(&block_queue)){
        pcb_t *block_one;
        block_one = (pcb_t *)queue_dequeue(&block_queue);
        check_sleeping(block_one);
    }

    int i, flag;
    flag = 0;
    FIND_NEXT_PROC:
    /* select the next process from the highest priority queue */
    i = NUM_MAX_PRIORITY - 1;
    for(; i>0 ; i--){
        if(!queue_is_empty(&ready_queue[core_id][i])){
            current_running[core_id] = (pcb_t *)queue_dequeue(&ready_queue[core_id][i]);
            current_running[core_id]->status = TASK_RUNNING;
            current_running[core_id]->runtimes++;
            screen_cursor_x[core_id] = current_running[core_id]->cursor_x;
            screen_cursor_y[core_id] = current_running[core_id]->cursor_y;
            return;
        }
    }
    /* the processes in ready_queue[0] can not be scheduled */
    /* if the high priority queue has no schedulable processes, */
    /* then refill the entire priority queue according to the ready_queue[0] and starting priority */
    if(core_id == 0){
        refill_ready_queue();
        goto FIND_NEXT_PROC;
    }
    else{
        if(flag == 0){
            flag = 1;
            refill_ready_queue();
            goto FIND_NEXT_PROC;
        }
        else{
            current_running[core_id] = &pcb[32];
        }
    }
    return;
}

void do_sleep(uint32_t sleep_time)
{
    current_running[core_id]->status = TASK_BLOCKED;
    current_running[core_id]->inqueue = BLOCK;
    current_running[core_id]->begin_time = get_timer();
    current_running[core_id]->sleep_time = sleep_time;
    queue_push(&block_queue, current_running[core_id]);
    do_scheduler();
}

void do_exit(void)
{
    current_running[core_id]->status = TASK_EXITED;
    current_running[core_id]->inqueue = 0;
    current_running[core_id]->pid = -1;
    free_kernel_stack(current_running[core_id]->kernel_sp);
    free_user_stack(current_running[core_id]->user_sp);
    do_unblock_all(current_running[core_id]->block_task, &(current_running[core_id]->block_task_count));
    while(current_running[core_id]->hold_lock_count){
        do_mutex_lock_release(current_running[core_id]->hold_lock[current_running[core_id]->hold_lock_count]);
    }
    do_scheduler();
}

void do_block(pid_t *block_task_queue, uint32_t *block_task_count)
{
    current_running[core_id]->status = TASK_BLOCKED;
    current_running[core_id]->inqueue = BLOCK;
    block_task_queue[++(*block_task_count)] = current_running[core_id]->pid;
    do_scheduler();
}

void do_unblock_one(pcb_t *unblock_one)
{
    if(unblock_one->status == TASK_BLOCKED){
        unblock_one->status = TASK_READY;
        unblock_one->inqueue = READY;
        if(unblock_one->incore != 2)
        queue_push(&ready_queue[unblock_one->incore][unblock_one->runtime_priority], unblock_one);
        else
        queue_push(&ready_queue[core_id][unblock_one->runtime_priority], unblock_one);
    }
}

void do_unblock_all(pid_t *block_task_queue, uint32_t *block_task_count)
{
    pcb_t *unblock_one;
    while(*block_task_count){
        pid_t unblock_one_pid = get_pcb(block_task_queue[*block_task_count]);
        if(unblock_one_pid > 0){
            unblock_one = &pcb[unblock_one_pid];
            do_unblock_one(unblock_one);
        }
        (*block_task_count)--;
    }
}

pid_t do_spawn(task_info_t *task, int incore)
{
    pid_t pid = process_id++;
    if(incore == 2)
        set_pcb(pid, pcb, task, pid%2);
    else
        set_pcb(pid, pcb, task, incore);
    return pid;
}

int do_kill(pid_t pid)
{
    int i, j, find;
    pcb_t *one;
    pid_t proc_id = get_pcb(pid);

    if(pid <= 1){
        return 0;
    }
    if(proc_id == -1 || pcb[proc_id].status == TASK_EXITED){
        return -1;
    }
    if(core_id == 0 && current_running[1] == &pcb[proc_id]){
        kill[1] = pid;
        return pid;
    }
    if(core_id == 1 && current_running[0] == &pcb[proc_id]){
        kill[0] = pid;
        return pid;
    }

    if(pcb[proc_id].status = TASK_READY){
        find = 0;
        for(i=0; i<CORE_NUM; i++){
            for(j=0; j<NUM_MAX_PRIORITY; j++){
                if(!queue_is_empty(&ready_queue[i][j])){
                    one = (pcb_t*)(ready_queue[i][j].head);
                    while(one != NULL){
                        if(one == &pcb[proc_id]){
                            find = 1;
                            goto FIND_PROC;
                        }
                        one = (pcb_t*)(one->next);
                    }
                }
            }
        }
        FIND_PROC:
        if(find == 1){
            queue_remove(&ready_queue[i][j], &pcb[proc_id]);
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
    return pid;
}

int do_waitpid(pid_t pid)
{
    pid_t proc_id = get_pcb(pid);
    if(proc_id == -1 || pcb[proc_id].status == TASK_EXITED){
        return -1;
    }
    current_running[core_id]->status = TASK_BLOCKED;
    current_running[core_id]->inqueue = BLOCK;
    pcb[proc_id].block_task[++(pcb[proc_id].block_task_count)] = current_running[core_id]->pid;
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
            *ps_base = pcb[i].incore;
            ps_base++;
            num++;
        }
    }
    return num;
}

pid_t do_getpid(void)
{
    return current_running[core_id]->pid;
}

int do_band(pid_t pid, int core)
{
    pid_t proc_id = get_pcb(pid);

    if(pid <= 1){
        return 0;
    }
    if(proc_id == -1 || pcb[proc_id].status == TASK_EXITED){
        return -1;
    }
    pcb[proc_id].incore = core;
    return pid;
}

void do_getpriority(priority_info_t *prio)
{
    prio->priority = current_running[core_id]->priority;
    prio->runtime_priority = current_running[core_id]->runtime_priority;
    prio->runtimes = current_running[core_id]->runtimes;
}
