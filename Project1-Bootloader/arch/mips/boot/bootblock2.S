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

	  #read kernel in SD card
	ld  $a0, kernel
	daddi $a1, $0, 0x200
	daddi $a2, $t1, 0
	ld  $t2, read_sd_card
	ld  $ra, kernel_main
	jr  $t2

	#use $ra to achieve relocation
	#in fact, the following codes will not be executed
	
	# 3) jump to kernel
	ld    $t1,kernel_main
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

# 5. kernel main address (jmp here to start kernel main!)
kernel_main: .dword 0xffffffffa0800000

# 6. kernel_size address
kernel_size_addr: .dword 0xffffffffa08001fc
