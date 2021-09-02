本实验主要的文件及其主要功能如下：

1. main.c
    文件位于【Project2-A Simple Kernel-part1/init】目录下
    主要功能：（1）初始化内核栈与用户栈空间。
                    （2）初始化PCB数组，载入task。
                    （3）初始化屏幕。
                    （4）开始第一次进程调度。

2. sched.h
    文件位于【Project2-A Simple Kernel-part1/include/os】目录下
    主要功能：（1）PCB结构体的定义。
                    （2）其他数据结构的定义和函数的声明。
    注：
    PCB结构体的具体设计请见【设计文档】。

3. sched.c
    文件位于【Project2-A Simple Kernel-part1/kernel/sched】目录下
    主要功能：（1）初始化一个task的PCB。
                    （2）初始化一个task的内核栈。
                    （3）分配内核栈与用户栈空间。
                    （4）调度算法。
                    （5）其他的内核功能，如阻塞进程do_block，解除阻塞do_unblock等。

4. entry.S
    文件位于【Project2-A Simple Kernel-part1/arch/mips/kernel】目录下
    主要功能：（1）do_scheduler汇编函数，完成一次进程切换。
                    （2）SWITCH_STACK宏定义，从用户栈空间切换到内核栈空间。
                    （3）SAVE_CONTEXT宏定义，保存进程的上下文到内核栈。
                    （4）RESTORE_CONTEXT宏定义，从内核栈载入进程的上下文。

5. lock.c
    文件位于【Project2-A Simple Kernel-part1/kernel/locking】目录下
    主要功能：（1）do_mutex_lock_init，初始化一个互斥锁。
                    （2）do_mutex_lock_acquire，获取一个互斥锁，失败则将自身挂起。
                    （3）do_mutex_lock_release，释放一个互斥锁，同时解除被该锁阻塞的进程。

