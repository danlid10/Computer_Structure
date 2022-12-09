	out $zero, $imm1, $zero, $imm2, 6, L2		# set irqhandler as L2
	out $zero, $zero, $imm2, $imm1, 1, 2		# enable irq2
L1:
	in $t0, $imm1, $zero, $zero, 8, 0		# $t0 = clocks counter
	blt $zero, $t0, $imm2, $imm1, L1, 1024		# if ($t0 < 1024) goto L1
	add $t0, $zero, $zero, $zero, 0, 0		# $t0 = 0 do will be the same for all students
	halt $zero, $zero, $zero, $zero, 0, 0		# halt
L2:
	add $t1, $t1, $imm1, $zero, 1, 0		# increment $t1
	out $zero, $zero, $imm2, $zero, 0, 5		# clear irq2 status
	reti $zero, $zero, $zero, $zero, 0, 0		# return from interrupt
