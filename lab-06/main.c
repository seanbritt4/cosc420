#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <limits.h>
#include "mLib.h"
// #include "vector.h"
// #include "matadd.h"

int main(int argc, char** argv){
	MPI_Init(NULL, NULL);
	MPI_Comm world = MPI_COMM_WORLD;
	int rank, nprocs;
	MPI_Comm_rank(world, &rank);
	MPI_Comm_size(world, &nprocs);

	if(rank == 0){
		srand(0);
		struct matrix A, B, C;

		if(argc == 1){
			printf("Useage: mpiexec ./[executable] [n] (will create two nxn matrices)\n");
			printf("or\n");
			printf("Useage: mpiexec ./[exectutable] [n] [m] (will create an nxm matrix)\n");
			exit(1);
		}else if(argc == 2){ //creates square matrixes of the same size
			int s = atoi(argv[1]);
			initMatrix(&A, s, s);
			// initMatrix(&B, s, s);
			// initMatrix(&C, s, s);
		}else if(argc == 3){
			int a = atoi(argv[1]);
			int b = atoi(argv[2]);
			initMatrix(&A, a, b);
		}

		printMatrix(&A);
		powerMethod(&A);

		free(A.arr);
		// free(B.arr);
		// free(C.arr);
	}

	MPI_Finalize();
	return 0;
}
