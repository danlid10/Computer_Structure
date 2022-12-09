	sll $sp, $imm1, $imm2, $zero, 1, 11				# set $sp = 1 << 11 = 2048
	jal $ra, $zero, $zero, $imm2, 0, disktest		# disktest()
	halt $zero, $zero, $zero, $zero, 0, 0			# halt
disktest:
	add $sp ,$sp, $imm2, $zero, 0, -1				# push stack for 1 item
	sw $zero, $sp, $imm2, $s0, 0, 0 				# save $s0
	add $s0, $imm2, $zero, $zero, 0, 7				# i = 7
loop:
	blt $zero, $s0, $zero, $imm2, 0, end			# if(i < 0) goto end
read:
	in $t0, $imm2, $zero ,$zero, 0, 17				# $t0 = diskstatus
	beq $zero, $t0, $imm1, $imm2,  1, read			# polling - disk busy, wait for the disk to be ready to read
	out $zero, $imm1, $zero, $s0, 15, 0  			# disksector = i
	out $zero, $imm1, $zero, $imm2, 16, 0x100		# disbuffer = 0x100
	out $zero, $imm1, $zero, $imm2, 14, 1			# diskcmd = 1 - read
	add $t1, $s0, $zero, $imm2, 0, 1				# $t1 = i + 1
write:
	in $t0, $imm2, $zero,$zero, 0, 17				# $t0 = diskstatus
	beq $zero, $t0, $imm1, $imm2, 1, write			# polling - disk busy, wait for the disk to be ready to write
	out $zero, $imm1, $zero, $t1, 15, 0  			# disksector = i + 1
	out $zero, $imm1, $zero, $imm2, 16, 0x100		# disbuffer = 0x100
	out $zero, $imm1, $zero, $imm2, 14, 2			# diskcmd = 2 - write
	sub $s0, $s0, $zero, $imm2, 0, 1				# i--
	beq $zero, $zero, $zero, $imm2, 0, loop			# goto loop
end:
	in $t0, $imm2, $zero ,$zero, 0, 17				# $t0 = diskstatus
	beq $zero, $t0, $imm1, $imm2,  1, end			# polling - disk busy, wait for the last write to get done
	lw $s0, $sp, $imm2, $zero, 0, 0 				# restore $s0
	add $sp ,$sp, $imm2, $zero, 0, 1				# pop 1 item from stack
	beq $zero, $zero, $zero, $ra, 0, 0				# return						