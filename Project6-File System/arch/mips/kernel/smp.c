#include "irq.h"

#include "stdio.h"
#include "sched.h"
#include "screen.h"
#include "common.h"
#include "lock.h"
#include "smp.h"
#include "test.h"
 
static void init_pcb_core1()
{
    
}
  
  
void smp_bootstrap(void)
{

     
}

void ipi_write64(unsigned long arg1, void *arg2)
{
     
}

/*
 * Setup the PC, SP, and GP of a secondary processor and start it runing!
 */
void loongson3_boot_secondary()
{
}