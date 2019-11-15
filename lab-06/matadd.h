#ifndef MATADD_H
#define MATADD_H

// #include "mLib.h"
#include <time.h>

struct matrix{
  int size, rows, cols;
  int* arr;
};

void initMatrix(struct matrix* A, int r, int c){
  A->rows = r;
  A->cols = c;
  A->arr = malloc(r*c*sizeof(int));
  int i;
  for(i=0; i<r*c; i++){
    A->arr[i] = rand() % 100+1;
  }
}

void printMatrix(struct matrix* A){
  int i,j;
  for(i=0; i<A->rows; i++){
    for(j=0; j<A->cols; j++){
      printf("%5d ", A->arr[i*A->cols+j]);
    }
    puts("");
  }
  puts("");
}

void matrixAdd(struct matrix *A, struct matrix *B, struct matrix *C, _Bool sub){
    if((A->rows != B->rows) || (A->cols != B->cols)){
      printf("Matices must be of the same size-- cannot perform this operation.\n");
      return;
    }

    //mpi stuff
    MPI_Init(NULL, NULL);
    MPI_Comm world = MPI_COMM_WORLD;
    int rank, nprocs;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &nprocs);

    // usefull variables
    int i,j,index, size, elem_per_proc, elem_rem;
    size = A->rows * A->cols; //since matrices must be same size, this is a universal variable for all matrices
    elem_per_proc = size/nprocs;
    elem_rem = size%nprocs;

    // arrays
    int *send_div, *send_rem, *recv_div, *recv_rem;
    send_div = malloc(elem_per_proc * 2 * sizeof(int));
    send_rem = malloc(2 * sizeof(int));

    // 'initialize' stage for each process
    if(rank==0){
      printf("root\n");
    }


}

#endif
