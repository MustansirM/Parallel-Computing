all: program
	@echo "Execute with mpirun -np <number of processes> --oversubscribe ./program"

program: exponential_MPI.o
	mpicc exponential_MPI.o -o program

exponential_MPI.o: exponential_MPI.c
	mpicc -c exponential_MPI.c
	

