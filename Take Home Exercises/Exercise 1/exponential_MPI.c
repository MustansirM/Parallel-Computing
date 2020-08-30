/*
 Compute the value of e^x
 
 Input:
    x
 
 Output:
    Value of e^x
 
 Constraints:
    x is an integer
*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {

    int rank, numtasks;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int source = 0;
    int destination = 0; 
    double x;

    if(rank == source) {
            printf("Enter value of exponent : ");
            fflush(stdout);
            scanf("%lf", &x);
 
    }

    MPI_Bcast(&x, 1, MPI_DOUBLE, source, MPI_COMM_WORLD);
    
    double snd, rcv, fin;

    for(int i = 0; i < numtasks; ++i) {
        if(i == rank) {
            if(rank == 0)
                snd = 1;
            else
                snd = x/i;
        }
    }

    MPI_Scan(&snd, &rcv, 1, MPI_DOUBLE, MPI_PROD, MPI_COMM_WORLD);
    MPI_Reduce(&rcv, &fin, 1, MPI_DOUBLE, MPI_SUM, destination, MPI_COMM_WORLD);

    if(rank == destination)
        printf("e^%d = %lf\n", (int)x, fin);
    
    MPI_Finalize();
}
