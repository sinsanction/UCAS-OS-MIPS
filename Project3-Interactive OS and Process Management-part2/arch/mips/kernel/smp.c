#include "irq.h"

#include "stdio.h"
#include "sched.h"
#include "screen.h"
#include "common.h"
#include "lock.h"
#include "smp.h"
#include "test.h"
 
static void init_pcb_core1(void)
{
    pcb[32].pid = 0;
    pcb[32].kernel_state = 0;
    pcb[32].incore = 1;
    pcb[32].kernel_sp = new_kernel_stack();
    pcb[32].user_sp = new_kernel_stack();
    pcb[32].pcb_sp = pcb[32].kernel_sp - 0x1000;
    current_running[1] = &pcb[32];
}

void smp_bootstrap(void)
{
    asm_start_core1();
    set_cp0_count(0);
    set_cp0_compare(TIMER_INTERVAL);
    enable_interrupt();
    while(1){
        ;
    }
}

void ipi_write64(unsigned long arg1, void *arg2)
{

}

/*
 * Setup the PC, SP, and GP of a secondary processor and start it runing!
 */
void loongson3_boot_secondary(void)
{
    init_pcb_core1();

    uint64_t *Mailbox = (uint64_t *)0xffffffffbfe11120;
    Mailbox[1] = pcb[32].user_sp;
    Mailbox[2] = get_reg_gp();
    Mailbox[3] = 0;
    Mailbox[0] = (uint64_t)&smp_bootstrap;
}