本实验主要的文件及其主要功能如下：

-------------------------------------------------------------------------------------------------------------------
Project4-Virtual Memory Management-part2 (C core) 部分
-------------------------------------------------------------------------------------------------------------------

1. main.c
    文件位于【Project4-Virtual Memory Management-part2/init】目录下
    添加功能：（1）init_memory 函数，初始化页表和物理页框。
                    （2）init_task 函数，初始化用户进程信息。
    其他功能：见【Project3】部分

2. memory.c
    文件位于【Project4-Virtual Memory Management-part2/mm】目录下
    主要功能：（1）init_page_table 函数，初始化页表。
                    （2）physical_frame_initial 函数，初始化物理页框。
                    （3）init_TLB 函数，初始化TLB例外处理。
                    （4）release_page_table 函数，回收页表和物理页框。
                    （5）init_user_tlb 函数，初始化进程代码段页表和TLB。
                    （6）do_TLB_Refill 函数，处理TLB refill例外。
                    （7）do_page_fault 函数，处理TLB invalid例外。
                    （8）do_tlb_modify 函数，处理TLB modify例外。
                    （9）swap 函数，处理换页。

3. entry.S
    文件位于【Project4-Virtual Memory Management-part2/arch/mips/kernel】目录下
    添加功能：（1）TLBexception_handler_entry，TLB重填例外处理入口。
                    （2）handle_tlb ，正常例外处理入口的TLB处理函数。
    其他功能：见【Project3】部分

4. tiny_libc文件夹
    文件位于【Project4-Virtual Memory Management-part2/tiny_libc】目录下
    主要功能：（1）用户进程的系统调用函数。
                    （2）用户进程的头文件、string等库函数。

5. process_swap.c
    文件位于【Project4-Virtual Memory Management-part2/test/test_project4】目录下
    主要功能：（1）换页的IO操作，完成SD读写。

6. share_mm.c
    文件位于【Project4-Virtual Memory Management-part2/mm】目录下
    主要功能：（1）share_memory_initial 函数，初始化共享内存。
                    （2）do_shmget 函数，根据key得到一个共享内存ID。
                    （3）do_shmat 函数，根据共享内存ID建立共享内存映射，返回虚地址。
                    （4）do_shmdt 函数，根据共享内存的虚地址取消共享内存映射。

