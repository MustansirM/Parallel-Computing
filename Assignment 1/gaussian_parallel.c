/*
Compilation:- 
mpicc -g -Wall -o gaussian_parallel gaussian_parallel.c
Execution:-
mpirun -np <number of processes> ./gaussian_parallel
*/

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<mpi.h>

int main(int argc, char** argv){

    //Input file open
    FILE *fp1 = fopen("./input.txt", "r");
    if(fp1==NULL){
        printf("Error opening input file\n");
        exit(0);
    }

    int n;
    //Reading the number of equations = the number of variable
    fscanf(fp1, "%d", &n);

    double *A_lin, *b_lin;
    double A[n][n];
    double b[n];
    // double y[n];
    double x[n];

    int rank, numtasks, /*sendcount, recvcount,*/ source, target, size, len;
    int *sendcounts, *sendcountsy, *displs, *displsy;

    // recvcount = n;
    // sendcount = n;
    source = 0;
    target = 0;

    double start_time, end_time;

    //MPI_Init
    MPI_Init(&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &numtasks);

    //To calculate the maximum number of equations that can be allocated to each task.
    if(n%numtasks!=0)
        size = n/numtasks+1;
    else
        size = n/numtasks;

    double buf[n*size];
    double bufy[size];
    double tempbuf[n];
    double tempy;
    double bufx[size];
    double tempx;

    //Input read only in source task.
    if(rank==source){

        A_lin = (double *)malloc(n * n * sizeof(double));
        b_lin = (double *)malloc(n * sizeof(double));
        
        //Reading the input
        for(int i=0; i<n; ++i){
            for(int j=0; j<n; ++j){
                fscanf(fp1, "%lf", (A_lin+i*n+j));
                
            }
        }
        
        for(int i=0; i<n; ++i){
            fscanf(fp1, "%lf", &b_lin[i]);
        }
        fclose(fp1);
        
        start_time = MPI_Wtime();

        //Re-arrangement of the equations
        int curr_size=0;
        for(int i = 0; i<numtasks; ++i){
            for(int j = 0; j<size; ++j){
                if((i+j*numtasks)<n){
                    for(int k = 0; k<n; ++k){
                        A[curr_size][k] = *(A_lin+(i+j*numtasks)*n+k);
                    }
                    b[curr_size] = *(b_lin+(i+j*numtasks));
                    ++curr_size; 
                }
            }
        }
        free(A_lin);
        free(b_lin);
        
        sendcounts = (int *)malloc(numtasks*sizeof(int));
        displs = (int *)malloc(numtasks*sizeof(int));
        sendcountsy = (int *)malloc(numtasks*sizeof(int));
        displsy = (int *)malloc(numtasks*sizeof(int));

        //Calculation of displacements and number of elements to be kept in each task.
        for(int i=0; i<numtasks; ++i){
            if(i<n%numtasks || n%numtasks==0){
                sendcounts[i] = size*n;
                sendcountsy[i] = size;
            }
            else{
                sendcounts[i] = (size-1)*n;
                sendcountsy[i] = size-1;
            }
        }

        for(int i=0; i<numtasks; ++i){
            displs[i] = 0;
            displsy[i] = 0;
        }

        for(int i=1; i<numtasks; ++i){
            displs[i] = displs[i-1]+sendcounts[i-1];
            displsy[i] = displsy[i-1]+sendcountsy[i-1];
        }
        
    }

    //Calculation of number of equation allotted to each task.
    if(rank<n%numtasks || n%numtasks==0)
        len = size;
    else
        len = size-1;
    
    //Input divided among each task in a cyclical manner. 
    MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE, buf, n*len, MPI_DOUBLE, source, MPI_COMM_WORLD);
    MPI_Scatterv(b, sendcountsy, displsy, MPI_DOUBLE, bufy, len, MPI_DOUBLE, source, MPI_COMM_WORLD);
    
    //Loop for the number of equations allotted to each task.
    //For each equation i (0<= i < n)
    //Data from the previous i-1 equation via the previous task
    //and passes i equations to the next task
    //i-1 equations are passed unchanged
    //The ith equation requires the previous i-1 equation to be computed   
    for(int m=0; m<len; ++m){

        if(numtasks==1){
            for(int j=m+1; j<n; ++j){
                buf[m*n+j] = buf[m*n+j]/buf[m*n+m];
            }
            bufy[m] = bufy[m]/buf[m*n+m];
            buf[m*n+m] = 1;
            for(int i=m+1; i<n; ++i){
                for(int j=m+1; j<n; ++j){
                    buf[i*n+j] = buf[i*n+j] - buf[i*n+m]*buf[m*n+j];
                }
                bufy[i] = bufy[i]-buf[i*n+m]*bufy[m];
                buf[i*n+m] = 0;
            }
            continue;
        }
        int prev_task, next_task, eq_no;

        //Computation of equation number
        eq_no = m*numtasks+rank;

        //Computation of the tasks from which a task receives/sends data.  
        if(rank-1<0)
            prev_task = numtasks-1;
        else
            prev_task = rank-1;
        if(rank+1>=numtasks)
            next_task = 0;
        else
            next_task = rank+1;
        
        //Equation number 0 only sends data. 
        if(eq_no==0){
            for(int i=eq_no+1; i<n; ++i){
                buf[i+m*n] = buf[i+m*n]/buf[eq_no+m*n];
            }
            bufy[m] = bufy[m]/buf[eq_no+m*n];
            buf[eq_no+m*n] = 1;
            MPI_Send(buf+m*n, n, MPI_DOUBLE, next_task, 0, MPI_COMM_WORLD);
            MPI_Send(bufy+m, 1, MPI_DOUBLE, next_task, 0, MPI_COMM_WORLD);
        }
        //Equation between 0 and n-1 both send and receive data
        else if((eq_no)!=(n-1)){    
            for(int i=0; i<eq_no; ++i){
                MPI_Recv(tempbuf, n, MPI_DOUBLE, prev_task, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&tempy, 1, MPI_DOUBLE, prev_task, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(tempbuf, n, MPI_DOUBLE, next_task, 0, MPI_COMM_WORLD);
                MPI_Send(&tempy, 1, MPI_DOUBLE, next_task, 0, MPI_COMM_WORLD);
                for(int j=i+1; j<n; ++j){
                    buf[j+m*n] = buf[j+m*n] - buf[i+m*n]*tempbuf[j];
                }
                bufy[m] = bufy[m]-buf[i+m*n]*tempy;
                buf[i+m*n] = 0;
            }
            for(int i=eq_no+1; i<n; ++i){
                buf[i+m*n] = buf[i+m*n]/buf[eq_no+m*n];
            }
            bufy[m] = bufy[m]/buf[eq_no+m*n];
            buf[eq_no+m*n] = 1;
            MPI_Send(buf+m*n, n, MPI_DOUBLE, next_task, 0, MPI_COMM_WORLD);
            MPI_Send(bufy+m, 1, MPI_DOUBLE, next_task, 0, MPI_COMM_WORLD);
        }
        //Equation n-1 only receives data. 
        else{
            for(int i=0; i<eq_no; ++i){
                MPI_Recv(tempbuf, n, MPI_DOUBLE, prev_task, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&tempy, 1, MPI_DOUBLE, prev_task, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for(int j=i+1; j<n; ++j){
                    buf[j+m*n] = buf[j+m*n] - buf[i+m*n]*tempbuf[j];
                }
                bufy[m] = bufy[m]-buf[i+m*n]*tempy;
                buf[i+m*n] = 0;
            }
            for(int i=eq_no+1; i<n; ++i){
                buf[i+m*n] = buf[i+m*n]/buf[eq_no+m*n];
            }
            bufy[m] = bufy[m]/buf[eq_no+m*n];
            buf[eq_no+m*n] = 1;
        }
    }
    
    //Inverse process as described above used to compute the X vector
    for(int m=len-1; m>=0; --m){
        if(numtasks==1){
            bufx[m] = bufy[m];
            for(int i=m-1; i>=0; --i)
                bufy[i] = bufy[i] - bufx[m]*buf[i*n+m];
            continue;
        }
        int prev_task, next_task, eq_no;

        eq_no = m*numtasks+rank;
        if(rank-1<0)
            next_task = numtasks-1;
        else
            next_task = rank-1;
        if(rank+1>=numtasks)
            prev_task = 0;
        else
            prev_task = rank+1;
        
        if(eq_no==(n-1)){
            bufx[m] = bufy[m];
            MPI_Send(bufy+m, 1, MPI_DOUBLE, next_task, 0, MPI_COMM_WORLD);
        }
        else if(eq_no!=0){    
            for(int i=n-1; i>eq_no; --i){
                MPI_Recv(&tempx, 1, MPI_DOUBLE, prev_task, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&tempx, 1, MPI_DOUBLE, next_task, 0, MPI_COMM_WORLD);
                bufy[m] = bufy[m]-buf[i+m*n]*tempx;
            }
            bufx[m] = bufy[m];
            MPI_Send(bufx+m, 1, MPI_DOUBLE, next_task, 0, MPI_COMM_WORLD);
        }
        else{
            for(int i=n-1; i>eq_no; --i){
                MPI_Recv(&tempx, 1, MPI_DOUBLE, prev_task, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                bufy[m] = bufy[m]-buf[i+m*n]*tempx;
            }
            bufx[m] = bufy[m];
        }
    }

    MPI_Gatherv(bufx, len, MPI_DOUBLE, x, sendcounts, displsy, MPI_DOUBLE, target, MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    //X vector is printed to file.
    if(rank==target){
        printf("Time taken: %f\n",(end_time-start_time)*1000);
        FILE* fp2 = fopen("./output_parallel.txt", "w");
        if(fp2==NULL){
            printf("Error opening output file\n");
            exit(0);
        }
        for(int j=0; j<size; ++j){
            for(int i=0; i<numtasks; ++i){
                if((i+j*numtasks)<n){
                    //printf("%.4f ", *(x+displsy[i]+j));
                    fprintf(fp2, "%.4f\n", *(x+displsy[i]+j));
                }
                else
                    break;
            }
        }
        fclose(fp2);
    }
    
    MPI_Finalize();
}