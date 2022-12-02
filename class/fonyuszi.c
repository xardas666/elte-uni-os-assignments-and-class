//
// Created by istvan.gottfried on 2022. 05. 09..
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "kozos.h"
#include <sys/types.h>
#include <sys/wait.h>

char *szampi_file = "szampi.txt";
char *balkk_file = "balkk.txt";
char *base_file = "input.txt";

char *valaszto_kerulet_balkk[] = {
        "Baratfa",
        "Lovas",
        "Kigyos-patak",
        "Kaposztas kert",
};

char *valaszto_kerulet_szampi[] = {
        "Szula",
        "Malom telek",
        "Paskom",
};

char *osszes_telepules[] = {
        "Baratfa",      //0
        "Lovas",        //1
        "Szula",        //2
        "Kigyos-patak", //3
        "Malom telek",  //4
        "Paskom",       //5
        "Kaposztas kert",//6
};
char *osszes_telepules_array[] = {
        "Barátfa",
        "Lovas",
        "Szula",
        "Kígyós-patak",
        "Malom telek",
        "Páskom",
        "Káposztás kert",
};

char **nev_array;
char **telepules_array;
int *szam_array;
int bejegyzes_szamok;

void main_menu();
void read_data_file(char *filename_);
char *getlineL(void);
void uj_felvetel();
void save_to_data_file(char *telepules_, char *nev_, int szam_);
void add_entry_to_arrays(char *telepules_, char *nev_, int szam_);
void telepulesek_listazasa();
void teruleti_lista();
void teljes_lista();
void modositas();
void torles();
void exit_program();
int randomL(int lower_, int upper_);
char *versenyzo_eloallitasa();
int szavazokerulet_szampi(char * versenyzo_);

int szimulacio() {
    srand(time(NULL));

    // open the file for writing
    FILE *fp1 = fopen(szampi_file, "a+");
    if (fp1 == NULL){
        return -1;
    }
    FILE *fp2 = fopen(balkk_file, "a+");
    if (fp2 == NULL){
        return -1;
    }

    int versenyzok_szama = randomL(50, 100);

    for (int i = 0; i < versenyzok_szama; ++i) {
        char * versenyzo = versenyzo_eloallitasa();
        if (szavazokerulet_szampi(versenyzo) == 1) {
            fprintf(fp1, "%s\n", versenyzo);
        } else {
            fprintf(fp2, "%s\n", versenyzo);
        }
    }

    fclose(fp1);
    fclose(fp2);

    int fd1[2]; // Used to store two ends of first pipe
    int fd2[2]; // Used to store two ends of second pipe

    if (pipe(fd1) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd2) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    write(fd1[1], szampi_file, strlen(szampi_file) + 1);
    write(fd2[1], balkk_file, strlen(balkk_file) + 1);

    pid_t p = fork();

    if (p < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    } else if (p > 0) { // Main process
        while(wait(NULL) > 0);//várunk mindenkire
        char buf[100];
        read(fd1[0], buf, 100);
        printf("Szampi nyertese: %s\n", buf);
        read(fd2[0], buf, 100);
        printf("Balkk nyertese: %s\n", buf);
    } else { // child process
        char buf[100];

        pid_t p1 = fork();
        if(p1==p){
            read(fd1[0], buf, 100);
        } else {
            read(fd2[0], buf, 100);
        }

        char eredmeny[100];
        //kozos rész

        //parse
        read_data_file(buf);
        //find legtöbb
        int max =0;
        int k = 0;
        for (int i = 0; i < bejegyzes_szamok; ++i)
        {
            if (szam_array[i] > max)
            {
                max = (int)szam_array[i];
                k = i;
            }
        }
        snprintf(eredmeny, 100, "%s, \t %s,\t %d\n", telepules_array[k], nev_array[k], szam_array[k]);

        //kozos vége

        if(p1==p){
            write(fd1[1], eredmeny, strlen(eredmeny) + 1);
        } else {
            write(fd2[1], eredmeny, strlen(eredmeny) + 1);
        }

        exit(0);
    }


    return 0;
}



int main() {
    main_menu();

    while (1) {
        int menu_input = strtol(getlineL(), NULL, 10);
        switch (menu_input) {
            case 11: //teruleti lista
                read_data_file(base_file);
                teruleti_lista();
                break;
            case 12: //teljes lista
                read_data_file(base_file);
                teljes_lista();
                break;
            case 21: //uj felvetel
                uj_felvetel();
                break;
            case 22: //modositas
                modositas();
                break;
            case 23: //torles
                torles();
                break;
            case 24: //szimulacio
                szimulacio();
                break;
            case 99: //kilepes
                exit_program();
            default:
                printf("Nem letezo menu!\n");
        }
    }
}

void modositas() {
    printf("Add meg az ugyfel nevet!\n");
    char *nev_ = getlineL();
    char *trimmed_end_nev = strtok(nev_, "\n");
    int ugyfel_index;

    for (int i = 0; i < bejegyzes_szamok - 1; i++) {
        if (strcmp(trimmed_end_nev, nev_array[i]) == 0) {
            ugyfel_index = i;
        }
    }

    telepulesek_listazasa();
    int varos_index = strtol(getlineL(), NULL, 10);
    while (varos_index < 0 || varos_index > 6) {
        printf("Hibas ertek 0 - 6 -ig vannak a varosok!\n");
        varos_index = strtol(getlineL(), NULL, 10);
    }
    printf("Add meg az ugyfel uj nevet!\n");
    char *nev = getlineL();

    printf("Add meg az ugyfel uj jelentkezesi szamat!\n");
    int jelentkezes_szama = strtol(getlineL(), NULL, 10);

    char *telepules = osszes_telepules_array[varos_index];

    telepules_array[ugyfel_index] = telepules;
    nev_array[ugyfel_index] = trimmed_end_nev;
    szam_array[ugyfel_index] = jelentkezes_szama;

    FILE *filePointer;
    filePointer = fopen(base_file, "w");

    for (int i = 0; i < bejegyzes_szamok; i++) {
        fprintf(filePointer, "%s,%s,%d\n", telepules_array[i], nev_array[i], szam_array[i]);
    }

    fclose(filePointer);

    printf("Az ugyfel (%s -> %s) módosítva!\n", nev_, trimmed_end_nev);
    exit(0);
}

void teljes_lista() {
    for (int i = 0; i < bejegyzes_szamok; i++) {
        printf("| %s, \t %s,\t %d\n", telepules_array[i], nev_array[i], szam_array[i]);
    }
    exit(0);
}

void teruleti_lista() {
    telepulesek_listazasa();
    int varos_index = strtol(getlineL(), NULL, 10);
    while (varos_index < 0 || varos_index > 6) {
        printf("Hibas ertek 0 - 6 -ig vannak a varosok!\n");
        varos_index = strtol(getlineL(), NULL, 10);
    }
    char *telepules = osszes_telepules_array[varos_index];
    printf("A teruleti(%s) lekerdezes eredmenye:\n", telepules);
    for (int i = 0; i < bejegyzes_szamok; i++) {
        if (strcmp(telepules, telepules_array[i]) == 0) {
            printf("| %s, %d\n", nev_array[i], szam_array[i]);
        }
    }
    exit(0);
}

void uj_felvetel() {
    telepulesek_listazasa();
    int varos_index = strtol(getlineL(), NULL, 10);
    while (varos_index < 0 || varos_index > 6) {
        printf("Hibas ertek 0 - 6 -ig vannak a varosok!\n");
        varos_index = strtol(getlineL(), NULL, 10);
    }
    printf("Add meg az ugyfel nevet!\n");
    char *nev = getlineL();

    printf("Add meg az ugyfel jelentkezesi szamat!\n");
    int jelentkezes_szama = strtol(getlineL(), NULL, 10);

    char *telepules = osszes_telepules_array[varos_index];
    save_to_data_file(telepules, nev, jelentkezes_szama);
}

void telepulesek_listazasa() {
    int i = 0;
    printf("Add meg a varos szamat!\n");
    while (osszes_telepules_array[i] && strlen(osszes_telepules_array[i]) != 0) {
        printf("%d - %s\n", i, osszes_telepules_array[i]);
        i++;
    }
}

void save_to_data_file(char *telepules_, char *nev_, int szam_) {
    char *trimmed_end_nev = strtok(nev_, "\n");

    FILE *filePointer;
    filePointer = fopen(base_file, "a+");
    fprintf(filePointer, "%s,%s,%d\n", telepules_, trimmed_end_nev, szam_);
    printf("%s, %s, %d adatok mentesre kerultek!\n", telepules_, trimmed_end_nev, szam_);
    fclose(filePointer);
    exit(0);
}

void torles() {
    printf("Add meg az ugyfel nevet!\n");
    char *nev_ = getlineL();
    char *trimmed_end_nev = strtok(nev_, "\n");
    for (int i = 0; i < bejegyzes_szamok - 1; i++) {
        if (strcmp(trimmed_end_nev, nev_array[i]) == 0) {
            for (int j = i; j < bejegyzes_szamok - 1; j++) {
                telepules_array[j] = telepules_array[j + 1];
                nev_array[j] = nev_array[j + 1];
                szam_array[j] = szam_array[j + 1];
            }
            bejegyzes_szamok--;
        }
    }

    FILE *filePointer;
    filePointer = fopen(base_file, "w");

    for (int i = 0; i < bejegyzes_szamok; i++) {
        fprintf(filePointer, "%s,%s,%d\n", telepules_array[i], nev_array[i], szam_array[i]);
    }

    printf("Az ugyfel (%s) torolve!\n", trimmed_end_nev);
    fclose(filePointer);
    exit(0);
}




void main_menu() {
    printf("Valassz a kovetkezo menupontokbol:\n");
    printf("-Kimutatasok:\n");
    printf("11 - teruleti lista\n");
    printf("12 - teljes lista\n");
    printf("-Jelentkezok kezelese:\n");
    printf("21 - uj felvetel\n");
    printf("22 - modositas\n");
    printf("23 - torles\n");
    printf("-Egyeb:\n");
    printf("24 - szimulacio\n");
    printf("99 - kilepes\n");
    printf("----------------------------------------\n");
}



void exit_program() {
    free(telepules_array);
    free(nev_array);
    free(szam_array);
    exit(0);
}

int szavazokerulet_szampi(char * versenyzo_) {
    if(versenyzo_[0] == '2' ||versenyzo_[0] == '4' ||versenyzo_[0] ==  '5'){
        return 1;
    } else {
        return 0;
    }
}

char *versenyzo_eloallitasa() {
    int telepules_index = randomL(0, 6);
    char *nev = "Nyuszika";
    char buf[100];
    int eredmeny= randomL(1, 100);
    snprintf(buf, 100, "%d,%s%d%d,%d", telepules_index,nev,telepules_index,eredmeny,eredmeny);
    char *str_to_ret = malloc (sizeof (char) * sizeof(buf));
    strcpy(str_to_ret,buf);
    return str_to_ret;
}

int randomL(int lower_, int upper_) {
    return (rand() % (upper_ - lower_ + 1)) + lower_;
}

void read_data_file(char *filename_) {
    FILE *filePointer;
    int bufferLength = 255;
    char buffer[bufferLength];
    filePointer = fopen(filename_, "r");

    telepules_array = NULL;
    nev_array = NULL;
    szam_array = NULL;
    bejegyzes_szamok = 0;

    while (fgets(buffer, bufferLength, filePointer)) {
        char *token = strtok(buffer, ",");
        while (token != NULL) {
            char *telepules = token;
            char *nev = strtok(NULL, ",");
            int l_szam = strtol(strtok(NULL, ","), NULL, 10);

            add_entry_to_arrays(telepules, nev, l_szam);

            token = strtok(NULL, ",");
        }
    }
    fclose(filePointer);
}

void add_entry_to_arrays(char *telepules_, char *nev_, int szam_) {
    telepules_array = (char **) realloc(telepules_array, (bejegyzes_szamok + 1) * sizeof(char *));
    telepules_array[bejegyzes_szamok] = strdup(telepules_);

    nev_array = (char **) realloc(nev_array, (bejegyzes_szamok + 1) * sizeof(char *));
    nev_array[bejegyzes_szamok] = strdup(nev_);

    szam_array = (int *) realloc(szam_array, (bejegyzes_szamok + 1) * sizeof(int));
    szam_array[bejegyzes_szamok] = (szam_);

    bejegyzes_szamok++;
}

char *getlineL(void) {
    char *line = malloc(100), *linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if (line == NULL){
        return NULL;
    }

    for (;;) {
        c = fgetc(stdin);
        if (c == EOF)
            break;

        if (--len == 0) {
            len = lenmax;
            char *linen = realloc(linep, lenmax *= 2);

            if (linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if ((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
}
