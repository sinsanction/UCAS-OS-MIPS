#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "screen.h"
#include "mm.h"

void process_swap(void)
{
    uint64_t mem_buff;
    uint32_t disk_base;
    int mode, i;
    
    while(1){
        if(!swap_valid){
            sys_block();
        }
        mem_buff = swap_addr;
        disk_base = swap_disk_addr;
        mode = swap_rw;
        if(mode == 1){
            for(i=0; i<8; i++){
                sdwrite(mem_buff, disk_base, 0x400);
                mem_buff += 0x400;
                disk_base += 0x400;
            }
            //sdwrite(mem_buff, disk_base, 0x2000);
            sys_update_pf();
        }
        else{
            for(i=0; i<8; i++){
                sdread(mem_buff, disk_base, 0x400);
                mem_buff += 0x400;
                disk_base += 0x400;
            }
            //sdread(mem_buff, disk_base, 0x2000);
            sys_update_pf();
        }
    }
    sys_exit();
}
