.data
.word 10, 20, 30, 40
.text
lui x10,0x10
lw x11, 0(x10)   
lw x12, 4(x10)
lw x13, 8(x10)
lw x14, 12(x10)
addi x4,x3,1