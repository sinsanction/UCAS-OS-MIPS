.text
.global main

main:
	# 1) call BIOS print string "It's bootblock!"
	dla  $a0, msg
	ld    $t1, printstr
 	jalr  $t1
	
	# 2) call BIOS read kernel in SD card
	  #read kernel_size
	ld   $t0, kernel_size_addr
	lhu  $t1, 0($t0)
	dsll  $t1, $t1, 9

	  #copy bootloader to the back of kernel
	ld  $t0, kernel
	daddi $t2, $t0, 0
	daddi $a0, $t0, 0
	daddi $a0, $a0, 0x200
	dadd  $t0, $t0, $t1
	copy:
	ld  $t3, 0($t2)
	sd  $t3, 0($t0)
	daddi $t2, $t2, 8
	daddi $t0, $t0, 8
	bne $t2, $a0, copy

	  #jump to the copy of bootloader
	jal read_pc
	read_pc:
	dadd  $ra, $ra ,$t1
	daddi $ra, $ra, 16
	jr $ra

	  #read kernel in SD card
	daddi $a0, $a0, -0x200
	daddi $a1, $0, 0x200
	daddi $a2, $t1, 0
	ld  $t2, read_sd_card
	jalr  $t2

	# 3) jump to kernel
	dli   $t1, 0xffffffffa0800000
	jalr  $t1

	# 4) while(1) --> stop here
 	stop:
	j stop
	nop

.data

msg: .ascii "It's a bootloader...\n\r"

# 1. PMON read SD card function address
# read_sd_card();
read_sd_card: .dword 0xffffffff8f0d5e10

# 2. PMON print string function address
# printstr(char *string)
printstr: .dword 0xffffffff8f0d5534

# 3. PMON print char function address
# printch(char ch)
printch: .dword 0xffffffff8f0d5570

# 4. kernel address (move kernel to here ~)
kernel: .dword 0xffffffffa0800000

# 5. kernel_size address
kernel_size_addr: .dword 0xffffffffa08001fc
