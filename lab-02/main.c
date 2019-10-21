#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include "mLib.h"


int main(int argc, char** argv){
	MPI_Init(NULL,NULL);
	srand(time(0));
	struct matrix A;
	struct matrix B;
	initMatrix(&A, 4, 3);
	initMatrix(&B, 3, 4);
	matrixMult(&A, &B);
	MPI_Finalize();
	return 0;

}
