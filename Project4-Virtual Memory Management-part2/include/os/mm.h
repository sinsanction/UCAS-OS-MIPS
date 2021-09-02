#ifndef INCLUDE_MM_H_
#define INCLUDE_MM_H_
#include "type.h"
#include "sched.h"

#define NUM_PF 14
#define USER_PROC_BASE 0xffffffffa0820000
#define SECTOR_SIZE 0x200
#define PAGE_SIZE 0x2000
#define NUM_PAGE_TABLE 8
#define NUM_PAGE_TABLE_ENTRY 0x40000

typedef struct ENTRYLO
{
    int G : 1;
    int V : 1;
    int D : 1;
    int C : 3;
    int PFN : 24;
    int PFNX : 2;

} ENTRYLO_t;

typedef struct PTE
{
    int VPN2;
    ENTRYLO_t entrylo[2];
    int disk_addr;
    char ASID;
    char setup;
    char inmemory;

} PTE_t;

typedef struct PF
{
    int setup;
    int swap_times;
    int ins;
    int VPN2;
    int ASID;
    int PFN[2];

} PF_t;

typedef struct swap_type
{
    uint64_t swap_addr;
    int swap_disk_addr;
    int swap_rw;
    int swap_asid;
    int swap_i;
    int swap_index;
    int swap_valid;
    int swap_inwork;

} swap_t;

extern PF_t physical_frame[NUM_PF];
extern PTE_t *page_table[8];
extern int proc_ptn[NUM_MAX_TASK];

extern swap_t swap_reg;
extern int count;

void init_page_table(void);
void release_page_table(pid_t);
void get_swap_reg(swap_t *);
void get_pf_num(int *);
int get_page_table(void);
int init_user_tlb(pid_t, task_info_t *);
void swap_update_pf(void);
void do_TLB_Refill(uint64_t, uint64_t);
void do_page_fault(int, int);
void do_tlb_modify(int, int);
void init_TLB(void);
void physical_frame_initial(void);
void tlb_exception_handler(uint32_t, uint64_t, uint64_t);
void share_memory_initial(void);
uint64_t do_shmget(uint64_t);
uint64_t do_shmat(uint64_t);
uint64_t do_shmdt(uint64_t);

#endif
