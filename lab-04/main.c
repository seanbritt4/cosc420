#include <stdio.h>
#include <stdlib.h>
#include<mpi.h>

typedef struct User{
    /*all of the following will be assigned as the shadow file is parsed*/
    char* username,
    alg_id,   //encryption algorithm id; expected to always be '5' in this lab
    salt,     //encryption salt; expected to always be 'ab' in this lab
    epwd,     //encrypted password
    dpwd;     //decrypted password

    /*after shadow file is fully parsed, cast alg_id to int-- may not need alg_id at all*/
    int alg;  //alg_id as an integer
}User;

void initUser(User* u, char** data, int* str_len){
    for(int i=0; i<4; i++){
        printf("\n%2d ", str_len[i]);
        for(int j=0; j<str_len[i]; j++){
            printf("%c", data[i][j]);
        }
    }
    puts("");
    // u->username = malloc(sizeof(char) * 15  /*temp size, may want to change*/);
    // u->alg_id = malloc(sizeof(char) * 1);
    // u->salt = malloc(sizeof(char) * 2);
    // u->epwd = malloc(sizeof(char) * 15  /*temp size, may want to change*/);
    // u->dpwd = malloc(sizeof(char) * 15  /*temp size, may want to change*/);

    // u->username = data[0];
    // u->alg_id = data[1];
    // u->salt = data[2];
    // u->epwd = data[3];
}

void parse(){
    FILE* src = fopen("shadow", "r");
    if(src ==  NULL){
        printf("Error: unable to open file\n");
        exit(1);
    }

    //array of all users in shadow file
    User** users = malloc(sizeof(User*) * 11); // can hardcode 11 for this lab only

    //array of data to assign to each User in users
    char** user_data = malloc(sizeof(char*) * 4);  //4 bc each entry in shadow file has 4 sections
                                                    //  username, alg_id, salt, encrypted password
    // malloc room for data read
    user_data[0] = malloc(sizeof(char) * 16);
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
            initUser(users[user], user_data, str_len);
            user++;
            //reset counting vars
            // char_count=0;
            data_cat=0;
            for(int i=0; i<4; i++) str_len[i] = 0;

        }else{ //read into current array
            //assignment is done here
            user_data[data_cat][str_len[data_cat]++] = buf[0];
        }
    }
}


int main(int argc, char** argv){
    MPI_Init(NULL, NULL);
    int rank, nprocs;
    MPI_Comm world = MPI_COMM_WORLD;
    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &nprocs);

    if(rank==0){
        parse();
        /*
            decrypt pwds
        */
    }


MPI_Finalize();
return 0;
}
