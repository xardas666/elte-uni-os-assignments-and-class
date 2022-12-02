#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

int UZENET_MAX_SIZE = 1024;

struct uzenet {
    long mtype;
    char mtext[1024];
};

int kuld(int uzenetsor, long mtype, char* mtext) {
    struct uzenet uz;
    strncpy(uz.mtext, mtext,  UZENET_MAX_SIZE);
    uz.mtype=mtype;
    int status = msgsnd(uzenetsor, &uz, strlen(uz.mtext) + 1, 0);
    if (status < 0) {
        perror("msgsnd");
    }
    printf( "ELKULDVE\n" );
    return 0;
}

char* fogad(int uzenetsor, long mtype) {
    struct uzenet uz;
    int status = msgrcv(uzenetsor, &uz, UZENET_MAX_SIZE, mtype, 0);
    if (status < 0) { perror("msgsnd"); }
    char *str = malloc (sizeof (char) * UZENET_MAX_SIZE);
    strncpy(str, uz.mtext, UZENET_MAX_SIZE);
    printf( "FOGADVA \n" );
    return str;
}

int main(int argc, char *argv[]) {
    pid_t child;
    int uzenetsor;
    int status;
    key_t kulcs = ftok(argv[0], 1);

    uzenetsor = msgget(kulcs, 0600 | IPC_CREAT);

    if (uzenetsor < 0) {
        perror("msgget");
        return 1;
    }

    child = fork();

    if (child > 0) {
        kuld(uzenetsor, 5, "Hajra Fradi!");
        wait(NULL);

        status = msgctl(uzenetsor, IPC_RMID, NULL);
        if (status < 0){
            perror("msgctl");
        }
        return 0;
    } else if (child == 0) {
        char* buff= fogad(uzenetsor,5);
        printf( "A kapott uzenet:  %s\n", buff );
        return 0;
    } else {
        perror("fork");
        return 1;
    }

    return 0;
} 
