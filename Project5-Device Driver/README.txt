本实验主要的文件及其主要功能如下：

1. main.c
    文件位于【Project2-A Simple Kernel-part2/init】目录下
    主要功能：（1）初始化内核栈与用户栈空间。
                    （2）初始化系统调用。
                    （3）初始化例外处理。
                    （4）初始化PCB数组，载入task。
                    （5）初始化锁和信号量。
                    （6）初始化屏幕。
                    （7）开中断，开始第一次进程调度。

2. sched.h
    文件位于【Project2-A Simple Kernel-part2/include/os】目录下
    主要功能：（1）PCB结构体的定义。
                    （2）其他数据结构的定义和函数的声明。
    注：
    PCB结构体的具体设计请见【P2 part1】的【设计文档】。

3. sched.c
    文件位于【Project2-A Simple Kernel-part2/kernel/sched】目录下
    主要功能：（1）初始化一个task的PCB。
                    （2）初始化一个task的内核栈。
                    （3）分配内核栈与用户栈空间。
                    （4）检测睡眠中进程是否到时间。
                    （5）优先级调度算法。
                    （6）其他的内核功能，如进程睡眠do_sleep，阻塞进程do_block，解除阻塞do_unblock等。

4. entry.S
    文件位于【Project2-A Simple Kernel-part2/arch/mips/kernel】目录下
    主要功能：（1）do_scheduler汇编函数，完成一次进程切换。
                    （2）SWITCH_STACK宏定义，从用户栈空间切换到内核栈空间。
                    （3）SAVE_CONTEXT宏定义，保存进程的上下文到内核栈。
                    （4）RESTORE_CONTEXT宏定义，从内核栈载入进程的上下文。
                    （5）exception_handler_entry例外处理入口函数，切换内核态、保存现场、根据ExcCode跳到相应处理函数。
                    （6）handle_int中断处理函数。
                    （7）handle_syscall系统调用处理函数。
                    （8）读/写status、cause、count、compare寄存器的函数。

5. syscall.S
    文件位于【Project2-A Simple Kernel-part2/arch/mips/kernel】目录下
    主要功能：（1）syscall指令触发系统调用。

6. irq.c
    文件位于【Project2-A Simple Kernel-part2/kernel/irq】目录下
    主要功能：（1）interrupt_helper函数，根据cause寄存器查明中断类型，并跳到相应中断处理函数。
                    （2）irq_timer函数，处理时钟中断，完成刷新屏幕、更新计时器、重置count和compare寄存器、以及调用scheduler()切换进程。

7. syscall.c
    文件位于【Project2-A Simple Kernel-part2/kernel/syscall】目录下
    主要功能：（1）system_call_helper函数，根据系统调用号，查系统调用向量表，执行相应系统调用。
                    （2）封装各种系统调用sys*函数。

8. lock.c
    文件位于【Project2-A Simple Kernel-part2/kernel/locking】目录下
    主要功能：（1）do_mutex_lock_init，初始化一个互斥锁。
                    （2）do_mutex_lock_acquire，获取一个互斥锁，失败则将自身挂起。
                    （3）do_mutex_lock_release，释放一个互斥锁，同时解除被该锁阻塞的进程。
                    （4）do_binsem_init，初始化一个二元信号量。
                    （5）do_binsem_acquire，二元信号量P操作。
                    （6）do_binsem_release，二元信号量V操作。
                    （7）do_binsemget，输入一个key返回二元信号量id。

9. test_priority.c
    文件位于【Project2-A Simple Kernel-part2/test/test_project2】目录下
    主要功能：（1）测试优先级调度的10个任务，具体说明见【设计文档】。

10. test_measure.c
    文件位于【Project2-A Simple Kernel-part2/test/test_project2】目录下
    主要功能：（1）测量do_scheduler时间开销的任务，具体说明见【设计文档】。

