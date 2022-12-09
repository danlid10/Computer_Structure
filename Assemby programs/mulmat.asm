	sll $sp, $imm1, $imm2, $zero, 1, 11			# set $sp = 1 << 11 = 2048
	add $a0, $zero, $imm2, $zero, 0, 0x100		# $a0 = 0x100 , address of the 1st matrix (A) 
	add $a1, $zero, $imm2, $zero, 0, 0x110		# $a1 = 0x110 , address of the 2nd matrix (B) 
	add $a2, $zero, $imm2, $zero, 0, 0x120 		# $a2 = 0x120 , address of the result matrix (C) 
	jal $ra, $zero, $zero, $imm2, 0, mulmat		# call mulmat
	halt $zero, $zero, $zero, $zero, 0, 0		# halt
mulmat:	
	add $sp ,$sp, $imm2, $zero, 0, -3			# push stack for 3 items
	sw $zero, $sp, $imm2, $s2, 0, 2 			# save $s2
	sw $zero, $sp, $imm2, $s1, 0, 1 			# save $s1
	sw $zero, $sp, $imm2, $s0, 0, 0 			# save $s0
	add $s0, $zero, $zero, $zero, 0, 0			# $s0 = i = 0
loop1:
	bge $zero, $s0, $imm1, $imm2, 4, end1 		# if(i >= 4) goto end1
	add $s1, $zero, $zero, $zero, 0, 0			# $s1 = j = 0
loop2:
	bge $zero, $s1, $imm1, $imm2, 4, end2 		# if(j >= 4) goto end2
	add $t0, $zero, $zero, $zero, 0, 0			# $t0 = sum = 0
	add $s2, $zero, $zero, $zero, 0, 0			# $s2 = k = 0
loop3:
	bge $zero, $s2, $imm1, $imm2, 4, end3 		# if(k >= 4) goto end3
	sll $t1, $s0, $imm2, $zero, 0, 2			# $t1 = 4*i
	add $t1, $a0, $t1, $s2, 0, 0				# $t1 = A + 4*i + k = &A[i][k]
	lw  $t1, $zero, $t1, $zero, 0, 0			# $t1 = A[i][k]
	sll $t2, $s2, $imm2, $zero, 0, 2			# $t2 = 4*k
	add $t2, $a1, $t2, $s1, 0, 0				# $t2 = B + 4*k + j = &B[k][j]
	lw  $t2, $zero, $t2, $zero, 0, 0			# $t2 = B[k][j]
	mac $t0, $t1, $t2, $t0, 0, 0				# $t0 = $t0 + $t0*$t1 (sum =+  A[i][k]*B[k][j])
	add $s2, $s2, $imm2, $zero, 0, 1			# k++
	beq $zero, $zero ,$zero, $imm2, 0, loop3	# goto loop3
end3:
	sll $t1, $s0, $imm2, $zero, 0, 2			# $t1 = 4*i
	add $t1, $a2, $t1, $s1, 0, 0				# $t1 = C + 4*i + j = &C[i][j]
	sw $zero, $t1, $zero, $t0, 0, 0				# C[i][j] = sum
	add $s1, $s1, $imm2, $zero, 0, 1			# j++
	beq $zero, $zero ,$zero, $imm2, 0, loop2	# goto loop2
end2:	
	add $s0, $s0, $imm2, $zero, 0, 1			# i++
	beq $zero, $zero, $zero, $imm2, 0, loop1	# goto loop1	
end1:
	lw $s2, $sp, $imm2, $zero, 0, 2 			# restore $s2
	lw $s1, $sp, $imm2, $zero, 0, 1 			# restore $s1
	lw $s0, $sp, $imm2, $zero, 0, 0 			# restore $s0
	add $sp ,$sp, $imm2, $zero, 0, 3			# pop 3 items from stack
	beq $zero, $zero, $zero, $ra, 0, 0			# return
	