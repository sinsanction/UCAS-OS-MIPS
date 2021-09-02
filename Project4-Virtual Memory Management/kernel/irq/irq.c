#include "irq.h"
#include "time.h"
#include "sched.h"
#include "string.h"

/* exception handler */
uint64_t exception_handler[32];

/* used to init PCB */
uint32_t initial_cp0_status = 0x8003;
uint32_t initial_cp0_cause = 0;

// extern void do_shell();

static void irq_timer()
{
    screen_reflush();
    /* increase global time counter */
    time_elapsed += TIMER_INTERVAL*2;
    /* reset timer register */
    set_cp0_count(0);
    set_cp0_compare(TIMER_INTERVAL);
    /* sched.c to do scheduler */
    scheduler();
}

void interrupt_helper(uint32_t status, uint32_t cause, uint64_t epc)
{
    int IP = (cause & 0xff00) >> 8;
    if( (IP & 0x80) ){
        irq_timer();
    }
    else{
        printk("[other interrupt] EPC: 0x%x  IP: 0x%x.\n", epc, IP);
        while(1);
    }
}

void other_exception_handler(uint32_t cause, uint64_t epc, uint64_t badvaddr)
{
    int exccode = (cause & 0xff) >> 2;
    if(badvaddr == 0x123456){
        printk("[ERROR] call kernel function in user state.\n\r");
        printk("EPC: 0x%x  ExcCode: %d.\n", epc, exccode);
        while(1);
    }
    else{
        printk("[other exception] EPC: 0x%x Badvaddr: 0x%x ExcCode: %d.\n", epc, badvaddr, exccode);
        while(1);
    }
}