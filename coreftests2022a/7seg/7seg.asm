	add $zero, $imm1, $imm2, $zero, 1, 2	# try to write 3 (1+2) to $zero -- shouldn't change it
	add $imm1, $imm1, $imm2, $imm1, 1, 2	# try to write 3 (1+2+1) to $imm1 -- shouldn't change it
	add $imm2, $imm2, $imm2, $zero, 1, 2	# try to write 4 (2+2) to $imm2 -- shouldn't change it
	add $s0, $zero, $s1, $imm2, 0, 1	# $s0 = 1
L1:
	out $zero, $zero, $imm2, $s0, 0, 10	# out $s0 to 7-segment display
	add $s0, $s0, $imm1, $zero, 1, 0	# increment $s0
	add $s0, $s0, $zero, $zero, 0, 0	# add $zero to $s0 (shouldn't change it)
	blt $zero, $s0, $imm1, $imm2, 100, L1	# jump to L1 if $s0 < 100
	halt $zero, $zero, $zero, $zero, 0, 0	# halt
