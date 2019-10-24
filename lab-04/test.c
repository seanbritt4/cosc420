#include<stdio.h>
#include<stdlib.h>

int main(int argc, char** argv){

  int a = atoi(argv[1]);
  int* arr = malloc(sizeof(int) * a);
  for(int i=0; i<a; i++){
    arr[i] = i;
  }

  for(int i=0; i<a; i++){
    printf("%d\n", arr[i]);
  }

  return 0;
}
