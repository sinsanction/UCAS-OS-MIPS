本实验主要的文件及其主要功能如下：

-------------------------------------------------------------------------------------------------------------------
Project3-Interactive OS and Process Management-part2 (multi core) 部分
-------------------------------------------------------------------------------------------------------------------

1. main.c
    文件位于【Project3-Interactive OS and Process Management-part2/init】目录下
    主要功能：（1）初始化内核栈与用户栈空间。
                    （2）初始化系统调用。
                    （3）初始化例外处理。
                    （4）初始化PCB数组，载入task。
                    （5）初始化锁和信号量。
                    （6）初始化屏幕。
                    （7）初始化从核。
                    （8）开中断，开始第一次进程调度。

2. sched.c
    文件位于【Project3-Interactive OS and Process Management-part2/kernel/sched】目录下
    添加功能：（1）do_spawn 函数，启动一个任务。
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
    添加功能：（1）CORE_LOCK_ACQUIRE 宏定义，获取内核锁。
                    （2）CORE_LOCK_RELEASE 宏定义，释放内核锁。
                    （3）get_cpu_id 函数，获取当前运行的核的id。
    其他功能：见【Project2】部分

4. smp.c
    文件位于【Project3-Interactive OS and Process Management-part2/arch/mips/kernel】目录下
    主要功能：（1）init_pcb_core1 函数，给从核分配并初始化PCB和栈空间。
                    （2）loongson3_boot_secondary 函数，初始化从核、给Mailbox寄存器赋值。
                    （3）smp_bootstrap 函数，初始化从核的寄存器，开中断，开始第一次进程调度。

5. test_shell.c
    文件位于【Project3-Interactive OS and Process Management-part2/test】目录下
    主要功能：（1）getchar 函数，从串口读取一个输入字符。
                    （2）check_add_cursor_y 函数，y光标加1，并根据y值滚动屏幕。
                    （3）test_shell 函数，读取、回显、解析、执行命令。

6. cond.c
    文件位于【Project3-Interactive OS and Process Management-part2/ipc】目录下
    主要功能：（1）do_condition_init 函数，初始化条件变量。
                    （2）do_condition_wait 函数，释放锁-阻塞-获取锁。
                    （3）do_condition_signal 函数，唤醒一个被该条件变量阻塞的进程。
                    （4）do_condition_broadcast 函数，唤醒所有被该条件变量阻塞的进程。

7. barrier.c
    文件位于【Project3-Interactive OS and Process Management-part2/ipc】目录下
    主要功能：（1）do_barrier_init 函数，初始化屏障，设置goal。
                    （2）do_barrier_wait 函数，根据到达屏障数目选择阻塞或唤醒全部进程。

8. mailbox.c
    文件位于【Project3-Interactive OS and Process Management-part2/libs】目录下
    主要功能：（1）do_mbox_init 函数，初始化一个邮箱。
                    （2）do_mbox_open 函数，根据输入字符串打开一个邮箱并返回邮箱id。
                    （3）do_mbox_close 函数，关闭邮箱，当打开邮箱的任务数为0时回收邮箱。
                    （4）do_mbox_send 函数，向邮箱发送消息。
                    （5）do_mbox_recv 函数，从邮箱读取消息。
                    （6）mbox_is_full 函数，判断邮箱是否已满。
                    （7）mbox_is_empty 函数，判断邮箱是否已空。

9. syscall.c
    文件位于【Project3-Interactive OS and Process Management-part2/kernel/syscall】目录下
    添加功能：（1）添加shell命令和同步原语相关的系统调用。
    其他功能：见【Project2】部分


