#include "type.h"
#include "stdio.h"
#include "syscall.h"
#include "string.h"
#include "struct.h"

swap_t swap_reg;

void __attribute__((section(".entry_function"))) _start(void)
{
    uint64_t mem_buff;
    uint64_t disk_base;
    int mode, i;
    
    while(1){
        sys_get_swapreg(&swap_reg);
        if(!swap_reg.swap_valid){
            sys_block();
            sys_get_swapreg(&swap_reg);
        }
        mem_buff = swap_reg.swap_addr;
        //disk_base = swap_reg.swap_disk_addr + 0xffffffffa8000000;
        disk_base = swap_reg.swap_disk_addr;
        mode = swap_reg.swap_rw;
        if(mode == 1){
            //sys_move_cursor(1, 2);
            //printf("sdwrite begin                                        ");
            /*for(i=0; i<8; i++){
                //memcpy((char*)disk_base, (char*)mem_buff, 0x400);
                sys_sdwrite(mem_buff, disk_base, 0x400);
                mem_buff += 0x400;
                disk_base += 0x400;
            }*/
            sys_sdwrite(mem_buff, disk_base, 0x2000);
            //sys_move_cursor(1, 2);
            //printf("sdwrite finish                                      ");
            sys_update_pf();
        }
        else{
            //sys_move_cursor(1, 2);
            //printf("sdread begin                                        ");
            /*for(i=0; i<8; i++){
                //memcpy((char*)mem_buff, (char*)disk_base, 0x400);
                sys_sdread(mem_buff, disk_base, 0x400);
                mem_buff += 0x400;
                disk_base += 0x400;
            }*/
            sys_sdread(mem_buff, disk_base, 0x2000);
            //sys_move_cursor(1, 2);
            //printf("sdread finish                                      ");
            sys_update_pf();
        }
    }
    sys_exit();
}
