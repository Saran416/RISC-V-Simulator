.data
.dword 3, 0, 12, 125, 50, 32, 16 
.text
lui x3, 0x10   ;amogh is gay
addi x2, x3, 0x200 
ld x4, 0(x3) 
addi x3, x3, 8 
loop: beq x4, x0, quit 
ld x5, 0(x3) 

ld x6, 8(x3) 



    beq x6, x0, update 
    beq x5, x0, movex5tox6 
    blt x5, x6, case2
    case1: beq x6, x5, update
        sub x5, x5, x6
        beq x0, x0, check 
    case2: beq x6, x5, update
        sub x6, x6, x5
        beq x0, x0, check
    check:blt x6, x5, case1
        blt x5, x6, case2
update:sd x6, 0(x2)
    addi x4, x4, -1
    addi x3, x3, 16
    addi x2, x2, 8
    beq x0, x0, loop
movex5tox6:add x6, x0, x5
    beq x0, x0, update
quit: addi x0, x0, 0