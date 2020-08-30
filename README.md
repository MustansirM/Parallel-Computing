# Parallel-Computing
Programming assignments completed for CS F422: Parallel Computing (Sem 2 2019-20)

## Assignment 1
Two questions were given and work was to be divided between two students.
I completed the second question, whose problem statement and deliverables are given below.

### Question 2

You are required to implement a parallel algorithm for solving a system of linear equations
given in a file ‘input.txt’ using Gaussian Elimination method.
  (a) Algorithm must be designed to take maximum advantage of the number of processes
      specified at run time. Should be implemented using MPI.
  (b) Must identify the tasks, communication patterns, task agglomeration, and task
      mapping strategies as per Foster design methodology
  (c) Identify the theoritical Speedup possible
  (d) Evaluate the speedup achieved by running your program for a single processor and  
      multiple processors in the increment of 1 processor at a time.

Deliverables:
 - Design Document (.pdf). Must contain answers for (b), (c), (d)
 - gaussian_parallel.c for (a)

## Assignment 2
Two questions were given and work was to be divided between two students.
I completed the second question, whose problem statement and deliverables are given below.

### Question 2

Implement and test OpenMP program for computing a matrix-matrix product. Use the
OMP_NUM_THREADS environment variable to control the number of threads and plot the
performance with varying numbers of threads separately for the following three cases
  (a) only the outermost loop is parallelized;
  (b) the outer two loops are parallelized; and
  (c) all three loops are parallelized.
  (d) What is the observed result from these three cases?
 
Deliverables:
  - Design Document (.pdf) containing design steps of parallel algorithm and implementation aspects
  - Source code: matrixmul_case_a.c, matrixmul_case_b.c, matrixmul_case_c.c
  - Input files
  - Plots and data
  - Screen shots/ Video recording of the demo
  
## Take Home Assignements
5 Take Home Assigments were given, to be done individually. 
Any 4 out of 5 could be completed for full credit.

A .c file for the implementation and an associated make file have been created for each individual problem. 
The problem statement have been included as a comment at the beginning of each .c file
