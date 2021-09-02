/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *         The kernel's entry, where most of the initialization work is done.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
 * persons to whom the Software is furnished to do so, subject to the following conditions:
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

#include "fs.h"
#include "irq.h"
#include "test.h"
#include "time.h"
#include "lock.h"
#include "stdio.h"
#include "sched.h"
#include "screen.h"
#include "common.h"
#include "syscall.h"
#include "smp.h"
#include "mm.h"
#include "mac.h"

#define TASK_INIT (00)
static void init_memory()
{
}

static void load_task(int task_num, task_info_t *tasks[])
{
    int i = 0;
    while( i<task_num ){
        set_pcb(process_id++, pcb, tasks[i++], 0);
    }
}

static void init_pcb0(void)
{
    pcb[0].pid = 0;
    pcb[0].kernel_state = TRUE;
    pcb[0].status = TASK_EXITED;
    core_id = get_cpu_id();
    current_running[0] = &pcb[0];
}

static void init_pcb(void)
{
    int i, j;
    pcb[0].kernel_sp = new_kernel_stack();
    pcb[0].user_sp = pcb[0].kernel_sp;
    pcb[0].pcb_sp = pcb[0].kernel_sp - 0x1000;
    for(i=0; i<CORE_NUM; i++){
        for(j=0; j<NUM_MAX_PRIORITY; j++)
            queue_init(&ready_queue[i][j]);
        kill[i] = 0;
    }
    queue_init(&block_queue);
    for(i=1; i<NUM_MAX_TASK; i++){
        pcb[i].status = TASK_EXITED;
        pcb[i].pid = -1;
    }

    load_task(num_shell_tasks, shell_tasks);
}

static void init_exception_handler(void)
{
    int i;
    exception_handler[0] = (uint64_t)handle_int;
    exception_handler[1] = (uint64_t)handle_tlb;
    exception_handler[2] = (uint64_t)handle_tlb;
    exception_handler[3] = (uint64_t)handle_tlb;
    exception_handler[4] = (uint64_t)handle_other;
    exception_handler[5] = (uint64_t)handle_other;
    exception_handler[6] = (uint64_t)handle_other;
    exception_handler[7] = (uint64_t)handle_other;
    exception_handler[8] = (uint64_t)handle_syscall;

    for(i=9; i<32; i++)
    exception_handler[i] = (uint64_t)handle_other;
}

static void init_exception(void)
{
    /* copy exception handler entry */
    char *exception_addr = (void *)(BEV0_EBASE + BEV0_OFFSET);
    memcpy(exception_addr, &exception_handler_entry, (exception_handler_end - exception_handler_begin));

    /* init exception handler */
    init_exception_handler();

    /* set COUNT & set COMPARE */
    set_cp0_count(0);
    set_cp0_compare(TIMER_INTERVAL);
}

// [2]
// extern int read_shell_buff(char *buff);

static void init_lock(void)
{
    int i;
    for(i=0; i<NUM_MAX_LOCK; i++){
        do_mutex_lock_init(i);
        do_binsem_init(i);
        do_mbox_init(i);
    }
}

static void init_syscall(void)
{
    syscall[0] = (syscall_func)do_spawn;
    syscall[1] = (syscall_func)do_exit;
    syscall[2] = (syscall_func)do_sleep;
    syscall[3] = (syscall_func)do_kill;
    syscall[4] = (syscall_func)do_waitpid;
    syscall[5] = (syscall_func)do_process_show;
    syscall[6] = (syscall_func)do_getpid;
    syscall[7] = (syscall_func)do_band;
    syscall[8] = (syscall_func)get_timer;
    syscall[9] = (syscall_func)do_scheduler;
    syscall[10] = (syscall_func)do_getpriority;

    syscall[11] = (syscall_func)do_binsemget;
    syscall[12] = (syscall_func)do_binsem_acquire;
    syscall[13] = (syscall_func)do_binsem_release;

    syscall[20] = (syscall_func)screen_write;
    //syscall[21] = 
    syscall[22] = (syscall_func)screen_move_cursor;
    syscall[23] = (syscall_func)screen_reflush;
    //syscall[24] = 
    //syscall[25] = 
    //syscall[26] = 
    syscall[27] = (syscall_func)screen_clear;
    syscall[28] = (syscall_func)screen_scroll;
 
    syscall[30] = (syscall_func)do_mutex_lock_init;
    syscall[31] = (syscall_func)do_mutex_lock_acquire;
    syscall[32] = (syscall_func)do_mutex_lock_release;

    syscall[33] = (syscall_func)do_condition_init;
    syscall[34] = (syscall_func)do_condition_wait;
    syscall[35] = (syscall_func)do_condition_signal;
    syscall[36] = (syscall_func)do_condition_broadcast;

    //syscall[37] = (syscall_func)do_semaphore_init;
    //syscall[38] = (syscall_func)do_semaphore_up;
    //syscall[39] = (syscall_func)do_semaphore_down;

    syscall[40] = (syscall_func)do_barrier_init;
    syscall[41] = (syscall_func)do_barrier_wait;

    /*syscall[42] = (syscall_func)do_wait_recv_package;
    syscall[43] = (syscall_func)do_net_recv;
    syscall[44] = (syscall_func)do_net_send;
    syscall[45] = (syscall_func)do_init_mac;*/

    syscall[46] = (syscall_func)do_mbox_open;
    syscall[47] = (syscall_func)do_mbox_close;
    syscall[48] = (syscall_func)do_mbox_send;
    syscall[49] = (syscall_func)do_mbox_recv;

    /*syscall[50] = 
    syscall[51] = 
    syscall[52] = 
    syscall[53] = 
    syscall[54] = 
    syscall[55] = 
    syscall[56] = 
    syscall[57] = 
    syscall[58] = 
    syscall[59] = 
    syscall[60] = 
    syscall[61] = */
}

/* [0] The beginning of everything >_< */
void __attribute__((section(".entry_function"))) _start(void)
{

    asm_start();

    /* init pcb[0] */
    init_pcb0();
    printk("> [INIT] PCB[0] initialization succeeded.\n");

    /* init stack space */
    init_stack();
    printk("> [INIT] Stack heap initialization succeeded.\n");

    init_memory();
    printk("> [INIT] Virtual memory initialization succeeded.\n");

    /* init system call table */
    init_syscall();
    printk("> [INIT] System call initialized successfully.\n");

    /* init interrupt */
    init_exception();
    printk("> [INIT] Interrupt processing initialization succeeded.\n");

    /* init Process Control Block */
    init_pcb();
    printk("> [INIT] PCB initialization succeeded.\n");

    /* init mutex_lock and binsem queue */
    init_lock();
    printk("> [INIT] LOCK initialization succeeded.\n");

    /* init screen */
    init_screen();
    printk("> [INIT] SCREEN initialization succeeded.\n");

    /* init filesystem */
    //read_super_block();

    /* wake up core1*/
    loongson3_boot_secondary();

    /* set cp0_status register to allow interrupt */
    // enable exception and interrupt
    // ERL = 0, EXL = 0, IE = 1
    enable_interrupt();
    //int offset = (int)((uint64_t)(&(pcb[0].kernel_state)) - (uint64_t)(&pcb[0]));
    //printk("kernel_state offset: %d", offset);

    while (1)
    {
        //do_scheduler();
    };
    return;
}
