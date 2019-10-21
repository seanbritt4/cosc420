#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <limits.h>
// #include "mLib.h"
#include "matadd.h"

int main(int argc, char** argv){
	MPI_Init(NULL, NULL);

	srand(0);
	struct matrix A, B, C;
	// struct matrix B;
	// struct matrix

	if(argc == 1){
		initMatrix(&A, 4, 8);
		initMatrix(&B, 8, 4);
	}else if(argc == 2){ //creates two square matrixes of the same size
		int s = atoi(argv[1]);
		initMatrix(&A, s, s);
		initMatrix(&B, s, s);
		initMatrix(&C, s, s);

	}else if(argc == 5){	//creates two matrices of sizes passed as arguments
		initMatrix(&A, atoi(argv[1]), atoi(argv[2]));
		initMatrix(&B, atoi(argv[3]), atoi(argv[4]));
		initMatrix(&C, atoi(argv[3]), atoi(argv[4]));
	}

	// printf("multiplying A(%d x %d) * B(%d x %d)\n", A.rows, A.cols, B.rows, B.cols);
	// matrixMult(&A, &B, &C);
	//
	// printf("C:\n");
	// printMatrix(&C);

	printf("adding A(%d x %d) + B(%d x %d)\n", A.rows, A.cols, B.rows, B.cols);
	matrixAdd(&A, &B, &C, 0);

	printf("C:\n");
	printMatrix(&C);

	printf("subtracting A(%d x %d) - B(%d x %d)\n", A.rows, A.cols, B.rows, B.cols);
	matrixAdd(&A, &B, &C, 1);

	printf("C:\n");
	printMatrix(&C);

	MPI_Finalize();
	free(A.arr);
	free(B.arr);
	free(C.arr);
	return 0;
}
