	add $sp, $zero, $imm1, $zero, 1, 0	# set $sp = 1
	sll $sp, $sp, $imm1, $zero, 11, 0	# set $sp = 1 << 11 = 2048
	add $t0, $zero, $imm1, $zero, 6, 0	# set $t0 = 6
	out $zero, $t0, $zero, $imm1, L2, 0	# set irqhandler as L2
	add $t0, $zero, $imm1, $zero, 1, 0	# set $t0 = 1
	out $zero, $zero, $imm1, $t0, 0, 0	# enable irq0
	add $t0, $zero, $imm1, $zero, 1023, 0	# set $t0 = 1023
	out $zero, $zero, $imm1, $t0, 13, 0	# write to timermax
	add $t0, $zero, $imm1, $zero, 1, 0	# set $t0 = 1
	out $zero, $zero, $imm1, $t0, 11, 0	# write to timerenable
L1:
	lw $a0, $zero, $imm1, $zero, 0, 0	# $a0 = mem[0]
	lw $a1, $zero, $imm1, $zero, 1, 0	# $a1 = mem[1]
	bne $zero, $a0, $a1, $imm1, L1, 0	# jump to L1 if mem[0] != mem[1]
	halt $zero, $zero, $zero, $zero, 0, 0	# halt
L2:
	in $s0, $zero, $imm1, $zero, 9, 0	# read leds register into $s0
	sll $s0, $s0, $imm1, $zero, 1, 0	# left shift led pattern to the left
	or $s0, $s0, $imm1, $zero, 1, 0	# lit up the rightmost led
	out $zero, $zero, $imm1, $s0, 9, 0	# write the new led pattern
	add $s0, $zero, $imm1, $zero, 255, 0	# set $s0 = 255
	out $zero, $zero, $imm1, $s0, 21, 0	# write to monitordata
	add $s0, $zero, $imm1, $zero, 1, 0	# set $s0 = 1
	out $zero, $zero, $imm1, $s0, 22, 0	# write to monitorcmd
	in $s0, $zero, $imm1, $zero, 20, 0	# read monitoraddr register into $s0
	add $s0, $s0, $imm1, $zero, 257, 0	# $s0 = $s0 + 257
	out $zero, $zero, $imm1, $s0, 20, 0	# set monitoraddr
	lw $s0, $zero, $imm1, $zero, 0, 0	# $s0 = mem[0]
	add $s0, $s0, $imm1, $zero, 1, 0	# $s0 = $s0 + 1
	sw $zero, $zero, $imm1, $s0, 0, 0	# mem[0] = $s0
	out $zero, $zero, $imm1, $zero, 3, 0	# clear irq0 status
	reti $zero, $zero, $zero, $zero, 0, 0	# return from interrupt
	.word 1 7
