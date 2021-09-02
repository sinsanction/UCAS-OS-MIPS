#ifndef INCLUDE_MM_H_
#define INCLUDE_MM_H_
#include "type.h"
#include "sched.h"

#define NUM_PF 8

typedef struct PTE
{
    int setup;
    int inmemory;
    int disk_addr;
    int VPN2;
    int ASID;

    int PFN[2];
    int Coherency[2];
    int Dirty[2];
    int Valid[2];
    int Global[2];

} PTE_t;

typedef struct PF
{
    int setup;
    int swap_times;
    int VPN2;
    int ASID;
    int PFN[2];

} PF_t;

extern PF_t physical_frame[NUM_PF];

extern uint64_t swap_addr;
extern int swap_disk_addr;
extern int swap_rw;
extern int swap_i;
extern int swap_index;
extern int swap_valid;
extern int swap_inwork;

void init_page_table(void);
void release_page_table(pid_t);
void swap_update_pf(void);
void do_TLB_Refill(uint64_t, uint64_t);
void do_page_fault(int, int);
void do_tlb_modify(int, int);
void init_TLB(void);
void physical_frame_initial(void);
void tlb_exception_handler(uint32_t, uint64_t, uint64_t);

#endif
