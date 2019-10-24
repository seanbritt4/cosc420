#ifndef USERS_H
#define USERS_H

typedef struct User{
    /*all of the following will be assigned as the shadow file is parsed*/
    char *username,
         *alg_id,   //encryption algorithm id; expected to always be '5' in this lab
         *salt,     //encryption salt; expected to always be 'ab' in this lab
         *epwd,     //encrypted password
         *dpwd;     //decrypted password

    /*after shadow file is fully parsed, cast alg_id to int-- may not need alg_id at all*/
    int alg;  //alg_id as an integer
}User;

void initUser(User* u){
    u->username = malloc(sizeof(char) * 10);
    u->alg_id   = malloc(sizeof(char));
    u->salt     = malloc(sizeof(char) * 2);
    u->epwd     = malloc(sizeof(char) * 43);
    u->dpwd     = malloc(sizeof(char) * 43);
}

void printUserInfo(User* u){
    printf("username: %s\n", u->username);
    printf("alg_id: %s\n", u->alg_id);
    printf("salt: %s\n", u->salt);
    printf("epwd: %s\n", u->epwd);
    printf("dpwd: %s\n", u->dpwd);
    printf("alg: %d\n", u->alg);
}

void assignUserInfo(User* u, char** data, int* str_len){
    /*for(int i=0; i<4; i++){
        printf("\nstr_len: %2d, data[%d] ", str_len[i], i);
        for(int j=0; j<str_len[i]; j++){
            printf("%c", data[i][j]);
        }
    }
    puts("");
    */

    strcpy(u->username, data[0]);
    strcpy(u->alg_id, data[1]);
    strcpy(u->salt, data[2]);
    strcpy(u->epwd, data[3]);
    // strcpy(u->dpwd, data[4]);

    u->alg = atoi(u->alg_id);

    // printf("iU: %s\n", u->username);
}

void deleteUser(User* u){
    free(u->username);
    free(u->alg_id);
    free(u->salt);
    free(u->epwd);
    free(u->dpwd);
}

#endif
