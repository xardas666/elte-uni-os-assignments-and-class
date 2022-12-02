//
// Created by istvan.gottfried on 2022. 05. 12..
//
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

int UZENET_MAX_SIZE = 1024;
int TAPSI_M_TYPE=1;
int HAPSI_M_TYPE=2;
struct uzenet {long mtype; char mtext[1024];};
int terulet_adatok_regi[7] = {7, 6, 5, 4, 3, 2, 1};
int terulet_adatok_uj[7] = {0, 0, 0, 0, 0, 0, 0};
char *osszes_telepules[] = {
        "Baratfa",      //0
        "Lovas",        //1
        "Szula",        //2
        "Kigyos-patak", //3
        "Malom telek",  //4
        "Paskom",       //5
        "Kaposztas kert",//6
};

int randomL(int lower_, int upper_);
int kuld(int uzenetsor, long mtype, char* mtext);
char* fogad(int uzenetsor, long mtype);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    key_t kulcs = ftok(argv[0], 1);

    int uzenetsor = msgget(kulcs, 0600 | IPC_CREAT);

    if (uzenetsor < 0) {
        perror("msgget");
        return 1;
    }

    pid_t p = fork();

    if (p < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    } else if (p > 0) {
        // Main process
        char buf[100];
        for (int i = 0; i < 7; ++i) {
            if (i < 4) {
                kuld(uzenetsor, TAPSI_M_TYPE, osszes_telepules[i]);
                //write(tapsi_cso[1], osszes_telepules[i], strlen(osszes_telepules[i]) + 1);
                //sleep(1);
            } else {
                kuld(uzenetsor, HAPSI_M_TYPE, osszes_telepules[i]);
                //write(fules_cso[1], osszes_telepules[i], strlen(osszes_telepules[i]) + 1);
                //sleep(1);
            }
        }

        while (wait(NULL) > 0);//várunk mindenkire hogy befejezzék a futást

        int status = msgctl(uzenetsor, IPC_RMID, NULL);
        if (status < 0){
            perror("msgctl");
        }

        printf("Tapsi és hapsi is nyugovóra tért, Főnyuszi is leáll.\n");

    } else {
        // child process
        pid_t p1 = fork();
        if (p1 == p) {
            int i = 0;
            while (i < 4) {
                char* buff= fogad(uzenetsor,TAPSI_M_TYPE);
                printf("Tapsi -> %s\n", buff);
                if (buff)i++;
            }

        } else {
            int i = 0;
            while (i < 3) {
                char* buff= fogad(uzenetsor,HAPSI_M_TYPE);
                printf("Hapsi -> %s\n", buff);
                if (buff)i++;
            }
            while (wait(NULL) > 0);
        }
        exit(0);
    }
    return 0;
}

int randomL(int lower_, int upper_) {
    return (rand() % (upper_ - lower_ + 1)) + lower_;
}

int kuld(int uzenetsor, long mtype, char* mtext) {
    struct uzenet uz;
    strncpy(uz.mtext, mtext,  UZENET_MAX_SIZE);
    uz.mtype=mtype;
    int status = msgsnd(uzenetsor, &uz, strlen(uz.mtext) + 1, 0);
    if (status < 0) {
        perror("msgsnd");
    }
    //printf( "ELKULDVE\n" );
    return 0;
}

char* fogad(int uzenetsor, long mtype) {
    struct uzenet uz;
    int status = msgrcv(uzenetsor, &uz, UZENET_MAX_SIZE, mtype, 0);
    if (status < 0) { perror("msgsnd"); }
    char *str = malloc (sizeof (char) * UZENET_MAX_SIZE);
    strncpy(str, uz.mtext, UZENET_MAX_SIZE);
    //printf( "FOGADVA \n" );
    return str;
}