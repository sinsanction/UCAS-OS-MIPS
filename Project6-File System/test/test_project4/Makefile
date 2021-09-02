CC = mips64el-linux-gcc

all: clean createimage image asm   #floppy
MAIN_DIR = ../..
SRC_BOOT 	= $(MAIN_DIR)/arch/mips/boot/bootblock.S

SRC_ARCH	= $(MAIN_DIR)/arch/mips/kernel/entry.S $(MAIN_DIR)/arch/mips/pmon/common.c 
SRC_DRIVER	= $(MAIN_DIR)/drivers/screen.c $(MAIN_DIR)/drivers/mac.c 
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
SRC_TEST5_0	= ./test_recv.c 
SRC_TEST5_1	= ./test_send.c 
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
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0xffffffffa1800000 -N -o shell \
	$(SRC_SHELL) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

proc1: $(SRC_TEST4_1)
	${CC} -g -G 0 -I$(MAIN_DIR)/tiny_libc/include -I$(MAIN_DIR)/arch/mips/include \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0xffffffffa1900000 -N -o proc1 \
	$(SRC_TEST5_0) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

proc2: $(SRC_TEST4_2)
	${CC} -g -G 0 -I$(MAIN_DIR)/tiny_libc/include -I$(MAIN_DIR)/arch/mips/include \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0xffffffffa1a00000 -N -o proc2 \
	$(SRC_TEST5_1) $(SRC_LIBC) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

createimage: $(SRC_IMAGE)
	gcc $(SRC_IMAGE) -o createimage

image: bootblock main shell proc1 proc2
	./createimage --extended bootblock main shell proc1 proc2

clean:
	rm -rf bootblock image createimage main shell proc1 proc2 *.o

floppy:
	sudo fdisk -l /dev/sdb
	sudo dd if=image of=/dev/sdb conv=notrunc
	sudo dd if=image of=disk conv=notrunc

asm:
	mips64el-linux-objdump -d main > kernel.txt
	mips64el-linux-objdump -d proc1 > proc1.txt


	

