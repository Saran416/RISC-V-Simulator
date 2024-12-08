.data
.dword 12
.text
addi x5, x0, 0xab
add x4, x5, x0
lui x3, 0x10   
ld x6, 0(x3)
sd x6, 24(x3)
slli x5, x5, 8