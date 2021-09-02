本实验主要的文件及其主要功能如下：

-------------------------------------------------------------------------------------------------------------------
Project5-Device Driver-part2 (C core) 部分
-------------------------------------------------------------------------------------------------------------------

1. main.c
    文件位于【Project5-Device Driver-part2/init】目录下
    添加功能：（1）do_init_mac 函数，初始化网卡设置。
    其他功能：见【Project4】部分

2. mac.c
    文件位于【Project5-Device Driver-part2/drivers】目录下
    主要功能：（1）mac_recv_desc_init 函数，初始化接收描述符。
                    （2）mac_send_desc_init 函数，初始化发送描述符。
                    （3）do_net_recv 函数，接收网络包。
                    （4）do_net_send 函数，发送网络包。
                    （5）register_irq_handler 函数，初始化网卡中断。
                    （6）mac_irq_handle 函数，网卡中断处理。

3. irq.c
    文件位于【Project5-Device Driver-part2/kernel/irq】目录下
    添加功能：（1）识别网卡中断。
    其他功能：见【Project2】部分
