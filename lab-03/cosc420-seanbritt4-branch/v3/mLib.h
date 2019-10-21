#ifndef MLIB_H
#define MLIB_H

struct matrix{
	int rows, cols;
	int* arr;
};

void initMatrix(struct matrix* A, int r, int c){
	A->rows = r;
	A->cols = c;
	A->arr = malloc(r*c*sizeof(int));
	int i, j;
	for(i = 0; i < r*c; i++)
		// A->arr[i] = 1;
		A->arr[i] = rand() % 100 + 1;
		// A->arr[i] = rand() % 10 + 1;
}

void printMatrix(struct matrix* A){
	int i, j;
	for(i = 0; i < A->rows; i++){
		for(j = 0; j < A->cols; j++){
			printf("%5d ", A->arr[i*A->cols+j]);
		}
		puts("");
	}
	printf("\n");
}

struct matrix transpose(struct matrix* A){
	struct matrix T;
	initMatrix(&T, A->cols, A->rows);
	int i, j, count, index;
	count = 0;
	for(i = 0; i < A->cols; i++){
		for(j = 0; j < A->rows; j++){
			index = i + (A->cols * j);
			T.arr[count] = A->arr[index];
			count++;
		}
	}
	return T;
}

//helper function for matrixMult, multiplies a vector by a matrix, returns a vector
void vecMatrixMult(int* returnVec, int* vec, struct matrix* B){
	for(int i = 0; i < B->rows; i++){
		returnVec[i] = 0;
		for(int j = 0; j < B->cols; j++){
			returnVec[i] += vec[j] * B->arr[(i*B->cols)+j];
		}
	}
}

void matrixMult(struct matrix* A, struct matrix* B, struct matrix *C){
	if(A->rows != B->cols){
		printf("Matrices not of right size for multiplication!\n");
		return;
	}

	MPI_Comm world = MPI_COMM_WORLD;

	int nprocs, me;
	MPI_Comm_size(world, &nprocs);
	MPI_Comm_rank(world, &me);


	//process 0 init
	if(me == 0){
		*B = transpose(B);

		//printMatrix(A);
		//puts("");
		//printMatrix(B);
		//send B to procs
		for(int i = 1; i < nprocs; i++){
			MPI_Send(&B->rows, 1, MPI_INT, i, 2, world);
			MPI_Send(&B->cols, 1, MPI_INT, i, 2, world);
			MPI_Send(B->arr, B->rows*B->cols, MPI_INT, i, 2, world);
		}

		//send divisible rows to procs
		int divRows = A->rows / nprocs;
		for(int sendTo = 1; sendTo < nprocs; sendTo++){ //process to send to, do not need to send to root node
			MPI_Send(&divRows, 1, MPI_INT, sendTo, INT_MAX-1, world);
			for(int i = 0; i < divRows; i++){//row to send
				MPI_Send(&A->arr[A->cols*((sendTo*divRows)+i)], A->cols, MPI_INT, sendTo, 0, world);
			}
		}

		//send terminating message to signify that all divisible cols are sent
		for(int i = 1; i < nprocs; i++){
			MPI_Send(&i, 1, MPI_INT, i, INT_MAX, world);
		}

		//distribute remainder rows to procs (should be no more than one sent to each process)
		int remRows = A->rows % nprocs;
		if(remRows >= nprocs){
			printf("biggo error with distributing remaining rows\n");
			return;
		}
		for(int i = 1; i < remRows; i++){ // do not need to send to root node, hence starting at 1
			MPI_Send(&A->arr[A->cols*((A->rows-remRows)+i)], A->cols, MPI_INT, i, 1, world);
		}

		//send a second terminating message to signify that sending is 100% complete
		for(int i = 0; i < nprocs; i++)
			MPI_Send(&i, 1, MPI_INT, i, INT_MAX-2, world);

		//calculate 0's part of the matrix
		int* vector = malloc(A->rows*sizeof(int));
		for(int i = 0; i < divRows; i++){
			vecMatrixMult(vector, &A->arr[A->cols*i], B);
			for(int j = 0; j < A->rows; j++)
				C->arr[(i*A->rows) + j] = vector[j];
		}

		//printMatrix(&C);

		//receive the rest of the rows from procs
		MPI_Status status;
		for(int i = 1; i < nprocs; i++)
			for(int j = 0; j < divRows; j++){
				MPI_Recv(vector, A->rows, MPI_INT, i, 0, world, MPI_STATUS_IGNORE);
				for(int k = 0; k < A->rows; k++){
					C->arr[((i*divRows+j)*A->rows) + k] = vector[k];

				}
			}

		//calculate 0's remainder part of the matrix
		if(remRows != 0){
			vecMatrixMult(vector, &A->arr[A->cols*(A->rows - remRows)], B);
			for(int j = 0; j < A->rows; j++)
				C->arr[(A->rows-remRows)*A->rows+ j] = vector[j];
		}

		for(int i = 1; i < remRows; i++){
			MPI_Recv(vector, A->rows, MPI_INT, i, 1, world, MPI_STATUS_IGNORE);
			for(int j = 0; j < A->rows; j++){
				C->arr[((A->rows-remRows+i)*A->rows + j)] = vector[j];
			}
		}

	} else { //end proccess 0 init

		MPI_Status status;

		//receive B
		int rowsB, colsB;
		MPI_Recv(&rowsB, 1, MPI_INT, 0, 2, world, MPI_STATUS_IGNORE);
		MPI_Recv(&colsB, 1, MPI_INT, 0, 2, world, MPI_STATUS_IGNORE);
		struct matrix BT;
		initMatrix(&BT, rowsB, colsB);
		MPI_Recv(&BT.arr[0], rowsB*colsB, MPI_INT, 0, 2, world, MPI_STATUS_IGNORE);

		int* returnVec = malloc(BT.rows * sizeof(int));

		//length of vectors
		int vecLength = B->rows;
		int* vector = malloc(vecLength * sizeof(int));

		//get number of vectors
		int numVectors;
		MPI_Probe(0, MPI_ANY_TAG, world, &status);

		short flag = 0;
		if(status.MPI_TAG == INT_MAX){ // in the case it only receives a remainder
			MPI_Recv(&numVectors, 1, MPI_INT, 0, INT_MAX, world, MPI_STATUS_IGNORE); //receive to stop from blocking
			numVectors = 1;
			flag = 1;

		} else {
			MPI_Recv(&numVectors, 1, MPI_INT, 0, INT_MAX-1, world, MPI_STATUS_IGNORE);

			for(int i = 0; i < numVectors; i++){
				MPI_Recv(vector, vecLength, MPI_INT, 0, 0, world, MPI_STATUS_IGNORE);
				vecMatrixMult(returnVec, vector, &BT);
				MPI_Send(returnVec, BT.rows, MPI_INT, 0, 0, world);
			}

		}

		if(flag == 0){
			MPI_Recv(&numVectors, 1, MPI_INT, 0, INT_MAX, world, MPI_STATUS_IGNORE); // receive if haven't already done so above
			flag = 0;
		}
		MPI_Probe(0, MPI_ANY_TAG, world, &status);
		if(status.MPI_TAG == INT_MAX-2){
			MPI_Recv(&numVectors, 1, MPI_INT, 0, INT_MAX-2, world, MPI_STATUS_IGNORE); //receie to stop from blocking
			flag = 1;
		} else {
			MPI_Recv(vector, vecLength, MPI_INT, 0, MPI_ANY_TAG, world, MPI_STATUS_IGNORE);
			vecMatrixMult(returnVec, vector, &BT);
			MPI_Send(returnVec, BT.rows, MPI_INT, 0, 1, world);
		}
		if(flag == 0)
			MPI_Recv(&numVectors, 1, MPI_INT, 0, INT_MAX-2, world, MPI_STATUS_IGNORE); //receive if haven't already done so (for the second terminating message)

		//send terminating message signifying that all the returnVectors were sent
		MPI_Send(&numVectors, 1, MPI_INT, 0, INT_MAX, world);
	}
}

void matrixAdd(struct matrix* A, struct matrix * B, struct matrix *C){
	if((A->rows != B->rows) ||  (A->cols != B->cols)){
		printf("Matrices must be of the same size-- cannot add these matrices.\n");
		return;
	}

	MPI_Comm world = MPI_COMM_WORLD;
	int rank, nprocs, size, elem_per_proc, elem_remain;
	MPI_Comm_rank(world, & rank);
	MPI_Comm_size(world, &nprocs);
	size = A->rows * A->cols;
	elem_per_proc = size / nprocs;
	elem_remain = size % nprocs;

	int i,j, index;

	if(rank==0){
		int* send_div = malloc(elem_per_proc * 2 * sizeof(int));
		int* send_rem = malloc(2 * sizeof(int));
		//root divis sums
		for(i=0; i<elem_per_proc; i++){	//divisible loop on root
			C->arr[i] = A->arr[i] + B->arr[i];
		}
		//root remaining sum, will only ever be one
		C->arr[elem_per_proc * nprocs] = A->arr[elem_per_proc * nprocs] + B->arr[elem_per_proc * nprocs];

		//distribution loop- send divis array to corresponding proc
		for(i=1; i<nprocs; i++){
			for(j=0; j<elem_per_proc; j++){
				index = j+(i*elem_per_proc);
				send_div[j] = A->arr[index];
				send_div[j+elem_per_proc] = B->arr[index];
			}

			//send vectors to procs
			MPI_Send(send_div, elem_per_proc * 2, MPI_INT, i, 0, world);

			//send remaining elements to proccesses
			if(i<elem_remain){
				index = (elem_per_proc * nprocs) + i;
				send_rem[0] = A->arr[index];
				send_rem[1] = B->arr[index];
				MPI_Send(send_rem, 2, MPI_INT, i, 0, world);
			}

		}//--end distr. loop

		//signal finished sending
		for(i=1; i<nprocs; i++){
			MPI_Send(&i, 1, MPI_INT, i, 69, world);
		}

		//recv sums
		int *divis_arr, *remain_sum;
		divis_arr = malloc(elem_per_proc * sizeof(int));
		remain_sum = malloc(sizeof(int));

		MPI_Status status;
		// MPI_Probe(0, MPI_ANY_TAG, world, &status);
		// if(status.MPI_TAG == INT_MAX) printf("recv'd probe\n");
		int var = elem_per_proc;
		for(i=1; i<nprocs; i++){
			//recv processes vector of sums
			MPI_Recv(divis_arr, elem_per_proc, MPI_INT, i, 1, world, MPI_STATUS_IGNORE);
			for(j=0; j<elem_per_proc; j++){	//store divis. sums in C
				C->arr[var++]= divis_arr[j];
			}

			if(i < elem_remain){
				//recv processes remaining sum
				MPI_Recv(remain_sum, 1, MPI_INT, i, 1, world, MPI_STATUS_IGNORE);
				for(j=0; j<elem_remain; j++){		//store remain sums in C
					index = (elem_per_proc * nprocs) + i;
					C->arr[index] = remain_sum[0];
				}
			}
		}

		// printf("C:\n");
		// printMatrix(&C);

	}else{	//begin recv
		int arr_size = elem_per_proc*2;
		int* local_divis = malloc(sizeof(int) * arr_size);
		int* send_sums = malloc(sizeof(int) * elem_per_proc);

		//recv divisible vectors
		MPI_Recv(local_divis, arr_size, MPI_INT, 0, 0, world, MPI_STATUS_IGNORE);

		int a, b;
		for(i=0; i<elem_per_proc; i++){
			a = local_divis[i];
			b = local_divis[i + elem_per_proc];
			send_sums[i] = a+b;
		}

		//recv remaining elements if any
		int send_rem;
		if(rank<elem_remain){
			// printf("recving rem\n");
			int* local_rem = malloc(sizeof(int) * 2);
			MPI_Recv(local_rem, 2, MPI_INT, 0, 0, world, MPI_STATUS_IGNORE);
			send_rem = local_rem[0] + local_rem[1];
		}

		//send divis
		MPI_Send(send_sums, elem_per_proc, MPI_INT, 0, 1, world);

		//send sum of remaining elements if any
		if(rank<elem_remain){
			MPI_Send(&send_rem, 1, MPI_INT, 0, 1, world);
		}


	}

	free(C->arr);

} //--end matrixAdd()

void matrixSub(struct matrix *A, struct matrix *B, struct matrix *C){
	if((A->rows != B->rows) ||  (A->cols != B->cols)){
		printf("Matrices must be of the same size-- cannot subtract these matrices.\n");
		return;
	}

	MPI_Comm world = MPI_COMM_WORLD;
	int rank, nprocs, size, elem_per_proc, elem_remain;
	MPI_Comm_rank(world, & rank);
	MPI_Comm_size(world, &nprocs);
	size = A->rows * A->cols;
	elem_per_proc = size / nprocs;
	elem_remain = size % nprocs;

	// struct matrix C;

	int i,j, index;

	if(rank==0){
		// initMatrix(&C, A->rows, A->cols);

		int* send_div = malloc(elem_per_proc * 2 * sizeof(int));
		int* send_rem = malloc(2 * sizeof(int));
		//root divis sums
		for(i=0; i<elem_per_proc; i++){	//divisible loop on root
			C->arr[i] = A->arr[i] - B->arr[i];
		}
		//root remaining sum, will only ever be one
		C->arr[elem_per_proc * nprocs] = A->arr[elem_per_proc * nprocs] - B->arr[elem_per_proc * nprocs];

		//distribution loop- send divis array to corresponding proc
		for(i=1; i<nprocs; i++){
			for(j=0; j<elem_per_proc; j++){
				index = j+(i*elem_per_proc);
				send_div[j] = A->arr[index];
				send_div[j+elem_per_proc] = B->arr[index];
			}

			//send vectors to procs
			MPI_Send(send_div, elem_per_proc * 2, MPI_INT, i, 0, world);

			//send remaining elements to proccesses
			if(i<elem_remain){
				index = (elem_per_proc * nprocs) + i;
				send_rem[0] = A->arr[index];
				send_rem[1] = B->arr[index];
				MPI_Send(send_rem, 2, MPI_INT, i, 0, world);
			}

		}//--end distr. loop

		//signal finished sending
		for(i=1; i<nprocs; i++){
			MPI_Send(&i, 1, MPI_INT, i, 69, world);
		}

		//recv sums
		int *divis_arr, *remain_sum;
		divis_arr = malloc(elem_per_proc * sizeof(int));
		remain_sum = malloc(sizeof(int));

		MPI_Status status;
		// MPI_Probe(0, MPI_ANY_TAG, world, &status);
		// if(status.MPI_TAG == INT_MAX) printf("recv'd probe\n");
		int var = elem_per_proc;
		for(i=1; i<nprocs; i++){
			//recv processes vector of sums
			MPI_Recv(divis_arr, elem_per_proc, MPI_INT, i, 1, world, MPI_STATUS_IGNORE);
			for(j=0; j<elem_per_proc; j++){	//store divis. sums in C
				C->arr[var++]= divis_arr[j];
			}

			if(i < elem_remain){
				//recv processes remaining sum
				MPI_Recv(remain_sum, 1, MPI_INT, i, 1, world, MPI_STATUS_IGNORE);
				for(j=0; j<elem_remain; j++){		//store remain sums in C
					index = (elem_per_proc * nprocs) + i;
					C->arr[index] = remain_sum[0];
				}
			}
		}

	}else{	//begin recv
		int arr_size = elem_per_proc*2;
		int* local_divis = malloc(sizeof(int) * arr_size);
		int* send_sums = malloc(sizeof(int) * elem_per_proc);

		//recv divisible vectors
		MPI_Recv(local_divis, arr_size, MPI_INT, 0, 0, world, MPI_STATUS_IGNORE);

		int a, b;
		for(i=0; i<elem_per_proc; i++){
			a = local_divis[i];
			b = local_divis[i + elem_per_proc];
			send_sums[i] = a-b;
		}

		//recv remaining elements if any
		int send_rem;
		if(rank<elem_remain){
			// printf("recving rem\n");
			int* local_rem = malloc(sizeof(int) * 2);
			MPI_Recv(local_rem, 2, MPI_INT, 0, 0, world, MPI_STATUS_IGNORE);
			send_rem = local_rem[0] - local_rem[1];
		}

		//send divis
		MPI_Send(send_sums, elem_per_proc, MPI_INT, 0, 1, world);

		//send sum of remaining elements if any
		if(rank<elem_remain){
			MPI_Send(&send_rem, 1, MPI_INT, 0, 1, world);
		}
	}

	free(C->arr);

} //--end matrixAdd()

#endif
