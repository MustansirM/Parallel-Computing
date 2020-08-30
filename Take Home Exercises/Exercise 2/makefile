all: program
	@echo "Execute with ./program"

program: rgc.o
	gcc rgc.o -o program

rgc.o: rgc.c
	gcc -c rgc.c