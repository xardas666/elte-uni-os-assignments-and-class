#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

void main_menu();
void read_data_file();
char *getline(void);
void uj_felvetel();
void save_to_data_file(char *telepules_, char *nev_, int szam_);
void add_entry_to_arrays(char *telepules_, char *nev_, int szam_);
void telepulesek_listazasa();
void teruleti_lista();
void teljes_lista();
void modositas();
void torles();
void exit_program();

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

int main() {

    read_data_file();
    main_menu();

    while (1) {
        int menu_input = strtol(getline(), NULL, 10);
        switch (menu_input) {
            case 11: //teruleti lista
                teruleti_lista();
                break;
            case 12: //teljes lista
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
            case 99: //kilepes
                exit_program();
            default:
                printf("Nem letezo menu!\n");
        }
    }
}

void torles() {
    printf("Add meg az ugyfel nevet!\n");
    char *nev_ = getline();
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
    filePointer = fopen("input.txt", "w");

    for (int i = 0; i < bejegyzes_szamok; i++) {
        fprintf(filePointer, "%s,%s,%d\n", telepules_array[i], nev_array[i], szam_array[i]);
    }

    printf("Az ugyfel (%s) torolve!\n", trimmed_end_nev);
    fclose(filePointer);
    exit(0);
}

void modositas() {
    printf("Add meg az ugyfel nevet!\n");
    char *nev_ = getline();
    char *trimmed_end_nev = strtok(nev_, "\n");
    int ugyfel_index;

    for (int i = 0; i < bejegyzes_szamok - 1; i++) {
        if (strcmp(trimmed_end_nev, nev_array[i]) == 0) {
            ugyfel_index = i;
        }
    }

    telepulesek_listazasa();
    int varos_index = strtol(getline(), NULL, 10);
    while (varos_index < 0 || varos_index > 6) {
        printf("Hibas ertek 0 - 6 -ig vannak a varosok!\n");
        varos_index = strtol(getline(), NULL, 10);
    }
    printf("Add meg az ugyfel uj nevet!\n");
    char *nev = getline();

    printf("Add meg az ugyfel uj jelentkezesi szamat!\n");
    int jelentkezes_szama = strtol(getline(), NULL, 10);

    char *telepules = osszes_telepules_array[varos_index];

    telepules_array[ugyfel_index] = telepules;
    nev_array[ugyfel_index] = trimmed_end_nev;
    szam_array[ugyfel_index] = jelentkezes_szama;

    FILE *filePointer;
    filePointer = fopen("input.txt", "w");

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
    int varos_index = strtol(getline(), NULL, 10);
    while (varos_index < 0 || varos_index > 6) {
        printf("Hibas ertek 0 - 6 -ig vannak a varosok!\n");
        varos_index = strtol(getline(), NULL, 10);
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
    int varos_index = strtol(getline(), NULL, 10);
    while (varos_index < 0 || varos_index > 6) {
        printf("Hibas ertek 0 - 6 -ig vannak a varosok!\n");
        varos_index = strtol(getline(), NULL, 10);
    }
    printf("Add meg az ugyfel nevet!\n");
    char *nev = getline();

    printf("Add meg az ugyfel jelentkezesi szamat!\n");
    int jelentkezes_szama = strtol(getline(), NULL, 10);

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
    filePointer = fopen("input.txt", "a+");
    fprintf(filePointer, "%s,%s,%d\n", telepules_, trimmed_end_nev, szam_);
    printf("%s, %s, %d adatok mentesre kerultek!\n", telepules_, trimmed_end_nev, szam_);
    fclose(filePointer);
    exit(0);
}

void read_data_file() {
    FILE *filePointer;
    int bufferLength = 255;
    char buffer[bufferLength];
    filePointer = fopen("input.txt", "r");

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
    printf("99 - kilepes\n");
    printf("----------------------------------------\n");
}

char *getline(void) {
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

void exit_program() {
    free(telepules_array);
    free(nev_array);
    free(szam_array);
    exit(0);
}