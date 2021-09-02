#include "mm.h"
#include "irq.h"
#include "sched.h"

#define NUM_TLB 64
#define NUM_PF 16
#define NUM_DISK_PF 32
#define NUM_PAGE_TABLE 0x40000
#define USER_PADDR_BASE 0x20000000
#define USER_VADDR_BASE 0x0
#define PAGE_TABLE_BASE 0xffffffffa1f00000
#define DISK_ADDR_BASE 0x2000
#define PROC_SWAP 2

PTE_t *page_table = (PTE_t*)PAGE_TABLE_BASE;
int page_out;

PF_t physical_frame[NUM_PF];
int disk_pf[NUM_DISK_PF];
int disk_pf_free[NUM_DISK_PF];

uint64_t swap_addr;
int swap_disk_addr;
int swap_rw;
int swap_i;
int swap_index;
int swap_valid;
int swap_inwork;

int new_disk_pf_addr(void)
{
    int i;
    for(i=0; i<NUM_DISK_PF; i++){
        if(disk_pf_free[i]){
            disk_pf_free[i] = 0;
            return disk_pf[i];
        }
    }
}

void free_disk_pf(int disk_addr)
{
    int i = (disk_addr - DISK_ADDR_BASE)/0x2000;
    disk_pf_free[i] = 1;
}

int get_free_pf(void)
{
    int i;
    for(i=0; i<NUM_PF; i++){
        if(physical_frame[i].setup == 0){
            return i;
        }
    }
    return -1;
}

void physical_frame_initial(void)
{
    int i;
    for(i=0; i<NUM_DISK_PF; i++){
        disk_pf[i] = DISK_ADDR_BASE + i * 0x2000;
        disk_pf_free[i] = 1;
    }
    for(i=0; i<NUM_PF; i++){
        physical_frame[i].setup = 0;
        physical_frame[i].swap_times = 0;
        physical_frame[i].PFN[0] = (USER_PADDR_BASE>>12) + i * 2;
        physical_frame[i].PFN[1] = (USER_PADDR_BASE>>12) + i * 2 + 1;
    }
    page_out = 0;
    swap_valid = 0;
    swap_inwork = 0;
}

void init_page_table(void)
{
    int i;
    for(i=0; i<NUM_PAGE_TABLE; i++){
        page_table[i].setup = 0;
    }
}

void release_page_table(pid_t pid)
{
    int i;
    for(i=0; i<NUM_PAGE_TABLE; i++){
        if(page_table[i].ASID == pid){
            page_table[i].setup = 0;
            page_table[i].inmemory = 0;
        }
    }
    for(i=0; i<NUM_PF; i++){
        if(physical_frame[i].ASID == pid){
            physical_frame[i].setup = 0;
        }
    }
}

void protection_fault(int asid, int other_asid)
{
    printk("protection fault: process pid=%d access page of pid=%d.\n\r", asid, other_asid);
    do_exit();
}

void find_page_out(int asid)
{
    int i = page_out;
    int j = 0;
    while(j < NUM_PF){
        if(physical_frame[i].ASID == asid){
            page_out = i;
            return;
        }
        i = (i + 1) % NUM_PF;
        j = j + 1;
    }
    page_out = 4;
}

void swap_update_pf(void)
{
    uint64_t entryhi, entrylo0, entrylo1;

    swap_valid = 0;
    swap_inwork = 0;
    if(swap_rw){
        physical_frame[page_out].setup = 0;
        physical_frame[page_out].swap_times++;
        page_out = (page_out + 1) % NUM_PF;
        set_a_tlb_entry(0, 0, 0, swap_index);
    }
    else{
        entryhi = ((uint64_t)(page_table[swap_i].VPN2) << 13) | ((uint64_t)(page_table[swap_i].ASID) & 0xff);
        entrylo0 = ((uint64_t)(page_table[swap_i].PFN[0]) << 6) | ((uint64_t)(page_table[swap_i].Coherency[0]) << 3) | 
                ((uint64_t)(page_table[swap_i].Dirty[0]) << 2) | ((uint64_t)(page_table[swap_i].Valid[0]) << 1) | ((uint64_t)(page_table[swap_i].Global[0]));
        entrylo1 = ((uint64_t)(page_table[swap_i].PFN[1]) << 6) | ((uint64_t)(page_table[swap_i].Coherency[1]) << 3) | 
                ((uint64_t)(page_table[swap_i].Dirty[1]) << 2) | ((uint64_t)(page_table[swap_i].Valid[1]) << 1) | ((uint64_t)(page_table[swap_i].Global[1]));
        set_a_tlb_entry(entryhi, entrylo0, entrylo1, swap_index);
    }

    do_unblock_all(pcb[PROC_SWAP].block_task, &(pcb[PROC_SWAP].block_task_count));
    pcb[PROC_SWAP].status = TASK_BLOCKED;
    pcb[PROC_SWAP].inqueue = BLOCK;
    do_scheduler2();
}

void swap(int asid)
{
    find_page_out(asid);
    int i = physical_frame[page_out].VPN2 - (USER_VADDR_BASE>>13);
    uint64_t entryhi, entrylo0, entrylo1, index = -1;
    int disk_base;
    uint64_t mem_addr;

    current_running->status = TASK_BLOCKED;
    current_running->inqueue = BLOCK;
    pcb[PROC_SWAP].block_task[++(pcb[PROC_SWAP].block_task_count)] = current_running->pid;

    if(swap_inwork){
        do_scheduler2();
        return;
    }

    entryhi = ((uint64_t)(page_table[i].VPN2) << 13) | ((uint64_t)(page_table[i].ASID) & 0xff);
    index = find_index(entryhi);
    set_a_tlb_entry(entryhi, 0, 0, index);

    entryhi = ((uint64_t)(page_table[i].VPN2) << 13) | ((uint64_t)(PROC_SWAP) & 0xff);
    entrylo0 = ((uint64_t)(page_table[i].PFN[0]) << 6) | ((uint64_t)(page_table[i].Coherency[0]) << 3) | 
            ((uint64_t)(page_table[i].Dirty[0]) << 2) | ((uint64_t)(page_table[i].Valid[0]) << 1) | ((uint64_t)(page_table[i].Global[0]));
    entrylo1 = ((uint64_t)(page_table[i].PFN[1]) << 6) | ((uint64_t)(page_table[i].Coherency[1]) << 3) | 
            ((uint64_t)(page_table[i].Dirty[1]) << 2) | ((uint64_t)(page_table[i].Valid[1]) << 1) | ((uint64_t)(page_table[i].Global[1]));
    set_a_tlb_entry(entryhi, entrylo0, entrylo1, ++index);

    page_table[i].inmemory = 0;
    page_table[i].Valid[0] = 0;
    page_table[i].Valid[1] = 0;

    swap_addr = (uint64_t)(page_table[i].VPN2<<13);
    swap_disk_addr = new_disk_pf_addr();
    page_table[i].disk_addr = swap_disk_addr;
    swap_rw = 1;
    swap_index = index;
    swap_valid = 1;
    swap_inwork = 1;
    do_unblock_one(&pcb[PROC_SWAP]);
    do_scheduler2();
}

int count = 0;
void do_TLB_Refill(uint64_t entryhi0, uint64_t context)
{
    int asid = (entryhi0 & 0xff);
    int badVPN2 = (context & 0x7fffff) >> 4;
    int i = badVPN2 - (USER_VADDR_BASE>>13);
    uint64_t entryhi, entrylo0, entrylo1;

    if(page_table[i].setup == 1){
        if(asid == page_table[i].ASID){
            if(page_table[i].inmemory == 1){
                entryhi = ((uint64_t)(page_table[i].VPN2) << 13) | ((uint64_t)(page_table[i].ASID) & 0xff);
                entrylo0 = ((uint64_t)(page_table[i].PFN[0]) << 6) | ((uint64_t)(page_table[i].Coherency[0]) << 3) | 
                    ((uint64_t)(page_table[i].Dirty[0]) << 2) | ((uint64_t)(page_table[i].Valid[0]) << 1) | ((uint64_t)(page_table[i].Global[0]));
                entrylo1 = ((uint64_t)(page_table[i].PFN[1]) << 6) | ((uint64_t)(page_table[i].Coherency[1]) << 3) | 
                    ((uint64_t)(page_table[i].Dirty[1]) << 2) | ((uint64_t)(page_table[i].Valid[1]) << 1) | ((uint64_t)(page_table[i].Global[1]));
                if(swap_index == count && swap_valid == 1)
                    count = (count + 1) % NUM_TLB;
                set_a_tlb_entry(entryhi, entrylo0, entrylo1, count);
            }
            else{
                page_table[i].Valid[0] = 0;
                page_table[i].Valid[1] = 0;
                entryhi = ((uint64_t)(page_table[i].VPN2) << 13) | ((uint64_t)(page_table[i].ASID) & 0xff);
                if(swap_index == count && swap_valid == 1)
                    count = (count + 1) % NUM_TLB;
                set_a_tlb_entry(entryhi, 0, 0, count);
            }
        }
        else{
            protection_fault(asid, page_table[i].ASID);
        }
    }
    else{
        page_table[i].VPN2 = badVPN2;
        page_table[i].ASID = asid;
        page_table[i].Valid[0] = 0;
        page_table[i].Valid[1] = 0;
        page_table[i].Global[0] = 0;
        page_table[i].Global[1] = 0;
        entryhi = ((uint64_t)(page_table[i].VPN2) << 13) | ((uint64_t)(page_table[i].ASID) & 0xff);
        if(swap_index == count && swap_valid == 1)
            count = (count + 1) % NUM_TLB;
        set_a_tlb_entry(entryhi, 0, 0, count);
    }
    count = (count + 1) % NUM_TLB;
}

void do_page_fault(int badVPN2, int asid)
{
    int i = badVPN2 - (USER_VADDR_BASE>>13);
    uint64_t entryhi, entrylo0, entrylo1, index;
    int pf_num;

    if(page_table[i].setup == 0){
        pf_num = get_free_pf();
        if(pf_num >= 0){
            page_table[i].setup = 1;
            page_table[i].inmemory = 1;
            physical_frame[pf_num].setup = 1;
            physical_frame[pf_num].VPN2 = badVPN2;
            physical_frame[pf_num].ASID = asid;

            page_table[i].PFN[0] = physical_frame[pf_num].PFN[0];
            page_table[i].PFN[1] = physical_frame[pf_num].PFN[1];
            page_table[i].Coherency[0] = 2;
            page_table[i].Coherency[1] = 2;
            page_table[i].Dirty[0] = 1;
            page_table[i].Dirty[1] = 1;
            page_table[i].Valid[0] = 1;
            page_table[i].Valid[1] = 1;
            page_table[i].Global[0] = 0;
            page_table[i].Global[1] = 0;

            entryhi = ((uint64_t)(page_table[i].VPN2) << 13) | ((uint64_t)(page_table[i].ASID) & 0xff);
            entrylo0 = ((uint64_t)(page_table[i].PFN[0]) << 6) | ((uint64_t)(page_table[i].Coherency[0]) << 3) | 
                ((uint64_t)(page_table[i].Dirty[0]) << 2) | ((uint64_t)(page_table[i].Valid[0]) << 1) | ((uint64_t)(page_table[i].Global[0]));
            entrylo1 = ((uint64_t)(page_table[i].PFN[1]) << 6) | ((uint64_t)(page_table[i].Coherency[1]) << 3) | 
                ((uint64_t)(page_table[i].Dirty[1]) << 2) | ((uint64_t)(page_table[i].Valid[1]) << 1) | ((uint64_t)(page_table[i].Global[1]));
            index = find_index(entryhi);
            set_a_tlb_entry(entryhi, entrylo0, entrylo1, index);
        }
        else{
            swap(asid);
        }
    }
    else if(page_table[i].inmemory == 0){
        pf_num = get_free_pf();
        if(pf_num >= 0){
            current_running->status = TASK_BLOCKED;
            current_running->inqueue = BLOCK;
            pcb[PROC_SWAP].block_task[++(pcb[PROC_SWAP].block_task_count)] = current_running->pid;
            if(swap_inwork){
                do_scheduler2();
                return;
            }

            page_table[i].inmemory = 1;
            physical_frame[pf_num].setup = 1;
            physical_frame[pf_num].VPN2 = badVPN2;
            physical_frame[pf_num].ASID = asid;

            page_table[i].PFN[0] = physical_frame[pf_num].PFN[0];
            page_table[i].PFN[1] = physical_frame[pf_num].PFN[1];
            page_table[i].Coherency[0] = 2;
            page_table[i].Coherency[1] = 2;
            page_table[i].Dirty[0] = 1;
            page_table[i].Dirty[1] = 1;
            page_table[i].Valid[0] = 1;
            page_table[i].Valid[1] = 1;
            page_table[i].Global[0] = 0;
            page_table[i].Global[1] = 0;

            entryhi = ((uint64_t)(page_table[i].VPN2) << 13) | ((uint64_t)(page_table[i].ASID) & 0xff);
            index = find_index(entryhi);
            entryhi = ((uint64_t)(page_table[i].VPN2) << 13) | ((uint64_t)(PROC_SWAP) & 0xff);
            entrylo0 = ((uint64_t)(page_table[i].PFN[0]) << 6) | ((uint64_t)(page_table[i].Coherency[0]) << 3) | 
                ((uint64_t)(page_table[i].Dirty[0]) << 2) | ((uint64_t)(page_table[i].Valid[0]) << 1) | ((uint64_t)(page_table[i].Global[0]));
            entrylo1 = ((uint64_t)(page_table[i].PFN[1]) << 6) | ((uint64_t)(page_table[i].Coherency[1]) << 3) | 
                ((uint64_t)(page_table[i].Dirty[1]) << 2) | ((uint64_t)(page_table[i].Valid[1]) << 1) | ((uint64_t)(page_table[i].Global[1]));
            set_a_tlb_entry(entryhi, entrylo0, entrylo1, index);

            free_disk_pf(page_table[i].disk_addr);
            swap_addr = (uint64_t)(page_table[i].VPN2<<13);
            swap_disk_addr = page_table[i].disk_addr;
            swap_rw = 0;
            swap_i = i;
            swap_index = index;
            swap_valid = 1;
            swap_inwork = 1;
            do_unblock_one(&pcb[PROC_SWAP]);
            do_scheduler2();
        }
        else{
            swap(asid);
        }
    }
}

void do_tlb_modify(int badVPN2, int asid)
{
    int i = badVPN2 - (USER_VADDR_BASE>>13);
    uint64_t entryhi, entrylo0, entrylo1, index;

    page_table[i].Dirty[0] = 1;
    page_table[i].Dirty[1] = 1;

    entryhi = ((uint64_t)(page_table[i].VPN2) << 13) | ((uint64_t)(page_table[i].ASID) & 0xff);
    entrylo0 = ((uint64_t)(page_table[i].PFN[0]) << 6) | ((uint64_t)(page_table[i].Coherency[0]) << 3) | 
            ((uint64_t)(page_table[i].Dirty[0]) << 2) | ((uint64_t)(page_table[i].Valid[0]) << 1) | ((uint64_t)(page_table[i].Global[0]));
    entrylo1 = ((uint64_t)(page_table[i].PFN[1]) << 6) | ((uint64_t)(page_table[i].Coherency[1]) << 3) | 
            ((uint64_t)(page_table[i].Dirty[1]) << 2) | ((uint64_t)(page_table[i].Valid[1]) << 1) | ((uint64_t)(page_table[i].Global[1]));
    index = find_index(entryhi);
    set_a_tlb_entry(entryhi, entrylo0, entrylo1, index);
    printk("error: tlb_modify badVPN2: %d", badVPN2);
}

void init_TLB(void)
{
    char *refill_exception_addr = (void *)(BEV0_EBASE);
    memcpy(refill_exception_addr, &TLBexception_handler_entry, (TLBexception_handler_end - TLBexception_handler_begin));
}

void tlb_exception_handler(uint32_t cause, uint64_t entryhi, uint64_t context)
{
    int asid = (entryhi & 0xff);
    int exccode = (cause & 0xff) >> 2;
    int badVPN2 = (context & 0x7fffff) >> 4;

    if(exccode == 2 || exccode == 3){
        do_page_fault(badVPN2, asid);
    }
    else if(exccode == 1){
        do_tlb_modify(badVPN2, asid);
    }
}
