#include "countnames.h"

int check_in(char *a, char *b[]) {
    // Checks if a string is in the file.
    int rval = -1;
    for (int i = 0; b[i] != 0; i++) {
        if (strcmp(b[i], a) == 0) {
            rval = i;
            break;
        }
    }
    return rval;
}

void clnup(char *a1[], char *a2[]) {
    // Frees allocated memory.
    int i;
    for (i = 0; a1[i] != 0; i++) {
        free(a1[i]);
    }
    for (i = 0; a2[i] != 0; i++) {
        free(a2[i]);
    }
}

void nprinter(char *nused[], int count[]) {
    // Prints a string and the amount of times it occurs.
    for (int i = 0; nused[i] != 0; i++) {
        printf("%s: %d\n", nused[i], count[i]);
    }
}

void ncount(char *arr[], char *nused[], int count[]) {
    // Counts the number of times a string occurs in a file.
    int j = 0, k;
    for (int i = 0; arr[i] != 0; i++) {
        k = check_in(arr[i], nused);
        if (k == -1) {
            nused[j] = strdup(arr[i]); /* This allocates memory on the heap to store the string
                                        which needs to be freed later. */
            count[j++]++;
        } else {
            count[k]++;
        }
    }
}
