.data
.dword 56, 98
.text
lui x5, 0x10
ld x11, 0(x5)
ld x12, 8(x5)

gcd_loop: beq x12, x0, done
add x14, x11, x12
add x14, x14, x12
sub x13, x11, x14
add x11, x12, x0
add x12, x13, x0
beq x0, x0, gcd_loop

done: addi x0, x0, 1