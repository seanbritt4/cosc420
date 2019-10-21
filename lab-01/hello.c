#include<stdio.h>
#include<mpi.h>

// 
// int main(){
//   printf("Hello World!\n");
//   return 0;
// }

/*Compile with MPI library
 * #include<mpi.h>
 * Use 'mpicc or gcc -I/path/to/mpi/h'
 * 
 * MUST CALL:
 * 	MPI_Init(int *argc. int ***argc);
 * 		may use: MPI_Init(NULL, NULL);
 * 	MPI_Finalize();
 */

int main(int argc, char** argv){
  MPI_Init(NULL, NULL);
//   printf("num args: %d\n", argc);
  
  int world_size;
  MPI_Comm world = MPI_COMM_WORLD;
  MPI_Comm_size(world, &world_size);
  
  int me;
  MPI_Comm_rank(world, &me);
  
  char name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(name, &name_len);
  
  printf("Hello, world of size: %d\n", world_size);
  printf("I am processor: %s\n", name);
  printf("I am node: %d\n", me);
  
  
  MPI_Finalize();
  return 0;
}