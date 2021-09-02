CC = mips64el-linux-gcc

all: clean createimage image asm qemu # floppy

MAIN_DIR = ../..


SRC_BOOT 	= $(MAIN_DIR)/arch/mips/boot/bootblock.S

SRC_ARCH	= $(MAIN_DIR)/arch/mips/kernel/entry.S $(MAIN_DIR)/arch/mips/kernel/syscall.S $(MAIN_DIR)/arch/mips/pmon/common.c 
SRC_INIT 	= $(MAIN_DIR)/init/main.c
SRC_DRIVER      = $(MAIN_DIR)/drivers/screen.c
SRC_INT		= $(MAIN_DIR)/kernel/irq/irq.c # ./kernel/irq/shell.c
SRC_LOCK	= $(MAIN_DIR)/kernel/locking/lock.c
SRC_SCHED	= $(MAIN_DIR)/kernel/sched/sched.c $(MAIN_DIR)/kernel/sched/queue.c $(MAIN_DIR)/kernel/sched/time.c
SRC_SYSCALL	= $(MAIN_DIR)/kernel/syscall/syscall.c
SRC_LIBS	= $(MAIN_DIR)/libs/string.c $(MAIN_DIR)/libs/printk.c
SRC_TEST	= ./test.c 
SRC_TEST2	= ./test_scheduler1.c ./test_scheduler2.c  ./test_lock2.c ./test_sleep.c ./test_timer.c 
SRC_IMAGE	= $(MAIN_DIR)/tools/createimage.c
LD = $(MAIN_DIR)/ld.script

bootblock: $(SRC_BOOT)
	${CC} -g -G 0 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800000 -N -o bootblock $(SRC_BOOT) -nostdlib -e main -Wl,-m -Wl,elf64ltsmip -T $(LD)

main : $(SRC_ARCH)  $(SRC_INIT) $(SRC_INT) $(SRC_DRIVER)  $(SRC_LOCK)  $(SRC_SCHED) $(SRC_SYSCALL) $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST2) 
	${CC} -g -G 0 -I$(MAIN_DIR)/include -I./ -I$(MAIN_DIR)/drivers -I$(MAIN_DIR)/fs -I$(MAIN_DIR)/libs -I$(MAIN_DIR)/arch/mips/include -I$(MAIN_DIR)/include/os -I$(MAIN_DIR)/include/sys  \
	-fno-pic -mno-abicalls -fno-builtin -nostdinc  -mabi=64  -mips64 -Ttext=0xffffffffa0800200 -N -o main \
	 $(SRC_ARCH)  $(SRC_INIT) $(SRC_INT)  $(SRC_LOCK) $(SRC_SCHED) $(SRC_DRIVER) $(SRC_SYSCALL)  $(SRC_LIBS) $(SRC_TEST) $(SRC_TEST2)  -nostdlib -Wl,-m -Wl,elf64ltsmip -T $(LD) 

createimage: $(SRC_IMAGE)
	gcc $(SRC_IMAGE) -o createimage

image: bootblock main
	./createimage --extended bootblock main

clean:
	rm -rf bootblock image disk  createimage main *.o

floppy:
	sudo fdisk -l /dev/sdb
	sudo dd if=image of=/dev/sdb conv=notrunc
	sudo dd if=image of=disk conv=notrunc

asm:
	mips64el-linux-objdump -d main > kernel.txt

qemu:
	sudo dd if=image of=disk conv=notrunc	
