all: riscv_asm

riscv_asm: assembler.o app.o
	g++ assembler.o app.o -o riscv_asm

assembler.o: assembler.cpp assembler.h
	g++ -c assembler.cpp -o assembler.o

app.o: app.cpp
	g++ -c app.cpp -o app.o

clean:
	rm -f assembler.o app.o riscv_asm
