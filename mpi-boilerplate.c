//inclass Allreduce example

#include<stdio.h>	//puts, printf
#include<stdlib.h>	//rand
#include<time.h>	//time
#include<mpi.h>		//mpi

int main(int argc, char** argv){
    MPI_Init(NULL, NULL);
    
    MPI_Comm world = MPI_COMM_WORLD;
    int rank, world_size;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &world_size);
    
     
    
    MPI_Finalize();
    return 0;
}
