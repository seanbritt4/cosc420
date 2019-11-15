// #ifndef VECTOR_H
// #define VECTOR_H
//
// // #include"mLib.h"
//
// //may come back to this...
// // struct vector{
// // 	int rows, cols;
// // 	int* arr;
// // }
// //
// // void initVector(int n, bool column_vector){
// //
// // }
//
// /*
// . Implement a first-pass attempt to calculate the eigenvector corresponding to the largest eigenvector of
// a matrix. You may use the naıve power-method discussed in lecture:
// (a) Let x be an all 1 vector in the appropriate dimension
// (b) Perform update: x ← Ax
// (c) Normalize: x ← x/kxk2
// .
// (d) From this, x will converge quite quickly to the eigenvector corresponding to the largest eigenvalue.
// You may obtain an estimate of this eigenvalue by computing kAxk2
// /kxk2
// .*/
// void calcEigenvector(struct matrix* M){
// 	printf("in cE()\n");
// 	printMatrix(M);
// 	printf("rows: %d", M->rows);
// 	printf("cols: %d\n", M->cols);
//
// 	// struct matrix x;
// 	int* x = malloc(sizeof(int) * M->rows);
// 	for(int i=0; i<M->rows; i++){
// 		x[i] = 1;
// 	}
// 	// printMatrix(&x);
// 	// initMatrix(&x, M->rows, 1);
// 	// x.arr = temp_arr;
// 	// printMatrix(&x);
//
// 	float delta;
// 	float epsilon = .00001;
// 	while(delta > epsilon){
// 		matrixMult(&x, M, &x);
// 		printMatrix(&x);
// 		printMatrix(M);
// 		delta = 0.0;
// 	}
// }
//
// #endif
