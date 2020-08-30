/*
 Display the nodes corresponding to the Hamiltonian path
 in an n-dimensional hypercube
 Make the assumption that n = 2^d
 
 Input:
    d
 
 Output:
    Nodes corresponding to the Hamiltonian path of a 2^d hypercube
 
 Constraints:
    d is a positive integer
 
 Note:
    Output increasing exponentially with d
*/

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

int main(){
    int nodes;
    int d;
    printf("2^d nodes will be computed\n");
    printf("Enter d : ");
    scanf("%d", &d);
    nodes = 1<<d;
    for(int i = 0; i < nodes; ++i) {
        printf("Linear array : ");
        printf("%d ", i);
        int grey = i ^ (i >> 1); 
        printf("Binary : ");
        char bin[32], gry[32];
        for(int j = 0; j < 32; ++j) {
            bin[j] = '0';
            gry[j] = '0';
        }
        int n = i;
        int c = 0;
        while(n) {
            if (n & 1)
                bin[32-c-1] = '1';
            else
                bin[32-c-1] = '0';
            n >>= 1;
            ++c;
        }
        n = grey;
        c = 0;
        while(n) {
            if (n & 1)
                gry[32-c-1] = '1';
            else
                gry[32-c-1] = '0';
            n >>= 1;
            ++c;
        }
        for(int i = 32-d; i < 32; ++i)
            printf("%c", bin[i]);
        printf(" Gray : ");  
        for(int i = 32-d; i < 32; ++i)
            printf("%c", gry[i]);
        printf(" Node number of hypercube : ");
        printf("%d", grey);
        printf("\n");
    }
}
