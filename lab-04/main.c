#include <stdio.h>
#include <stdlib.h>
#include<mpi.h>
#include<string.h>
#include "users.h"

/* parses shadowfile and creates user structs for each user in the file
    returns array of initialized User pointers */
void parse(User** users){
    // FILE* src = fopen("testshadow", "r");
    FILE* src = fopen("shadow", "r");
    if(src ==  NULL){
        printf("Error: unable to open file\n");
        exit(1);
    }

    //array of data to assign to each User in users
    char** user_data = malloc(sizeof(char*) * 4);  //4 bc each entry in shadow file has 4 sections
                                                    //  username, alg_id, salt, encrypted password
    // malloc room for data read
    user_data[0] = malloc(sizeof(char) * 10);
    user_data[1] = malloc(sizeof(char) * 1);
    user_data[2] = malloc(sizeof(char) * 2);
    user_data[3] = malloc(sizeof(char) * 43);

    char buf[1];
    int data_cat, // category of data being read (username, alg_id, salt, pwd)
        user,     // new line count,
        nread;    //number of chars read from file, should be either 1 or 0

    int str_len[4] = {0,0,0,0};
    data_cat = 0;
    user = 0;
    while(nread=fread(buf, 1, sizeof buf, src) > 0){
        if(buf[0] == '$'){ //"$", move to next element in temp user data array
            data_cat++;
        }else if(buf[0] == ':'){ //':', do nothing
        }else if(buf[0] == '\n'){ //'\n', begin parsing for new user
            /* debugging stuff
            if(users[user]->username == NULL){
                printf("null\n");
            }else{
                printf("1%s\n",users[user]->username);
                users[user]->username = malloc(sizeof(char)*10);
                strcpy(users[user]->username,user_data[0]);
                printf("2%s\n",users[user]->username);
            }*/

            // initUser(users[user]);
            assignUserInfo(users[user], user_data, str_len);
            user++;
            //reset counting vars
            data_cat=0;
            for(int i=0; i<4; i++) str_len[i] = 0;

        }else{ //read into current array
            //assignment is done here
            user_data[data_cat][str_len[data_cat]++] = buf[0];
        }
    }

    free(user_data[0]);
    free(user_data[1]);
    free(user_data[2]);
    free(user_data[3]);
    free(user_data);
}


int main(int argc, char** argv){
    MPI_Init(NULL, NULL);
    int rank, nprocs;
    MPI_Comm world = MPI_COMM_WORLD;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &nprocs);

    if(rank==0){
        User** users = malloc(sizeof(User*) * 11);
        for(int i=0; i<11; i++){
            User temp;
            users[i] = &temp;
            initUser(users[i]);
        }
        parse(users);

        for(int i=0; i<11; i++){
            printUserInfo(users[i]);
            puts("");
        }

        /* decrypt pwds
        */

        //free User info
        // for(int i=0; i<11; i++){
        //     deleteUser(users[i]);
        // }
        // free(users);
    }

    MPI_Finalize();
    return 0;
}
