/*
 Compute the value of pi
 
 Input:
    Number of steps to be performed for the computation
 
 Output:
    Value of pi
 
*/

#include <omp.h>
#include <stdio.h>

int main(int argc, char** argv) {
    double pi, sum, step, x, aux;
    int num_steps, i;
    printf("Enter number of terms: ");
    fflush(stdout);
    scanf("%d", &num_steps);
    x=0;
    sum = 0.0;
    step = 1.0/(double) num_steps;
    #pragma omp parallel private(i,x,aux) shared(sum) 
    {
        #pragma omp for schedule(static) 
        for(i=0; i < num_steps; i++) {
            x = (i + 0.5) * step;
            aux = 4.0 / (1.0 + x * x);
            #pragma omp critical
            sum = sum + aux;
        }
    }
    pi = step * sum;
    printf("Value of pi = %lf\n", pi);
}