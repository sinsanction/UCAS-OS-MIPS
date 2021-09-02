本实验主要的文件及其主要功能如下：

1. bootblock.S
    文件位于【Project1-MIPS/arch/mips/boot】目录下
    主要功能：（1）打印提示字符串 "It's bootblock!"。
                    （2）将kernel代码从SD卡拷贝到内存中指定位置。
                    （3）跳转到kernel的入口位置，启动kernel。
    高级功能：（1）支持对较大kernel的加载。
                    （2）加载kernel时重定位。
    注：
    对于重定位使用了两种实现方法，
    bootblock.S中的方法可支持【bootloader在加载kernel后继续完成其他工作】；
    另一种偏向技巧的方法收录在同目录下的【bootblock2.S】文件中，
    该方法实现更简单，但不支持bootloader加载kernel后继续完成其他工作。
    两种方法的具体思路和实现请见【设计文档】。

2. kernel.c
    文件位于【Project1-MIPS/test/test_project1】目录下
    主要功能   （1）打印提示字符串 "Hello OS!"。
                    （2）打印提示字符串 "Ready for input!!!!!!!!"。
                    （3）通过串口寄存器读入用户输入的字符并进行回显。
                    （4）回显支持换行功能。

3. createimage.c
    文件位于【Project1-MIPS/tools】目录下
    主要功能   （1）读取输入的ELF文件（如bootblock、kernel），提取他们的程序段，并写入image文件。
                    （2）将kernel的大小写入image第一个扇区末尾的空白处。
                    （3）在-extended 选项下打印相关信息，如各ELF的各程序段的位置、大小等。
    注：
    执行createimage的样例输入【./createimage --extended bootblock main】
    执行该文件可能需要添加权限【chmod +x createimage】
    理论上createimage也支持超过2个ELF文件生成image，但第一个文件的程序段不应该超过一个扇区512B大小。
    在计算kernel大小时，除第一个文件之外，其他部分都会被视作kernel，算进kernel大小。
