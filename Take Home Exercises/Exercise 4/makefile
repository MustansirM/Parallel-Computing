all: program
	@echo "Execute with ./program"

program: pi_openmp.o
	gcc pi_openmp.o -o program -fopenmp -lpthread

pi_openmp.o: pi_openmp.c
	gcc -c pi_openmp.c -fopenmp -lpthread
