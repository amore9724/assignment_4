#include <sys/types.h>
#include <unistd.h>
#include "countnames.h"


/*
 *   NOTE: A history of this code is available on a GitHub repository.
 *   This repository can be made available upon request.
 */

int main(int argc, char *argv[]) /* int argc = argument count
                                  * char *argv[] = string array containing the actual arguments passed.*/

{
    char filename[64];
    sprintf(filename, "%s/%d.out", output_path, getpid());

    /* Create a PID.out for this child process
    and then set stdout to this PID.out */

    if (freopen(filename, "w", stdout) == NULL) {
        perror("freopen failed");
        return 1;
    }
    size_t global_size = MSIZE * sizeof(NameCountData);

    /* Create a PID.err for this child process
    and then set stderr to this PID.err */

    char fileerr[64];
    sprintf(fileerr, "%s/%d.err", output_path, getpid());

    if (freopen(fileerr, "w", stderr) == NULL) {
        perror("freopen failed");
        return 1;
    }
    if (argc == 1) // If no file was provided
    {
        puts("No file provided, exiting."); // This informs the user that there is no file.
        return 0;
    }

    FILE *f = fopen(argv[1], "r"); // Opens file to read from.
    if (f == NULL) {
        fprintf(stderr, "Error: Child %d could not open file %s and is exiting.\n", getpid(), argv[1]);
        return 1;
    }

    int i = 0, lnum = 0;
    char namebuf[MLINE] = {0}; // This buffer temporarily stores a line in the file.
    char *names[MSIZE] = {0}; // This stores all the names and their occurences in the file.
    while (fgets(namebuf, MLINE, f)) {
        lnum++;
        char *tok = strtok(namebuf, "\n"); // Uses "\n" character to tokenize string.
        if (tok == NULL) {
            fprintf(stderr, "Warning: Line %d in Child %d is empty.\n", lnum, getpid());
            continue;
        }
        names[i++] = strdup(tok); /* This allocates memory on the heap to store the string,
                                      which needs to be freed later. */
    }
    fclose(f);
    int count[MNAME] = {0}; // Contains the number of times a name occurs in the file.
    char *nused[MNAME] = {0}; // Contains the number of unique names used in the file.
    ncount(names, nused, count); // Counts the names used and sends it to the arrays.
    int mem_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0); // Open memory area in child process.

    if (mem_fd == -1) {
        perror("shm_open error");   // Can't open assigned memory location.
    }

    if (argv[2] == NULL) {
        perror("slot missing"); // Slot is not passed as an argument.
    }
    int slot = atoi(argv[2]);   // Get slot from parent.
    void *child_mem = mmap(NULL, global_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);   // Map memory to that slot

    if (child_mem == MAP_FAILED) {
        perror("mmap error");
    }

    NameCountData *space = (NameCountData *) child_mem + slot * MNAME; // Compute address where data will be written.

    for (i = 0; nused[i] != 0; i++) {
        // Sends data to parent

        NameCountData ncd; // Initializes NameCountData struct to send to parent.
        strcpy(ncd.name, nused[i]); // Copies the current name into the communication header.
        ncd.count = count[i]; // Sets the count of the current name to be set.
        memcpy(&space[i], &ncd, sizeof(NameCountData)); // Copy data to a NameCountData-sized slot in memory.
    }


    fflush(stdout); // Flushes stdout to prevent further issues.
    clnup(names, nused); // This will free the allocated memory
    munmap(child_mem, global_size); // Unmaps memory.
    close(mem_fd);  // Closes mem_fd.
    return 0;
}
