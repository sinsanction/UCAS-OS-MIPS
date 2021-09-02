#include "mm.h"
#include "irq.h"
#include "sched.h"

#define NUM_TLB 64
#define NUM_SHARE_PF 32
#define SHARE_PADDR_BASE 0x21000000
#define SHARE_VADDR_BASE 0x40000000

PF_t share_pf[NUM_SHARE_PF];

void share_memory_initial(void)
{
    int i;
    for(i=0; i<NUM_SHARE_PF; i++){
        share_pf[i].setup = 0;
        share_pf[i].VPN2 = ((SHARE_VADDR_BASE + i * PAGE_SIZE) >> 13);
        share_pf[i].PFN[0] = (SHARE_PADDR_BASE>>12) + i * 2;
        share_pf[i].PFN[1] = (SHARE_PADDR_BASE>>12) + i * 2 + 1;
    }
}

uint64_t do_shmget(uint64_t key)
{
    int i = key * 2 + 1;
    if(i >= NUM_SHARE_PF)
        return -1;
    else{
        share_pf[i].setup++;
        return i;
    }
}

uint64_t do_shmat(uint64_t shmid)
{
    if(shmid >= NUM_SHARE_PF)
        return -1;
    
    int i = share_pf[shmid].VPN2;
    uint64_t entryhi, entrylo0, entrylo1;
    int ptid = proc_ptn[current_running->pid];

    if(page_table[ptid][i].setup == 0){
        page_table[ptid][i].setup = 1;
        page_table[ptid][i].inmemory = 1;
        page_table[ptid][i].VPN2 = i;
        page_table[ptid][i].ASID = current_running->pid;
        page_table[ptid][i].entrylo[0].PFN = share_pf[shmid].PFN[0];
        page_table[ptid][i].entrylo[1].PFN = share_pf[shmid].PFN[1];
        page_table[ptid][i].entrylo[0].C = 2;
        page_table[ptid][i].entrylo[1].C = 2;
        page_table[ptid][i].entrylo[0].D = 1;
        page_table[ptid][i].entrylo[1].D = 1;
        page_table[ptid][i].entrylo[0].V = 1;
        page_table[ptid][i].entrylo[1].V = 1;
        page_table[ptid][i].entrylo[0].G = 0;
        page_table[ptid][i].entrylo[1].G = 0;

        entryhi = ((uint64_t)(page_table[ptid][i].VPN2) << 13) | ((uint64_t)(page_table[ptid][i].ASID) & 0xff);
        entrylo0 = *(int*)&(page_table[ptid][i].entrylo[0]);
        entrylo1 = *(int*)&(page_table[ptid][i].entrylo[1]);
        if(swap_reg.swap_index == count && swap_reg.swap_valid == 1)
            count = (count + 1) % NUM_TLB;
        set_a_tlb_entry(entryhi, entrylo0, entrylo1, count);
        count = (count + 1) % NUM_TLB;
        return (((uint64_t)share_pf[shmid].VPN2)<<13);
    }
    else{
        return -1;
    }
}

uint64_t do_shmdt(uint64_t addr)
{
    int i;
    int vpn2 = (addr >> 13);
    uint64_t entryhi, index;
    int ptid = proc_ptn[current_running->pid];

    for(i=0; i<NUM_SHARE_PF; i++){
        if(share_pf[i].VPN2 == vpn2){
            share_pf[i].setup--;
            page_table[ptid][vpn2].setup = 0;
            page_table[ptid][vpn2].inmemory = 0;
            entryhi = ((uint64_t)(page_table[ptid][vpn2].VPN2) << 13) | ((uint64_t)(page_table[ptid][vpn2].ASID) & 0xff);
            index = find_index(entryhi);
            set_a_tlb_entry(0, 0, 0, index);
            return 0;
        }
    }
    return -1;
}
