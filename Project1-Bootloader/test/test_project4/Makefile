CC = mips64el-linux-gcc

all: clean createimage image asm   floppy
MAIN_DIR = ../..
SRC_BOOT 	= $(MAIN_DIR)/arch/mips/boot/bootblock.S

SRC_ARCH	= $(MAIN_DIR)/arch/mips/kernel/entry.S $(MAIN_DIR)/arch/mips/kernel/syscall.S $(MAIN_DIR)/arch/mips/pmon/common.c 
SRC_DRIVER	= $(MAIN_DIR)/drivers/screen.c
SRC_INIT 	= $(MAIN_DIR)/init/main.c
SRC_INT		= $(MAIN_DIR)/kernel/irq/irq.c 
SRC_LOCK	=  $(MAIN_DIR)/kernel/locking/lock.c
 
SRC_SCHED	= $(MAIN_DIR)/kernel/sched/sched.c $(MAIN_DIR)/kernel/sched/queue.c $(MAIN_DIR)/kernel/sched/time.c
SRC_SYSCALL	= $(MAIN_DIR)/kernel/syscall/syscall.c
SRC_LIBS	= $(MAIN_DIR)/libs/mailbox.c $(MAIN_DIR)/libs/string.c $(MAIN_DIR)/libs/printk.c
SRC_MM		= $(MAIN_DIR)/mm/memory.c
SRC_IPC		= $(MAIN_DIR)/ipc/barrier.c $(MAIN_DIR)/ipc/cond.c $(MAIN_DIR)/ipc/sem.c
SRC_TEST	= $(MAIN_DIR)/test/test.c $(MAIN_DIR)/test/test_shell.c
SRC_TEST4	= ./process1.c ./process2.c
SRC_SMP 	= $(MAIN_DIR)/arch/mips/kernel/smp.c
SRC_IMAGE	= $(MAIN_DIR)/tools/createimage.c
LD = $(MAIN_DIR)/ld.script
bootblock: $(SRC_BOOT)
	${CC} -g -G 0 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800000 -N -o bootblock $(SRC_BOOT) -nostdlib -e main -Wl,-m -Wl,elf32ltsmip -T  $(LD) 

main : $(SRC_ARCH) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_LOCK) $(SRC_IPC) $(SRC_MM) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_LIBS) $(SRC_TEST)  $(SRC_TEST4)
	${CC} -g -G 0 -I$(MAIN_DIR)/include    -I$(MAIN_DIR)/libs  -I$(MAIN_DIR)/fs -I$(MAIN_DIR)/arch/mips/include -I$(MAIN_DIR)/drivers -I$(MAIN_DIR)/include/os -I$(MAIN_DIR)/include/sys -I$(MAIN_DIR)/test  -I$(MAIN_DIR)/test/test_project4   \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0xffffffffa0800200 -N -o main \
	$(SRC_TEST_FS) $(SRC_ARCH) $(SRC_FS) $(SRC_DRIVER) $(SRC_INIT) $(SRC_INT) $(SRC_IPC) $(SRC_SMP) $(SRC_LOCK)  $(SRC_MM) $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_PROC) $(SRC_LIBS) $(SRC_TEST)     $(SRC_TEST4) -nostdlib -Wl,-m -Wl,elf64ltsmip -T  $(LD) 

createimage: $(SRC_IMAGE)
	gcc $(SRC_IMAGE) -o createimage

image: bootblock main
	./createimage --extended bootblock main

clean:
	rm -rf bootblock image createimage main *.o

floppy:
	sudo fdisk -l /dev/sdb
	sudo dd if=image of=/dev/sdb conv=notrunc
	sudo dd if=image of=disk conv=notrunc

asm:
	mips64el-linux-objdump -d main > kernel.txt


	

