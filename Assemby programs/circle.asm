	sll $sp, $imm1, $imm2, $zero, 1, 11			# set $sp = 1 << 11 = 2048
	lw $a0, $zero, $imm2, $zero, 0, 0x100		# $a0 = R ,from address 0x100
	jal $ra, $zero, $zero, $imm2, 0, circle		# circle(R)
	halt $zero, $zero, $zero, $zero, 0, 0		# halt
circle:
	add $sp ,$sp, $imm2, $zero, 0, -3			# push stack for 3 items
	sw $zero, $sp, $imm2, $s2, 0, 2 			# save $s2
	sw $zero, $sp, $imm2, $s1, 0, 1 			# save $s1
	sw $zero, $sp, $imm2, $s0, 0, 0 			# save $s0
	mac $s0, $a0, $a0, $zero, 0, 0				# $s0 = R^2
	add $s1, $imm2, $zero, $zero, 0, -128		# $s1 = x = -128
loop1:
	bge $zero, $s1, $imm1, $imm2, 128, end1 	# if(x >= 128) goto end1
	add $s2, $imm2, $zero, $zero, 0, -128		# $s2 = y = -128
loop2:
	bge $zero, $s2, $imm1, $imm2, 128, end2 	# if(y >= 128) goto end2
	mac $t0, $s1, $s1, $zero, 0, 0				# $t0 = x^2
	mac $t0, $s2, $s2, $t0, 0, 0				# $t0 = x^2 + y^2
	bgt $zero, $t0, $s0, $imm2, 0, cond			# if (x^2 +y^2 > R^2) goto cond
	add $t0, $s1,$imm2, $zero, 0, 128			# $t0 = x + 128
	sll $t0, $t0, $imm2, $zero, 0, 8			# $t0 = (x + 128) << 8 = 256*(x + 128)
	add $t1, $s2, $imm2, $zero, 0, 128			# $t1 = y + 128
	add $t0, $t0, $t1, $zero, 0, 0				# $t0 = 256*(x + 128) + (y + 128)
	out $zero, $zero, $imm1, $t0, 20, 0			# set pixel address
	out $zero, $zero, $imm1, $imm2, 21, 255		# set pixel color to white
	out $zero, $zero, $imm1, $imm2, 22, 1		# draw pixel
cond:
	add $s2, $s2, $imm2, $zero, 0, 1			# y++
	beq $zero, $zero, $zero, $imm2, 0, loop2	# goto loop2
end2:
	add $s1, $s1, $imm2, $zero, 0, 1			# x++
	beq $zero, $zero, $zero, $imm2, 0, loop1	# goto loop1
end1:
	lw $s2, $sp, $imm2, $zero, 0, 2 			# restore $s2
	lw $s1, $sp, $imm2, $zero, 0, 1 			# restore $s1
	lw $s0, $sp, $imm2, $zero, 0, 0 			# restore $s0
	add $sp ,$sp, $imm2, $zero, 0, 3			# pop 3 items from stack
	beq $zero, $zero, $zero, $ra, 0, 0			# return