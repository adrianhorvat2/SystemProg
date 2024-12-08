#include <stdio.h>
#include <windows.h>
#include <time.h>

#define LARGE_FILE_SIZE (4LL * 1024 * 1024 * 1024) // 4 GB (64-bit)

void ProcessDirectoryPreorder(const char *directory, int *largeFileCount, FILETIME *oldestFileTime, char *oldestFileName) {
    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*", directory);

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (error != ERROR_ACCESS_DENIED) { 
            printf("Ne mogu otvoriti direktorij: %s (Error code: %lu)\n", directory, error);
        }
        return;
    }

    printf("Obilazak direktorija: %s\n", directory);

    do {

        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }

        char fullPath[MAX_PATH];
        snprintf(fullPath, MAX_PATH, "%s\\%s", directory, findFileData.cFileName);

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ProcessDirectoryPreorder(fullPath, largeFileCount, oldestFileTime, oldestFileName);
        } else {
            LARGE_INTEGER fileSize;
            fileSize.LowPart = findFileData.nFileSizeLow;
            fileSize.HighPart = findFileData.nFileSizeHigh;

            if (fileSize.QuadPart > LARGE_FILE_SIZE) {
                (*largeFileCount)++;
            }

            if (CompareFileTime(&findFileData.ftLastWriteTime, oldestFileTime) < 0) {
                *oldestFileTime = findFileData.ftLastWriteTime;
                snprintf(oldestFileName, MAX_PATH, "%s", fullPath);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    if (GetLastError() != ERROR_NO_MORE_FILES) {
        DWORD error = GetLastError();
        if (error != ERROR_ACCESS_DENIED) { // Ignorišemo grešku pristupa
            printf("Greška pri obilasku: %s (Error code: %lu)\n", directory, error);
        }
    }

    FindClose(hFind);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Koristenje: %s <direktorij_putanja>\n", argv[0]);
        return 1;
    }

    int largeFileCount = 0;
    FILETIME oldestFileTime;
    char oldestFileName[MAX_PATH] = "";

    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);
    SystemTimeToFileTime(&systemTime, &oldestFileTime);

    ProcessDirectoryPreorder(argv[1], &largeFileCount, &oldestFileTime, oldestFileName);

    SYSTEMTIME oldestSystemTime;
    FileTimeToSystemTime(&oldestFileTime, &oldestSystemTime);

    printf("Direktorij: %s\n", argv[1]);
    printf("Broj velikih datoteka (>4 GB): %d\n", largeFileCount);
    printf("Najstarija datoteka: %s\n", oldestFileName);
    printf("Datum najstarije datoteke: %02d-%02d-%04d\n",
           oldestSystemTime.wDay, oldestSystemTime.wMonth, oldestSystemTime.wYear);

    return 0;
}
