#include <stdio.h>
#include <stdlib.h>
#include<mpi.h>

int main(int argc, char** argv){
    MPI_Init(NULL, NULL);
    int rank, nprocs;
    MPI_Comm world = MPI_COMM_WORLD;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &nprocs);

    printf("%d %d\n", rank, nprocs);

    MPI_Finalize();
    return 0;
}
