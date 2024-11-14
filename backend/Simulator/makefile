all: riscv_sim

riscv_sim: simulator.o app.o
	g++ simulator.o app.o -o riscv_sim

simulator.o: simulator.cpp simulator.h
	g++ -c simulator.cpp -o simulator.o

app.o: app.cpp
	g++ -c app.cpp -o app.o

clean:
	rm -f simulator.o app.o riscv_sim
