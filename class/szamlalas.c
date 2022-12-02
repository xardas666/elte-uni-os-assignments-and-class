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
#include <signal.h>

int terulet_adatok_regi[7] = {
        7, 6, 5, 4, 3, 2, 1
};
int terulet_adatok_uj[7] = {
        75, 56, 78, 99, 81, 64, 93
};

char *osszes_telepules[] = {
        "Baratfa",      //0
        "Lovas",        //1
        "Szula",        //2
        "Kigyos-patak", //3
        "Malom telek",  //4
        "Paskom",       //5
        "Kaposztakert",//6
};

int randomL(int lower_, int upper_);

static int received = 0;

int tapsi_started = 0;
int hapsi_started = 0;

void readTapsi(int sig) {
    if (sig == SIGUSR1) {
        tapsi_started = 1;
    }
}

void readHapsi(int sig) {
    if (sig == SIGUSR2) {
        hapsi_started = 1;
    }
}

int main() {
    srand(time(NULL));

    signal(SIGUSR1, readTapsi);
    signal(SIGUSR2, readHapsi);

    int tapsi_cso[2]; // Used to store two ends of first pipe
    int fules_cso[2]; // Used to store two ends of second pipe

    if (pipe(tapsi_cso) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fules_cso) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    pid_t p = fork();

    if (p < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    } else if (p > 0) {
        // Main process

        while (tapsi_started == 0 || hapsi_started == 0);

        char buf[100];
        for (int i = 0; i < 7; ++i) {
            if (i < 4) {
                write(tapsi_cso[1], osszes_telepules[i], strlen(osszes_telepules[i]) + 1);
                sleep(1);
            } else {
                write(fules_cso[1], osszes_telepules[i], strlen(osszes_telepules[i]) + 1);
                sleep(1);
            }
        }
        for (int v = 0; v < 7; ++v) {
            terulet_adatok_regi[v] = randomL(50, 100);
        }

        int h = 0;
        int t = 0;
        char buf1[100];
        char buf2[100];
        while (h+t < 6) {
            read(tapsi_cso[0], buf1, 100);
            read(fules_cso[0], buf2, 100);
            if (strlen(buf1) > 0) {
                terulet_adatok_uj[t] = atoi(buf1);
                t++;
            }
            if (strlen(buf2) > 0) {
                terulet_adatok_uj[h + 3] = atoi(buf2);
                h++;
            }

        }

        printf("Eredmenyek:\n");

        for (int j = 0; j < 7; ++j) {
            if (terulet_adatok_regi[j] > terulet_adatok_uj[j]) {
                printf("--%s \t%d \t%d -- csokkent\n", osszes_telepules[j], terulet_adatok_regi[j],
                       terulet_adatok_uj[j]);
            } else if (terulet_adatok_regi[j] < terulet_adatok_uj[j]) {
                printf("--%s \t%d \t%d -- nott\n", osszes_telepules[j], terulet_adatok_regi[j],
                       terulet_adatok_uj[j]);
            } else{
                printf("--%s \t%d \t%d -- ugyanaz\n", osszes_telepules[j], terulet_adatok_regi[j],
                       terulet_adatok_uj[j]);
            }
        }

        while (wait(NULL) > 0);//várunk mindenkire hogy befejezzék a futást
        printf("Tapsi és hapsi is nyugovóra tért, Főnyuszi is leáll.\n");

    } else {
        // child process
        char buf[100];

        pid_t p1 = fork();
        if (p1 == p) {
            kill(p, SIGUSR1);
            int i = 0;
            while (i < 4) {
                read(tapsi_cso[0], buf, 100);
                printf("Tapsi -> %s\n", buf);
                if (buf)i++;
            }
            sleep(2);

            for (int l = 0; l < 4; ++l) {
                int random = randomL(50, 100);
                snprintf(buf, 100, "%d", random);
                write(tapsi_cso[1], buf, strlen(buf) + 1);
                sleep(2);
            }
        } else {
            kill(p, SIGUSR2);
            int i = 0;
            while (i < 3) {
                read(fules_cso[0], buf, 100);
                printf("Hapsi -> %s\n", buf);
                if (buf)i++;
            }
            sleep(2);

            for (int z = 0; z < 3; ++z) {
                int random = randomL(50, 100);
                snprintf(buf, 100, "%d", random);
                write(fules_cso[1], buf, strlen(buf) + 1);
                sleep(2);
            }
        }

        exit(0);
    }


    return 0;
}

int randomL(int lower_, int upper_) {
    return (rand() % (upper_ - lower_ + 1)) + lower_;
}