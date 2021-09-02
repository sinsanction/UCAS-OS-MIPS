#include "mm.h"
#include "irq.h"
#include "sched.h"

#define NUM_TLB 64
#define NUM_PF 14
#define NUM_DISK_PF 32
#define NUM_PAGE_TABLE 8
#define NUM_PAGE_TABLE_ENTRY 0x40000
#define USER_PADDR_BASE 0x20000000
#define USER_VADDR_BASE 0x0
#define PAGE_TABLE_BASE 0xffffffffa1000000
#define DISK_ADDR_BASE 0x20000
#define PROC_SWAP 2

PTE_t *page_table[8];
int page_table_free[8];
int proc_ptn[NUM_MAX_TASK];
int page_out;

PF_t physical_frame[NUM_PF];
int disk_pf[NUM_DISK_PF];
int disk_pf_free[NUM_DISK_PF];

swap_t swap_reg;

int count = 0;

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

int get_page_table(void)
{
    int i;
    for(i=0; i<NUM_PAGE_TABLE; i++){
        if(page_table_free[i] == 1){
            page_table_free[i] = 0;
            return i;
        }
    }
    return -1;
}

int init_user_tlb(pid_t pid, task_info_t *task_info)
{
    uint64_t entryhi, entrylo0, entrylo1, index;
    uint64_t entry_point, kernel_base;
    int ptid = proc_ptn[pid];
    int pf_num, i, j;

    entry_point = task_info->entry_point;
    kernel_base = USER_PROC_BASE + task_info->offset;

    for(i=0; i<task_info->page_num; i++){
        if(i == task_info->page_num-1){
            if(task_info->last_page_sector == 0)
            break;
        }
        pf_num = get_free_pf();
        if(pf_num == -1){
            release_page_table(pid);
            return -1;
        }
        j = ((entry_point + i * PAGE_SIZE) >> 13) - (USER_VADDR_BASE>>13);
        page_table[ptid][j].setup = 1;
        page_table[ptid][j].inmemory = 1;
        page_table[ptid][j].VPN2 = ((entry_point + i * PAGE_SIZE) >> 13);
        page_table[ptid][j].ASID = pid;
        physical_frame[pf_num].setup = 1;
        physical_frame[pf_num].ins = 1;
        physical_frame[pf_num].VPN2 = ((entry_point + i * PAGE_SIZE) >> 13);
        physical_frame[pf_num].ASID = pid;

        page_table[ptid][j].entrylo[0].PFN = physical_frame[pf_num].PFN[0];
        page_table[ptid][j].entrylo[1].PFN = physical_frame[pf_num].PFN[1];
        page_table[ptid][j].entrylo[0].C = 2;
        page_table[ptid][j].entrylo[1].C = 2;
        page_table[ptid][j].entrylo[0].D = 1;
        page_table[ptid][j].entrylo[1].D = 1;
        page_table[ptid][j].entrylo[0].V = 1;
        page_table[ptid][j].entrylo[1].V = 1;
        page_table[ptid][j].entrylo[0].G = 0;
        page_table[ptid][j].entrylo[1].G = 0;

        entryhi = ((uint64_t)(page_table[ptid][j].VPN2) << 13) | ((uint64_t)(page_table[ptid][j].ASID) & 0xff);
        entrylo0 = *(int*)&(page_table[ptid][j].entrylo[0]);
        entrylo1 = *(int*)&(page_table[ptid][j].entrylo[1]);
        set_a_tlb_entry(entryhi, entrylo0, entrylo1, count);
        count = (count + 1) % NUM_TLB;
        if(i == task_info->page_num-1)
            memcpy((char*)(entry_point + i * PAGE_SIZE), (char*)(kernel_base + i * PAGE_SIZE), task_info->last_page_sector * SECTOR_SIZE);
        else
            memcpy((char*)(entry_point + i * PAGE_SIZE), (char*)(kernel_base + i * PAGE_SIZE), PAGE_SIZE);
    }
    return 1;
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
    swap_reg.swap_valid = 0;
    swap_reg.swap_inwork = 0;
}

void init_page_table(void)
{
    int i, j;
    for(i=0; i<NUM_PAGE_TABLE; i++){
        page_table[i] = (PTE_t *)(PAGE_TABLE_BASE + NUM_PAGE_TABLE_ENTRY * sizeof(PTE_t) * i);
        page_table_free[i] = 1;
        for(j=0; j<NUM_PAGE_TABLE_ENTRY; j++){
            page_table[i][j].setup = 0;
        }
        printk("page_table[%d]: 0x%x\n\r", i, page_table[i]);
    }
}

void release_page_table(pid_t pid)
{
    int i;
    int ptid = proc_ptn[pid];
    for(i=0; i<NUM_PAGE_TABLE_ENTRY; i++){
        page_table[ptid][i].setup = 0;
        page_table[ptid][i].inmemory = 0;
    }
    page_table_free[ptid] = 1;
    proc_ptn[pid] = -1;
    for(i=0; i<NUM_PF; i++){
        if(physical_frame[i].ASID == pid){
            physical_frame[i].setup = 0;
            physical_frame[i].ins = 0;
        }
    }
}

void protection_fault(int asid, int other_asid, int ptid)
{
    printk("protection fault: process pid=%d access page of pid=%d in page table [%d].\n\r", asid, other_asid, ptid);
    do_exit();
}

void find_page_out(int asid)
{
    int i = page_out;
    int j = 0;
    while(j < NUM_PF){
        if(physical_frame[i].ASID == asid && physical_frame[i].ins != 1){
            page_out = i;
            return;
        }
        i = (i + 1) % NUM_PF;
        j = j + 1;
    }
    page_out = 10;
}

void get_swap_reg(swap_t *swapreg)
{
    *swapreg = swap_reg;
}

void get_pf_num(int *pfnum)
{
    int i;
    for(i=0; i<NUM_PF; i++){
        pfnum[i] = physical_frame[i].swap_times;
    }
}

void swap_update_pf(void)
{
    uint64_t entryhi, entrylo0, entrylo1;
    int ptid = proc_ptn[swap_reg.swap_asid];

    swap_reg.swap_valid = 0;
    swap_reg.swap_inwork = 0;
    if(swap_reg.swap_rw){
        physical_frame[page_out].setup = 0;
        physical_frame[page_out].swap_times++;
        page_out = (page_out + 1) % NUM_PF;
        set_a_tlb_entry(0, 0, 0, swap_reg.swap_index);
        vt100_move_cursor(1, 1);
        printk("[swap write finish]                                                                                       ");
        
    }
    else{
        entryhi = ((uint64_t)(page_table[ptid][swap_reg.swap_i].VPN2) << 13) | ((uint64_t)(page_table[ptid][swap_reg.swap_i].ASID) & 0xff);
        entrylo0 = *(int*)&(page_table[ptid][swap_reg.swap_i].entrylo[0]);
        entrylo1 = *(int*)&(page_table[ptid][swap_reg.swap_i].entrylo[1]);
        set_a_tlb_entry(entryhi, entrylo0, entrylo1, swap_reg.swap_index);
        vt100_move_cursor(1, 1);
        printk("[swap read finish]                                                                                       ");
    }

    do_unblock_all(pcb[PROC_SWAP].block_task, &(pcb[PROC_SWAP].block_task_count));
    pcb[PROC_SWAP].status = TASK_BLOCKED;
    pcb[PROC_SWAP].inqueue = BLOCK;
    do_scheduler2();
}

void swap(int asid)
{
    int i, ptid;
    uint64_t entryhi, entrylo0, entrylo1, index = -1;
    int disk_base;
    uint64_t mem_addr;

    if(asid == PROC_SWAP){
        vt100_move_cursor(1, 2);
        printk("[swap error] proc swap lost page "); 
        uint64_t badvddr= get_cp0_badvaddr();
        int bd_hi = (int)(badvddr>>32);
        int bd_lo = (int)(badvddr & 0xffffffff);
        printk("badvaddr: 0x%x %x", bd_hi, bd_lo);
    }

    current_running->status = TASK_BLOCKED;
    current_running->inqueue = BLOCK;
    //pcb[PROC_SWAP].block_task[++(pcb[PROC_SWAP].block_task_count)] = current_running->pid;
    pcb[PROC_SWAP].block_task[++(pcb[PROC_SWAP].block_task_count)] = asid;

    if(swap_reg.swap_inwork){
        do_scheduler2();
        return;
    }

    find_page_out(asid);
    i = physical_frame[page_out].VPN2 - (USER_VADDR_BASE>>13);
    ptid = proc_ptn[physical_frame[page_out].ASID];

    entryhi = ((uint64_t)(page_table[ptid][i].VPN2) << 13) | ((uint64_t)(page_table[ptid][i].ASID) & 0xff);
    index = find_index(entryhi);
    set_a_tlb_entry(entryhi, 0, 0, index);

    entryhi = ((uint64_t)(page_table[ptid][i].VPN2) << 13) | ((uint64_t)(PROC_SWAP) & 0xff);
    entrylo0 = *(int*)&(page_table[ptid][i].entrylo[0]);
    entrylo1 = *(int*)&(page_table[ptid][i].entrylo[1]);
    set_a_tlb_entry(entryhi, entrylo0, entrylo1, ++index);

    page_table[ptid][i].inmemory = 0;
    page_table[ptid][i].entrylo[0].V = 0;
    page_table[ptid][i].entrylo[1].V = 0;

    swap_reg.swap_addr = (uint64_t)(page_table[ptid][i].VPN2<<13);
    swap_reg.swap_disk_addr = new_disk_pf_addr();
    page_table[ptid][i].disk_addr = swap_reg.swap_disk_addr;
    swap_reg.swap_rw = 1;
    swap_reg.swap_asid = asid;
    swap_reg.swap_index = index;
    swap_reg.swap_valid = 1;
    swap_reg.swap_inwork = 1;

    vt100_move_cursor(1, 1);
    printk("[swap write] ptid: %d, num: %d, vpn: 0x%x, asid: %d, pt_vpn: 0x%x, pt_asid: %d, disk_addr: 0x%x", ptid, page_out, (i<<13), physical_frame[page_out].ASID, (page_table[ptid][i].VPN2 << 13), page_table[ptid][i].ASID, page_table[ptid][i].disk_addr);

    do_unblock_one(&pcb[PROC_SWAP]);
    do_scheduler2();
}

void do_TLB_Refill(uint64_t entryhi0, uint64_t context)
{
    int asid = (entryhi0 & 0xff);
    int badVPN2 = (context & 0x7fffff) >> 4;
    int i = badVPN2 - (USER_VADDR_BASE>>13);
    uint64_t entryhi, entrylo0, entrylo1;
    int ptid = proc_ptn[asid];

    if(page_table[ptid][i].setup == 1){
        if(asid == page_table[ptid][i].ASID){
            if(page_table[ptid][i].inmemory == 1){
                entryhi = ((uint64_t)(page_table[ptid][i].VPN2) << 13) | ((uint64_t)(page_table[ptid][i].ASID) & 0xff);
                entrylo0 = *(int*)&(page_table[ptid][i].entrylo[0]);
                entrylo1 = *(int*)&(page_table[ptid][i].entrylo[1]);
                if(swap_reg.swap_index == count && swap_reg.swap_valid == 1)
                    count = (count + 1) % NUM_TLB;
                set_a_tlb_entry(entryhi, entrylo0, entrylo1, count);
            }
            else{
                page_table[ptid][i].entrylo[0].V = 0;
                page_table[ptid][i].entrylo[1].V = 0;
                entryhi = ((uint64_t)(page_table[ptid][i].VPN2) << 13) | ((uint64_t)(page_table[ptid][i].ASID) & 0xff);
                if(swap_reg.swap_index == count && swap_reg.swap_valid == 1)
                    count = (count + 1) % NUM_TLB;
                set_a_tlb_entry(entryhi, 0, 0, count);
            }
        }
        else{
            protection_fault(asid, page_table[ptid][i].ASID, ptid);
        }
    }
    else{
        page_table[ptid][i].VPN2 = badVPN2;
        page_table[ptid][i].ASID = asid;
        page_table[ptid][i].entrylo[0].V = 0;
        page_table[ptid][i].entrylo[1].V = 0;
        page_table[ptid][i].entrylo[0].G = 0;
        page_table[ptid][i].entrylo[1].G = 0;
        entryhi = ((uint64_t)(page_table[ptid][i].VPN2) << 13) | ((uint64_t)(page_table[ptid][i].ASID) & 0xff);
        if(swap_reg.swap_index == count && swap_reg.swap_valid == 1)
            count = (count + 1) % NUM_TLB;
        set_a_tlb_entry(entryhi, 0, 0, count);
    }
    count = (count + 1) % NUM_TLB;
}

void do_page_fault(int badVPN2, int asid)
{
    int i = badVPN2 - (USER_VADDR_BASE>>13);
    uint64_t entryhi, entrylo0, entrylo1, index;
    int ptid = proc_ptn[asid];
    int pf_num;

    if(page_table[ptid][i].setup == 0){
        pf_num = get_free_pf();
        if(pf_num >= 0){
            page_table[ptid][i].setup = 1;
            page_table[ptid][i].inmemory = 1;
            physical_frame[pf_num].setup = 1;
            physical_frame[pf_num].VPN2 = badVPN2;
            physical_frame[pf_num].ASID = asid;

            page_table[ptid][i].entrylo[0].PFN = physical_frame[pf_num].PFN[0];
            page_table[ptid][i].entrylo[1].PFN = physical_frame[pf_num].PFN[1];
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
            index = find_index(entryhi);
            set_a_tlb_entry(entryhi, entrylo0, entrylo1, index);
        }
        else{
            swap(asid);
        }
    }
    else if(page_table[ptid][i].inmemory == 0){
        pf_num = get_free_pf();
        if(pf_num >= 0){
            current_running->status = TASK_BLOCKED;
            current_running->inqueue = BLOCK;
            pcb[PROC_SWAP].block_task[++(pcb[PROC_SWAP].block_task_count)] = current_running->pid;
            if(swap_reg.swap_inwork){
                do_scheduler2();
                return;
            }

            page_table[ptid][i].inmemory = 1;
            physical_frame[pf_num].setup = 1;
            physical_frame[pf_num].VPN2 = badVPN2;
            physical_frame[pf_num].ASID = asid;

            page_table[ptid][i].entrylo[0].PFN = physical_frame[pf_num].PFN[0];
            page_table[ptid][i].entrylo[1].PFN = physical_frame[pf_num].PFN[1];
            //page_table[ptid][i].entrylo[0].C = 2;
            //page_table[ptid][i].entrylo[1].C = 2;
            //page_table[ptid][i].entrylo[0].D = 1;
            //page_table[ptid][i].entrylo[1].D = 1;
            page_table[ptid][i].entrylo[0].V = 1;
            page_table[ptid][i].entrylo[1].V = 1;
            //page_table[ptid][i].entrylo[0].G = 0;
            //page_table[ptid][i].entrylo[1].G = 0;

            entryhi = ((uint64_t)(page_table[ptid][i].VPN2) << 13) | ((uint64_t)(page_table[ptid][i].ASID) & 0xff);
            index = find_index(entryhi);
            entryhi = ((uint64_t)(page_table[ptid][i].VPN2) << 13) | ((uint64_t)(PROC_SWAP) & 0xff);
            entrylo0 = *(int*)&(page_table[ptid][i].entrylo[0]);
            entrylo1 = *(int*)&(page_table[ptid][i].entrylo[1]);
            set_a_tlb_entry(entryhi, entrylo0, entrylo1, index);

            free_disk_pf(page_table[ptid][i].disk_addr);
            swap_reg.swap_addr = (uint64_t)(page_table[ptid][i].VPN2<<13);
            swap_reg.swap_disk_addr = page_table[ptid][i].disk_addr;
            swap_reg.swap_rw = 0;
            swap_reg.swap_i = i;
            swap_reg.swap_asid = asid;
            swap_reg.swap_index = index;
            swap_reg.swap_valid = 1;
            swap_reg.swap_inwork = 1;

            vt100_move_cursor(1, 1);
            printk("[swap read] ptid: %d, num: %d, vpn: 0x%x, asid: %d, pt_vpn: 0x%x, pt_asid: %d, disk_addr: 0x%x", ptid, pf_num, (physical_frame[pf_num].VPN2<<13), physical_frame[pf_num].ASID, (page_table[ptid][i].VPN2 << 13), page_table[ptid][i].ASID, page_table[ptid][i].disk_addr);

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
    int ptid = proc_ptn[asid];

    page_table[ptid][i].entrylo[0].D = 1;
    page_table[ptid][i].entrylo[1].D = 1;

    entryhi = ((uint64_t)(page_table[ptid][i].VPN2) << 13) | ((uint64_t)(page_table[ptid][i].ASID) & 0xff);
    entrylo0 = *(int*)&(page_table[ptid][i].entrylo[0]);
    entrylo1 = *(int*)&(page_table[ptid][i].entrylo[1]);
    index = find_index(entryhi);
    set_a_tlb_entry(entryhi, entrylo0, entrylo1, index);
    printk("error: tlb_modify badVPN2: %d ptid: %d", badVPN2, ptid);
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
