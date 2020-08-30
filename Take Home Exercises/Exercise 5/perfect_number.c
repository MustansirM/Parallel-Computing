/*
Display first N perfect numbers
 
Input:
    N
 
Output:
    First N perfect numbers
*/

#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

bool checkPrime(int n) {
    unsigned long long p = 1;
    p <<= n;
    p -= 1;
    if (p <= 1)
        return false;
    else if (p <= 3) 
        return true; 
    else if (p % 2 == 0 || p % 3 == 0) 
        return false; 
    else {
        for (unsigned long long i = 5; i * i <= p; i = i + 6) {
            if (p % i == 0 || p % (i + 2) == 0) { 
                return false; 
            }
        }
    }
    return true;
}
int main(int argc, char** argv) {

    int rank, numtasks;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int source = 0;
    int perfect_numbers;
    if(rank == source) {
            printf("Enter number of perfect numbers : ");
            fflush(stdout);
            scanf("%d", &perfect_numbers);
    }

    MPI_Bcast(&perfect_numbers, 1, MPI_INT, source, MPI_COMM_WORLD);

    int found = 0;
    int tot_found = 0;
    int index = rank;
    unsigned long long recvbuf[500];
    for(int i = 0; i < 500; ++i) {
        recvbuf[i] = 0;
    }
    unsigned long long sendbuf[50];
    for(int i = 0; i < 20; ++i) {
        sendbuf[i] = 0;
    }
    while(tot_found < perfect_numbers) {
        if(checkPrime(index)) {
            unsigned long long p = 1<<index;
            unsigned long long perfect = (p-1)*p/2;
            sendbuf[found++] = perfect;
        }
        MPI_Allreduce(&found, &tot_found, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Gather(sendbuf, perfect_numbers, MPI_UNSIGNED_LONG_LONG, recvbuf, perfect_numbers, MPI_UNSIGNED_LONG_LONG, source, MPI_COMM_WORLD);
        index += numtasks;
    }
    
    if(rank == source) {
        printf("First %d perfect numbers are :\n", perfect_numbers);
        unsigned long long smallest, curr;
        smallest = 1;
        for(int j = 1; j <= perfect_numbers; ++j) {
            curr = ULLONG_MAX;
            for(int i = 0; i < 500; ++i) {
                if(recvbuf[i] != 0 && recvbuf[i] > smallest) {
                    if(recvbuf[i] < curr)
                        curr = recvbuf[i];
                }
            }
            printf("%llu\n", curr);
            smallest = curr;
        }
    }

    MPI_Finalize();
}