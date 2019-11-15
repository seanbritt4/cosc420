#include<stdio.h>
#include<stdlib.h>
#include<math.h>

void norm(float* b_hat, int* b){
    //determines the length of an array, or size of vector in this case
    int len = (sizeof(b)/sizeof(int))+1;

    //magnitude of b
    float b_mag = 0;
    for(int i=0; i<len; i++){
        //want the square root of all elements in b squared
        //e.g.:     sqrt(b_1^2 + b_2^2+ ... + b_n^2)
        b_mag += (b[i] * b[i]);
    }

    //v_hat = 1/b_mag, represents the 'unit' by which b is being normalized
    float v_hat = 0;
    v_hat = 1/sqrt(b_mag);
    printf("%f\n", v_hat);

    //multiply b*v_hat to find normalized vector b_hat
    for( int i=0; i<len; i++){
        b_hat[i] = b[i] * v_hat;
    }
}

int main(){
    int* b = malloc( sizeof(int) * 3);
    float* b_hat= malloc( sizeof(float) * 3);
    //
    // //4 2 4
    b[0] = 0;
    b[1] = 3;
    b[2] = -4;
    // //0 3 -4
    //
    norm(b_hat, b);
    int len = (sizeof(b_hat)/sizeof(float)) + 1;
    for(int i=0; i<len; i++){
        printf("%.2f\n", b_hat[i]);
    }

    return 0;
}
