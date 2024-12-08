#include <stdio.h>
#include <windows.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Koristenje: %s <direktorij_putanja>\n", argv[0]);
        return 1;
    }

    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*", argv[1]);

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Ne mogu otvoriti direktorij: %s\n", argv[1]);
        return 1;
    }

    int fileCount = 0;
    int dirCount = 0;

    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                dirCount++;
            }
        } else {
            fileCount++;
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

    printf("Direktorij: %s\n", argv[1]);
    printf("Broj direktorija: %d\n", dirCount);
    printf("Broj datoteka: %d\n", fileCount);

    return 0;
}
/*
gcc -o lab2_zad1 lab2_zad1.c
.\lab2_zad1 C:\\direktorij
*/