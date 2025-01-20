all: riscv_sim riscv_asm

riscv_sim: simulator.o app.o cache.o
	g++ simulator.o app.o cache.o -o riscv_sim

riscv_asm: assembler.o assemblerApp.o
	g++ assembler.o assemblerApp.o -o riscv_asm

cache.o: cache_simulator.cpp cache_simulator.h
	g++ -c cache_simulator.cpp -o cache.o

simulator.o: simulator.cpp simulator.h cache.o
	g++ -c simulator.cpp cache.o -o simulator.o

assembler.o: assembler.cpp assembler.h
	g++ -c assembler.cpp -o assembler.o

app.o: app.cpp
	g++ -c app.cpp -o app.o

assemblerApp.o: assemblerApp.cpp
	g++ -c assemblerApp.cpp -o assemblerApp.o

clean:
	rm -f simulator.o app.o riscv_sim assembler.o assemblerApp.o riscv_asm cache.o
