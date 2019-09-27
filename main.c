#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <limits.h>
#include "mLib.h"

int main(int argc, char** argv){
	MPI_Init(NULL, NULL);
	srand(0);
	struct matrix A;
	struct matrix B;
	
	if(argc == 1){
		initMatrix(&A, 4, 8);
		initMatrix(&B, 8, 4);
	}else if(argc == 5){
		initMatrix(&A, atoi(argv[1]), atoi(argv[2]));
		initMatrix(&B, atoi(argv[3]), atoi(argv[4]));
	}

	matrixMult(&A, &B);
	MPI_Finalize();
	return 0;
}
