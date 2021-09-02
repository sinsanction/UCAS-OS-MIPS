CC = mips64el-linux-gcc

all: image


SRC_BOOT=../../../arch/mips/boot/bootblock.S
SRC_KERNEL=../kernel.c
SRC_LD = ../../../ld.script
SRC_IMAGE=../../../tools/createimage.c

bootblock: $(SRC_BOOT)
	${CC} -g -G 0 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800000 -N -o bootblock $(SRC_BOOT) -nostdlib -e main -Wl,-m -Wl,elf64ltsmip -T $(SRC_LD)

main: 
	${CC} -g -G 0 -fno-pic -mno-abicalls -fno-builtin -nostdinc -mips3 -Ttext=0xffffffffa0800000 -N -o main $(SRC_KERNEL) -nostdlib -e main -Wl,-m -Wl,elf64ltsmip -T $(SRC_LD)	

createimage:
	gcc $(SRC_IMAGE) -o createimage

image: bootblock main createimage
	./createimage --extended bootblock main

clean:
	rm -rf bootblock createimage image main *.o

floppy:
	sudo dd if=image of=disk conv=notrunc
	sudo dd if=image of=/dev/sdb conv=notrunc
	sudo fdisk -l /dev/sdb
