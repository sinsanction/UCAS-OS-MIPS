# UCAS-OS-MIPS

UCAS 操作系统实验课 一个简单的 MIPS64 架构操作系统

仅作思路参考，请不要直接照抄。



以下介绍各部分功能，采用迭代更新，只介绍每部分新增功能。

具体功能实现请查看各部分的【设计文档】。


## Project6-File System 部分

1. fs.c

     文件位于【Project6-File System/fs】目录下

     主要功能：

            （1）init_fs 函数，初始化建立文件系统。
            （2）read_super_block 函数，从磁盘读取文件系统。
            （3）print_fs 函数，打印文件系统信息。
            （4）clear_fs 函数，清除文件系统。
            （5）readdir 函数，打印目录或文件信息，即 ls 命令。
            （6）enterdir 函数，进入一个目录，即 cd 命令。
            （7）mkdir 函数，建立一个目录。
            （8）rmdir 函数，删除一个目录。
            （9）mknod 函数，建立一个空文件。
            （10）open 函数，打开文件。
            （11）write 函数，写文件。
            （12）read 函数，读文件。
            （13）close 函数，关闭文件。
            （14）cat 函数，打印文件。
            （15）hard_link 函数，建立硬链接。
            （16）soft_link 函数，建立软链接。

2. fs.h

    文件位于【Project6-File System/fs】目录下

    主要功能：

            （1）fs相关的各种数据结构和宏定义。

3. test_bigfile.c
   
    文件位于【Project6-File System/test/test_project6】目录下

    主要功能：

            （1）测试大文件，连续写和读一个大文件。


## Project5-Device Driver-part2 (C core) 部分

1. main.c
   
    文件位于【Project5-Device Driver-part2/init】目录下

    添加功能：

            （1）do_init_mac 函数，初始化网卡设置。

    其他功能：见【Project4】部分

2. mac.c
   
    文件位于【Project5-Device Driver-part2/drivers】目录下

    主要功能：

            （1）mac_recv_desc_init 函数，初始化接收描述符。
            （2）mac_send_desc_init 函数，初始化发送描述符。
            （3）do_net_recv 函数，接收网络包。
            （4）do_net_send 函数，发送网络包。
            （5）register_irq_handler 函数，初始化网卡中断。
            （6）mac_irq_handle 函数，网卡中断处理。

3. irq.c
   
    文件位于【Project5-Device Driver-part2/kernel/irq】目录下

    添加功能：

            （1）识别网卡中断。

    其他功能：见【Project2】部分


## Project4-Virtual Memory Management-part2 (C core) 部分

1. main.c
   
    文件位于【Project4-Virtual Memory Management-part2/init】目录下

    添加功能：

            （1）init_memory 函数，初始化页表和物理页框。
            （2）init_task 函数，初始化用户进程信息。

    其他功能：见【Project3】部分

2. memory.c
   
    文件位于【Project4-Virtual Memory Management-part2/mm】目录下

    主要功能：

            （1）init_page_table 函数，初始化页表。
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

    添加功能：

            （1）TLBexception_handler_entry，TLB重填例外处理入口。
            （2）handle_tlb ，正常例外处理入口的TLB处理函数。

    其他功能：见【Project3】部分

4. tiny_libc文件夹
   
    文件位于【Project4-Virtual Memory Management-part2/tiny_libc】目录下

    主要功能：
    
            （1）用户进程的系统调用函数。
            （2）用户进程的头文件、string等库函数。

5. process_swap.c
   
    文件位于【Project4-Virtual Memory Management-part2/test/test_project4】目录下

    主要功能：
    
            （1）换页的IO操作，完成SD读写。

6. share_mm.c

    文件位于【Project4-Virtual Memory Management-part2/mm】目录下

    主要功能：
    
            （1）share_memory_initial 函数，初始化共享内存。
            （2）do_shmget 函数，根据key得到一个共享内存ID。
            （3）do_shmat 函数，根据共享内存ID建立共享内存映射，返回虚地址。
            （4）do_shmdt 函数，根据共享内存的虚地址取消共享内存映射。


## Project3-Interactive OS and Process Management-part2 (multi core) 部分

1. main.c

    文件位于【Project3-Interactive OS and Process Management-part2/init】目录下

    主要功能：
    
            （1）初始化内核栈与用户栈空间。
            （2）初始化系统调用。
            （3）初始化例外处理。
            （4）初始化PCB数组，载入task。
            （5）初始化锁和信号量。
            （6）初始化屏幕。
            （7）初始化从核。
            （8）开中断，开始第一次进程调度。

2. sched.c
   
    文件位于【Project3-Interactive OS and Process Management-part2/kernel/sched】目录下

    添加功能：
    
            （1）do_spawn 函数，启动一个任务。
            （2）do_exit 函数，结束进程自身。
            （3）do_kill 函数，杀死目标进程。
            （4）do_waitpid 函数，等待目标进程结束。
            （5）do_band 函数，将指定进程绑定在指定核上。
            （6）free_kernel_stack、free_user_stack 函数，回收内核栈、用户栈。
            （7）get_new_pcb 函数，寻找一个可用pcb。
            （8）get_pcb 函数，由pid找到对应的pcb下标。
    
    其他功能：见【Project2】部分

3. entry.S
   
    文件位于【Project3-Interactive OS and Process Management-part2/arch/mips/kernel】目录下

    添加功能：
    
            （1）CORE_LOCK_ACQUIRE 宏定义，获取内核锁。
            （2）CORE_LOCK_RELEASE 宏定义，释放内核锁。
            （3）get_cpu_id 函数，获取当前运行的核的id。
    
    其他功能：见【Project2】部分

4. smp.c

    文件位于【Project3-Interactive OS and Process Management-part2/arch/mips/kernel】目录下

    主要功能：
    
            （1）init_pcb_core1 函数，给从核分配并初始化PCB和栈空间。
            （2）loongson3_boot_secondary 函数，初始化从核、给Mailbox寄存器赋值。
            （3）smp_bootstrap 函数，初始化从核的寄存器，开中断，开始第一次进程调度。

5. test_shell.c

    文件位于【Project3-Interactive OS and Process Management-part2/test】目录下

    主要功能：
    
            （1）getchar 函数，从串口读取一个输入字符。
            （2）check_add_cursor_y 函数，y光标加1，并根据y值滚动屏幕。
            （3）test_shell 函数，读取、回显、解析、执行命令。

6. cond.c

    文件位于【Project3-Interactive OS and Process Management-part2/ipc】目录下

    主要功能：
    
            （1）do_condition_init 函数，初始化条件变量。
            （2）do_condition_wait 函数，释放锁-阻塞-获取锁。
            （3）do_condition_signal 函数，唤醒一个被该条件变量阻塞的进程。
            （4）do_condition_broadcast 函数，唤醒所有被该条件变量阻塞的进程。

7. barrier.c

    文件位于【Project3-Interactive OS and Process Management-part2/ipc】目录下

    主要功能：
    
            （1）do_barrier_init 函数，初始化屏障，设置goal。
            （2）do_barrier_wait 函数，根据到达屏障数目选择阻塞或唤醒全部进程。

8. mailbox.c

    文件位于【Project3-Interactive OS and Process Management-part2/libs】目录下

    主要功能：
    
            （1）do_mbox_init 函数，初始化一个邮箱。
            （2）do_mbox_open 函数，根据输入字符串打开一个邮箱并返回邮箱id。
            （3）do_mbox_close 函数，关闭邮箱，当打开邮箱的任务数为0时回收邮箱。
            （4）do_mbox_send 函数，向邮箱发送消息。
            （5）do_mbox_recv 函数，从邮箱读取消息。
            （6）mbox_is_full 函数，判断邮箱是否已满。
            （7）mbox_is_empty 函数，判断邮箱是否已空。

9.  syscall.c

    文件位于【Project3-Interactive OS and Process Management-part2/kernel/syscall】目录下

    添加功能：
    
            （1）添加shell命令和同步原语相关的系统调用。
    
    其他功能：见【Project2】部分


## Project2-A Simple Kernel-part2 部分

1. main.c

    文件位于【Project2-A Simple Kernel-part2/init】目录下

    主要功能：
    
            （1）初始化内核栈与用户栈空间。
            （2）初始化系统调用。
            （3）初始化例外处理。
            （4）初始化PCB数组，载入task。
            （5）初始化锁和信号量。
            （6）初始化屏幕。
            （7）开中断，开始第一次进程调度。

2. sched.h

    文件位于【Project2-A Simple Kernel-part2/include/os】目录下

    主要功能：
    
            （1）PCB结构体的定义。
            （2）其他数据结构的定义和函数的声明。

    注：PCB结构体的具体设计请见【P2 part1】的【设计文档】。

3. sched.c

    文件位于【Project2-A Simple Kernel-part2/kernel/sched】目录下

    主要功能：
    
            （1）初始化一个task的PCB。
            （2）初始化一个task的内核栈。
            （3）分配内核栈与用户栈空间。
            （4）检测睡眠中进程是否到时间。
            （5）优先级调度算法。
            （6）其他的内核功能，进程睡眠do_sleep，阻塞进程do_block，解除阻塞do_unblock等。

4. entry.S

    文件位于【Project2-A Simple Kernel-part2/arch/mips/kernel】目录下

    主要功能：
    
            （1）do_scheduler汇编函数，完成一次进程切换。
            （2）SWITCH_STACK宏定义，从用户栈空间切换到内核栈空间。
            （3）SAVE_CONTEXT宏定义，保存进程的上下文到内核栈。
            （4）RESTORE_CONTEXT宏定义，从内核栈载入进程的上下文。
            （5）exception_handler_entry例外处理入口函数，切换内核态、保存现场、根据ExcCode跳到相应处理函数。
            （6）handle_int中断处理函数。
            （7）handle_syscall系统调用处理函数。
            （8）读/写status、cause、count、compare寄存器的函数。

5. syscall.S

    文件位于【Project2-A Simple Kernel-part2/arch/mips/kernel】目录下

    主要功能：
    
            （1）syscall指令触发系统调用。

6. irq.c

    文件位于【Project2-A Simple Kernel-part2/kernel/irq】目录下

    主要功能：
    
            （1）interrupt_helper函数，根据cause寄存器查明中断类型，并跳到相应中断处理函数。
            （2）irq_timer函数，处理时钟中断，完成刷新屏幕、更新计时器、重置count和compare寄存器、以及调用scheduler()切换进程。

7. syscall.c

    文件位于【Project2-A Simple Kernel-part2/kernel/syscall】目录下

    主要功能：
    
            （1）system_call_helper函数，根据系统调用号，查系统调用向量表，执行相应系统调用。
            （2）封装各种系统调用sys*函数。

8. lock.c

    文件位于【Project2-A Simple Kernel-part2/kernel/locking】目录下

    主要功能：
    
            （1）do_mutex_lock_init，初始化一个互斥锁。
            （2）do_mutex_lock_acquire，获取一个互斥锁，失败则将自身挂起。
            （3）do_mutex_lock_release，释放一个互斥锁，同时解除被该锁阻塞的进程。
            （4）do_binsem_init，初始化一个二元信号量。
            （5）do_binsem_acquire，二元信号量P操作。
            （6）do_binsem_release，二元信号量V操作。
            （7）do_binsemget，输入一个key返回二元信号量id。

9. test_priority.c

    文件位于【Project2-A Simple Kernel-part2/test/test_project2】目录下

    主要功能：

            （1）测试优先级调度的10个任务，具体说明见【设计文档】。

10. test_measure.c

    文件位于【Project2-A Simple Kernel-part2/test/test_project2】目录下

    主要功能：
    
            （1）测量do_scheduler时间开销的任务，具体说明见【设计文档】。


## Project2-A Simple Kernel-part1 部分

1. main.c

    文件位于【Project2-A Simple Kernel-part1/init】目录下

    主要功能：
    
            （1）初始化内核栈与用户栈空间。
            （2）初始化PCB数组，载入task。
            （3）初始化屏幕。
            （4）开始第一次进程调度。

2. sched.h

    文件位于【Project2-A Simple Kernel-part1/include/os】目录下

    主要功能：
    
            （1）PCB结构体的定义。
            （2）其他数据结构的定义和函数的声明。
    
    注：PCB结构体的具体设计请见【设计文档】。

3. sched.c

    文件位于【Project2-A Simple Kernel-part1/kernel/sched】目录下

    主要功能：
    
            （1）初始化一个task的PCB。
            （2）初始化一个task的内核栈。
            （3）分配内核栈与用户栈空间。
            （4）调度算法。
            （5）其他的内核功能，如阻塞进程do_block，解除阻塞do_unblock等。

4. entry.S

    文件位于【Project2-A Simple Kernel-part1/arch/mips/kernel】目录下

    主要功能：
    
            （1）do_scheduler汇编函数，完成一次进程切换。
            （2）SWITCH_STACK宏定义，从用户栈空间切换到内核栈空间。
            （3）SAVE_CONTEXT宏定义，保存进程的上下文到内核栈。
            （4）RESTORE_CONTEXT宏定义，从内核栈载入进程的上下文。

5. lock.c

    文件位于【Project2-A Simple Kernel-part1/kernel/locking】目录下

    主要功能：
    
            （1）do_mutex_lock_init，初始化一个互斥锁。
            （2）do_mutex_lock_acquire，获取一个互斥锁，失败则将自身挂起。
            （3）do_mutex_lock_release，释放一个互斥锁，同时解除被该锁阻塞的进程。


## Project1-Bootloader 部分

1. bootblock.S

    文件位于【Project1-Bootloader/arch/mips/boot】目录下

    主要功能：
    
            （1）打印提示字符串 "It's bootblock!"。
            （2）将kernel代码从SD卡拷贝到内存中指定位置。
            （3）跳转到kernel的入口位置，启动kernel。
    
    高级功能：
    
            （1）支持对较大kernel的加载。
            （2）加载kernel时重定位。
    
    注：
    对于重定位使用了两种实现方法，
    bootblock.S中的方法可支持【bootloader在加载kernel后继续完成其他工作】；
    另一种偏向技巧的方法收录在同目录下的【bootblock2.S】文件中，
    该方法实现更简单，但不支持bootloader加载kernel后继续完成其他工作。
    两种方法的具体思路和实现请见【设计文档】。

2. kernel.c

    文件位于【Project1-Bootloader/test/test_project1】目录下

    主要功能：
    
            （1）打印提示字符串 "Hello OS!"。
            （2）打印提示字符串 "Ready for input!!!!!!!!"。
            （3）通过串口寄存器读入用户输入的字符并进行回显。
            （4）回显支持换行功能。

3. createimage.c

    文件位于【Project1-Bootloader/tools】目录下

    主要功能：
    
            （1）读取输入的ELF文件（如bootblock、kernel），提取他们的程序段，并写入image文件。
            （2）将kernel的大小写入image第一个扇区末尾的空白处。
            （3）在-extended 选项下打印相关信息，如各ELF的各程序段的位置、大小等。
    
    注：
    执行createimage的样例输入【./createimage --extended bootblock main】
    执行该文件可能需要添加权限【chmod +x createimage】
    理论上createimage也支持超过2个ELF文件生成image，但第一个文件的程序段不应该超过一个扇区512B大小。
    在计算kernel大小时，除第一个文件之外，其他部分都会被视作kernel，算进kernel大小。

