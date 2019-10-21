#include <stdio.h>
#include <stdlib.h>
#include<mpi.h>

int main(int argc, char** argv){
    MPI_Init(NULL, NULL);
    int rank, nprocs;
    MPI_Comm world = MPI_COMM_WORLD;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &nprocs);

    if(rank==0){
        FILE* src = fopen("shadow", "r");
        if(src == NULL){
            printf("Error: unable to open file\n");
            exit(1);
        }

        char b;
        size_t nread;
        while(nread=fread(b, 1, sizeof b, src)) > 0){
            fwrite(b, 1, nread, stdout);
        }
    }


    MPI_Finalize();
    return 0;
}
