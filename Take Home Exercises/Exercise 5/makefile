all: program
	@echo "Execute with mpirun ./program"

program: perfect_number.o
	mpicc perfect_number.o -o program 

perfect_number.o: perfect_number.c
	mpicc -c perfect_number.c 