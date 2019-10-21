#define _XOPEN_SOURCE
#include<unistd.h>
#include<stdio.h>

int main(int argc, char** argv){
  if(argc < 2){
    printf("Useage: ./[executable] [string]\n");
    return 1;
  }

    char* salt = "$5$cat";
    char* cyphertext = crypt(argv[1], cyphertext);
    printf("Encrypted password %s\n", cyphertext);

    return 0;
}
