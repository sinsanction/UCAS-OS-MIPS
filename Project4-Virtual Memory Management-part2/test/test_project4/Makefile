CC = mips64el-linux-gcc

all: clean createimage image asm   #floppy
MAIN_DIR = ../..
SRC_BOOT 	= $(MAIN_DIR)/arch/mips/boot/bootblock.S

SRC_ARCH	= $(MAIN_DIR)/arch/mips/kernel/entry.S $(MAIN_DIR)/arch/mips/pmon/common.c 
SRC_DRIVER	= $(MAIN_DIR)/drivers/screen.c
SRC_INIT 	= $(MAIN_DIR)/init/main.c
SRC_INT		= $(MAIN_DIR)/kernel/irq/irq.c 
SRC_LOCK	=  $(MAIN_DIR)/kernel/locking/lock.c
 
SRC_SCHED	= $(MAIN_DIR)/kernel/sched/sched.c $(MAIN_DIR)/kernel/sched/queue.c $(MAIN_DIR)/kernel/sched/time.c
SRC_SYSCALL	= $(MAIN_DIR)/kernel/syscall/syscall.c
SRC_LIBS	= $(MAIN_DIR)/libs/mailbox.c $(MAIN_DIR)/libs/string.c $(MAIN_DIR)/libs/printk.c
SRC_MM		= $(MAIN_DIR)/mm/memory.c $(MAIN_DIR)/mm/share_mm.c
SRC_IPC		= $(MAIN_DIR)/ipc/barrier.c $(MAIN_DIR)/ipc/cond.c $(MAIN_DIR)/ipc/sem.c
SRC_TEST	= $(MAIN_DIR)/test/test.c

SRC_SHELL	= $(MAIN_DIR)/test/test_shell.c 
SRC_TEST4_0	= ./process_swap.c 
SRC_TEST4_1	= ./process2.c 
SRC_TEST4_2	= ./process1.c 
SRC_TEST4_3	= ./swap_test.c 
SRC_TEST4_4	= ./shm_test0.c 
SRC_TEST4_5	= ./shm_test1.c 
SRC_LIBC    = $(MAIN_DIR)/tiny_libc/printf.c $(MAIN_DIR)/tiny_libc/string.c $(MAIN_DIR)/tiny_libc/syscall.c $(MAIN_DIR)/tiny_libc/syscall.S 

SRC_SMP 	= $(MAIN_DIR)/arch/mips/kernel/smp.c
SRC_IMAGE	= $(MAIN_DIR)/tools/createimage.c
LD = $(MAIN_DIR)/ld.script
bootblock: $(SRC_BOOT)
	${CC} -g -G 0 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800000 -N -o bootblock $(SRC_BOOT) -nostdlib -e main -Wl,-m -Wl,elf32ltsmip -T  $(LD) 

main : $(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_IPC) $(SRC_MM) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_LIBS) $(SRC_TEST)
	${CC} -g -G 0 -I$(MAIN_DIR)/include    -I$(MAIN_DIR)/libs  -I$(MAIN_DIR)/fs -I$(MAIN_DIR)/arch/mips/include -I$(MAIN_DIR)/drivers -I$(MAIN_DIR)/include/os -I$(MAIN_DIR)/include/sys -I$(MAIN_DIR)/test \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0xffffffffa0800200 -N -o main \
	$(SRC_TEST_FS) $(SRC_ARCH) $(SRC_FS) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_IPC) $(SRC_SMP) $(SRC_LOCK)  $(SRC_MM) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_PROC) $(SRC_LIBS) $(SRC_TEST) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

shell: $(SRC_SHELL)
	${CC} -g -G 0 -I$(MAIN_DIR)/tiny_libc/include -I$(MAIN_DIR)/arch/mips/include \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0x1000000 -N -o shell \
	$(SRC_SHELL) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

process_swap: $(SRC_TEST4_0)
	${CC} -g -G 0 -I$(MAIN_DIR)/tiny_libc/include -I$(MAIN_DIR)/arch/mips/include \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0x2000000 -N -o process_swap \
	$(SRC_TEST4_0) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

proc1: $(SRC_TEST4_1)
	${CC} -g -G 0 -I$(MAIN_DIR)/tiny_libc/include -I$(MAIN_DIR)/arch/mips/include \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0x3000000 -N -o proc1 \
	$(SRC_TEST4_1) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

proc2: $(SRC_TEST4_2)
	${CC} -g -G 0 -I$(MAIN_DIR)/tiny_libc/include -I$(MAIN_DIR)/arch/mips/include \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0x4000000 -N -o proc2 \
	$(SRC_TEST4_2) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

swap: $(SRC_TEST4_3)
	${CC} -g -G 0 -I$(MAIN_DIR)/tiny_libc/include -I$(MAIN_DIR)/arch/mips/include \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0x5000000 -N -o swap \
	$(SRC_TEST4_3) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

shm0: $(SRC_TEST4_4)
	${CC} -g -G 0 -I$(MAIN_DIR)/tiny_libc/include -I$(MAIN_DIR)/arch/mips/include \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0x6000000 -N -o shm0 \
	$(SRC_TEST4_4) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

shm1: $(SRC_TEST4_5)
	${CC} -g -G 0 -I$(MAIN_DIR)/tiny_libc/include -I$(MAIN_DIR)/arch/mips/include \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0x7000000 -N -o shm1 \
	$(SRC_TEST4_5) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

createimage: $(SRC_IMAGE)
	gcc $(SRC_IMAGE) -o createimage

image: bootblock main shell process_swap proc1 proc2 swap shm0 shm1
	./createimage --extended bootblock main shell process_swap proc1 proc2 swap shm0 shm1

clean:
	rm -rf bootblock image createimage main shell process_swap proc1 proc2 swap shm0 shm1 *.o

floppy:
	sudo fdisk -l /dev/sdb
	sudo dd if=image of=/dev/sdb conv=notrunc
	sudo dd if=image of=disk conv=notrunc

asm:
	mips64el-linux-objdump -d main > kernel.txt
	mips64el-linux-objdump -d proc1 > proc1.txt
	mips64el-linux-objdump -d process_swap > process_swap.txt
	mips64el-linux-objdump -d swap > swap.txt


	

