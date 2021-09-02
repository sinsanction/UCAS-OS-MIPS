#include "irq.h"
#include "time.h"
#include "sched.h"
#include "string.h"

/* exception handler */
uint64_t exception_handler[32];

/* used to init PCB */
uint32_t initial_cp0_status;

// extern void do_shell();

static void irq_timer()
{
    screen_reflush();

    /* increase global time counter */

    /* reset timer register */

    /* sched.c to do scheduler */
}

void interrupt_helper(uint32_t status, uint32_t cause)
{
     
}

void other_exception_handler()
{
    
}