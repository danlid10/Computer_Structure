	sll $sp, $imm1, $imm2, $zero, 1, 11			# set $sp = 1 << 11 = 2048
	lw $a0, $zero, $imm2, $zero, 0, 0x100		# $a0 = n ,from address 0x100
	lw $a1, $zero, $imm2, $zero, 0, 0x101 		# $a1 = k ,from address 0x101
	jal $ra, $zero, $zero, $imm2, 0, binom		# $v0 = binom(n,k)
	sw $zero, $zero, $imm2, $v0, 0, 0x102		# store binom(n,k) in address 0x102
	halt $zero, $zero, $zero, $zero, 0, 0		# halt
binom:
	add $sp ,$sp, $imm2, $zero, 0, -4			# push stack for 4 items
	sw $zero, $sp, $imm2, $s0, 0, 3 			# save $s0
	sw $zero, $sp, $imm2, $a0, 0, 2 			# save $a0
	sw $zero, $sp, $imm2, $a1, 0, 1 			# save $a1
	sw $zero, $sp, $imm2, $ra, 0, 0 			# save $ra
	beq $zero, $a0, $a1, $imm2, 0, cond			# if (n==k)	goto cond
	beq $zero, $a1, $zero, $imm2, 0, cond		# if (k==0)	goto cond
	sub $a0, $a0, $zero, $imm2, 0, 1			# $a0 = n - 1
	jal $ra, $zero, $zero, $imm2, 0, binom		# $v0 = binom(n-1,k)
	add $s0, $v0, $zero, $zero, 0, 0			# $s0 = binom(n-1,k)
	sub $a1, $a1, $zero, $imm2, 0, 1			# $a1 = k - 1
	jal $ra, $zero, $zero, $imm2, 0, binom		# $v0 = binom(n-1,k-1)
	add $v0, $v0, $s0, $zero, 0, 0				# $v0 = binom(n-1,k) + binom(n-1,k-1)
	beq $zero, $zero, $zero, $imm2, 0, end		# goto end
cond:
	add $v0, $zero, $zero, $imm2, 0, 1			# $v0 = 1
end:	
	lw $s0, $sp, $imm2, $zero, 0, 3 			# restore $s0
	lw $a0, $sp, $imm2, $zero, 0, 2 			# restore $a0
	lw $a1, $sp, $imm2, $zero, 0, 1 			# restore $a1
	lw $ra, $sp, $imm2, $zero, 0, 0 			# restore $ra
	add $sp ,$sp, $imm2, $zero, 0, 4			# pop 4 items from stack
	beq $zero, $zero, $zero, $ra, 0, 0			# return
