#include<stdio.h>
#include<mpi.h>

int main(int argc, char** argv){
    MPI_Init(NULL, NULL);
    
    int world_size;
    MPI_Comm world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &world_size);
    
    int me;
    MPI_Comm_rank(world, &me);
    
    char name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(name, &name_len);
    
    unsigned long N = atoi(argv[1]);
    
    int i;
    for(i = 0; i < N; i++){
      printf("me: %d\n", me);
      printf("world_size: %d\n", world_size);
      printf("processor: %d\n", name);
      printf("n: %lu\n", N);
      printf("i: %d\n\n", i);
    }  
    
      
    MPI_Finalize();
    return 0;
}

/*
 *6. Write a second program to perform a “primality” test of a given number, N, an unsigned long integer:
  (a) Use brute force, but you need only test up to √N. Note you may need to use the compiler option
      -lm to link against the math libraries.
  (b) If the number is found to be composite, report one way to factor it.
  (c) Split the work across the available number of nodes, using the mpi interfaces.
  (d) Include some diagnostic messages to display which nodes are testing which ranges of factors, and
      other relevant information you find helpful to be sure the program is working appropriately. 
 */	