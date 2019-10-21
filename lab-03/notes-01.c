/*
TILING MATRIX MULTIPLICATION
for kk =1..n/bs
  for i =1..n
    for j=1..n
      s=0
      fpr k=(kk-1)*bs+1..kk*bs
        s += a[i,k]*b[k,j]
      c[i,j] += s

*/

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int main(int argc, char** argv){
  MPI_Init(NULL, NULL);

  int nprocs, me;
  MPI_Comm world = MPI_COMM_WORLD ;
  MPI_Comm_rank(world, &nprocs);
  MPI_Comm_size(world, &me);


  MPI_Finalize();
  return 0;
}
