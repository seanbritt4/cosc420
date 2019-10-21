#ifndef MLIB_H
#define MLIB_H


struct matrix{
	int rows, cols;
	int* arr;
};

struct multVector{
	int tag, size;
	int* arr;
};

//Send is what keeps track of what vectors to send where
struct send{
	int pSize, aSize;
	int* arr;
	int* procs;
};

void printMatrix(struct matrix* A){
	int i, j;
	for(i = 0; i < A->rows; i++){
		for(j = 0; j < A->cols; j++){
			printf("%d ", A->arr[i*A->cols+j]);
		}
		puts("");
	}
	printf("\n");
}

void initMatrix(struct matrix* A, int r, int c){
	A->rows = r;
	A->cols = c;
	A->arr = malloc(r*c*sizeof(int));
        int i, j;
        for(i = 0; i < r*c; i++)
		A->arr[i] = rand() % 100 + 1;
}

//converts an element index in a sequential "2D array" to an I,J index 
//and stores it in index* (a 2 element array)
void indexIJ(int cols, int element, int* index){
	index[0] = element / cols; // I
	index[1] = element % cols; // J
}

void matrixMult(struct matrix* A, struct  matrix* B){
	MPI_Comm world = MPI_COMM_WORLD;
	
	int nprocs, me;
	MPI_Comm_size(world, &nprocs);
	MPI_Comm_rank(world, &me);

	if(me == 0){
		struct matrix C;
		initMatrix(&C, A->rows, B->cols);
		printMatrix(A);

		printMatrix(B);

		//"vectorize" the matrices
		struct send sendA[A->rows];
		for(int i = 0; i < A->rows; i++){
			sendA[i].aSize = A->cols;
			sendA[i].pSize = 0;
			sendA[i].arr = malloc(A->cols*sizeof(int));
			sendA[i].procs = malloc(nprocs*sizeof(int));
			for(int j = 0; j < A->cols; j++)
				sendA[i].arr[j] = A->arr[i*(A->cols)+j];
			for(int j = 0; j < nprocs; j++)
				sendA[i].procs[j] = -1;
		}

		struct send sendB[B->cols];
		for(int i = 0; i < B->cols; i++){
			sendB[i].aSize = B->rows;
			sendB[i].pSize = 0;
			sendB[i].arr = malloc(B->rows*sizeof(int));
			sendB[i].procs = malloc(nprocs*sizeof(int));
			for(int j = 0; j < B->rows; j++)
				sendB[i].arr[j] = B->arr[j*(B->cols)+i];
			for(int j = 0; j < nprocs; j++)
				sendB[i].procs[j] = -1;
		}
		// done vectorizing
		//
		// now to find what vectors to send where
		

		int sizeC = C.rows * C.cols;
		int ePerProc = sizeC/nprocs; //elements per process 
		int eRootProc= ePerProc + (sizeC % nprocs);//elements on the root process
		int index[2];
		for(int i = 1; i <= nprocs; i++){
			if(i != nprocs){
				int high, low;
				low = ePerProc * (i - 1);
				high = (ePerProc * i) - 1;
				if((high - low)+1 != ePerProc){
					printf("Element distribution error: %d\n", i);
					printf("Real: %d\tProjected: %d\n", high-low+1, ePerProc);
					exit(1);
				}
				for(int j = low; j <= high; j++){
					indexIJ(C.cols, j, index);
					short flag = 0;
					for(int k = 0; k < sendA[index[0]].pSize; k++)
						if(i == sendA[index[0]].procs[k])
							flag = 1;
					if(flag == 0){
						sendA[index[0]].pSize++;
						sendA[index[0]].procs[sendA[index[0]].pSize-1] = i;
					}
			
					flag = 0;
					for(int k = 0; k < sendB[index[1]].pSize; k++)
						if(i == sendB[index[1]].procs[k])
							flag = 1;
					if(flag == 0){
						sendB[index[1]].pSize++;
						sendB[index[1]].procs[sendB[index[1]].pSize-1] = i;
					}
				}

			} else {
				int high, low;
				low = ePerProc * (nprocs - 1);
				high = sizeC - 1;
				if((high - low)+1 != eRootProc){
					printf("Element distribution error: Root\n");
					printf("Real: %d\tProjected: %d\n", high-low+1, eRootProc);
					exit(1);
				}
				for(int j = low; j <= high; j++){
					indexIJ(C.cols, j, index);
					short flag = 0;
					for(int k = 0; k < sendA[index[0]].pSize; k++)
						if(0 == sendA[index[0]].procs[k])
							flag = 1;
					if(flag == 0){
						sendA[index[0]].pSize++;
						sendA[index[0]].procs[sendA[index[0]].pSize-1] = 0;
					}

					flag = 0;
					for(int k = 0; k < sendB[index[1]].pSize; k++)
						if(0 == sendB[index[1]].procs[k])
							flag = 1;
					if(flag == 0){
						sendB[index[1]].pSize++;
						sendB[index[1]].procs[sendB[index[1]].pSize-1] = 0;
					}
				}
			}
		}
		
		for(int m = 0; m < A->rows; m++){
			printf("Vector %d A: ", m);
			for(int l = 0; l < sendA[m].pSize; l++)
				printf("%d ", sendA[m].procs[l]);
			printf("\n");
		}
		printf("\n\n");
		for(int m = 0; m < B->cols; m++){
			printf("Vector %d B: ", m);
			for(int l = 0; l < sendB[m].pSize; l++)
				printf("%d ", sendB[m].procs[l]);
			printf("\n");
		}
		

		//it is known which processes to send each vector to
		//...now actually send the vectors
		//the tag contains which row/column it was sent from
		//vectors from A will have tags between [0,nprocs-1], and ones
		//from B will have tags between [nprocs,2*nprocs]

		for(int i = 0; i < nprocs; i++){
			MPI_Send(&sendA[i].pSize, 1, MPI_INT, i, nprocs*3+1, world);
			MPI_Send(&sendB[i].pSize, 1, MPI_INT, i, nprocs*3+2, world);

		}

		for(int i = 0; i < A->rows; i++){
			for(int j = 0; j < sendA[i].pSize; j++){
				MPI_Send(sendA[i].arr, sendA[i].aSize, MPI_INT, sendA[i].procs[j], i, world);
			}
			for(int j = 0; j < sendB[i].pSize; j++){
				MPI_Send(sendB[i].arr, sendB[i].aSize, MPI_INT, sendB[i].procs[j], i+nprocs, world);
			}
		}
		//send a null message to show messages are finished being sent
		int nullMessage = 0;
		for(int i = 0; i < nprocs; i++)
			MPI_Send(&nullMessage, 1, MPI_INT, i, nprocs*3, world);
	}

	MPI_Status status;

	int numVectorsA, numVectorsB;
	MPI_Recv(&numVectorsA, 1, MPI_INT, 0, nprocs*3+1, world, MPI_STATUS_IGNORE);
	MPI_Recv(&numVectorsB, 1, MPI_INT, 0, nprocs*3+2, world, MPI_STATUS_IGNORE);


	struct multVector vectorsA[numVectorsA];
	struct multVector vectorsB[numVectorsB];
	
	numVectorsA = 0;
	numVectorsB = 0;

	short doneSending = 0;
	int vectorLength;
	while(doneSending == 0){
		MPI_Probe(0, MPI_ANY_TAG, world, &status);
		MPI_Get_count(&status, MPI_INT, &vectorLength);
		if(status.MPI_TAG == nprocs*3){
			doneSending = 1;
		} else {
			int* vector = malloc(vectorLength*sizeof(int));
			MPI_Recv(vector, vectorLength, MPI_INT, 0, MPI_ANY_TAG, world, MPI_STATUS_IGNORE);
			if(status.MPI_TAG - nprocs < 0){
				vectorsA[numVectorsA].arr = vector;
				vectorsA[numVectorsA].size = vectorLength;
				vectorsA[numVectorsA++].tag = status.MPI_TAG;
			} else {
				vectorsB[numVectorsB].arr = vector;
				vectorsB[numVectorsB].size = vectorLength;
				vectorsB[numVectorsB++].tag = status.MPI_TAG - nprocs;
			}
		}

	}

	if(me == 0)
		printf("%d\n", numVectorsB);
	else if(me == 2)
		printf("%d\n", numVectorsA);

	for(int i = 0; i < numVectorsA; i++){
		printf("[%d][A][%d] Vector %d: ", me, i, vectorsA[i].tag);
		for(int j = 0; j < vectorsA[i].size; j++){
			printf("%d ", vectorsA[i].arr[j]);
		}
		printf("\n");
	}


	for(int i = 0; i < numVectorsB; i++){
		printf("\t[%d][B][%d] Vector %d: ", me, i, vectorsB[i].tag);
		for(int j = 0; j < vectorsB[i].size; j++){
			printf("%d ", vectorsB[i].arr[j]);
		}
		printf("\n");
	}


}
#endif
