#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "A3.h"

/*
*   NOTE: A history of this code is available on a GitHub repository.
*   This repository can be made available upon request.
*/

void read_from_pipe(int fd, char **arr, int *pos, int *count) {
    /* This function reads the data from the pipe.
                               Position is passed as pointer in order for function to know where the position is.*/
    MessageHeader header;
    while (read(fd, &header, sizeof(header)) > 0) {
        // Reads from the message header sent by child process.
        switch (header.type) {
            // This switches based on what the header type is.

            case TYPE_NAMECOUNT: {
                NameCountData dataNC; // Initializes struct to be used when reading data from pipe.
                read(fd, &dataNC, sizeof(NameCountData)); // Reads the data.
                int i;
                if ((i = check_in(dataNC.name, arr)) != -1) {
                    count[i] += dataNC.count; // This adds the count to the name if the name is already there.
                } else {
                    arr[*pos] = strdup(dataNC.name); // This adds a name to the array.
                    count[*pos] = dataNC.count; // This sets the count to the name in the array.
                    (*pos)++; // This moves the position to the next slot.
                }
                break;
            }

            // This case is for a possible extension in future to process other Struct data
            case TYPE_B: {
                break;
            }

            // Handle unknown type error
            default:

                fprintf(stderr, "Unknown message type received: %d\\n", header.type);
        }
    }
}


int main(int argc, char *argv[]) {
    //raise(SIGSTOP); // Comment if unneeded, this is for debugging purposes.
    char buf[MAXLINE];
    char *args[MAXARGS];
    size_t size = MSIZE * sizeof(NameCountData); // Size of global memory.
    size_t region_size = MNAME * sizeof(NameCountData); // Size of each child region of memory.
    void *GLOBAL = mmap(NULL, size, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS,
                        -1, 0); // Actually maps the memory in question.

    if (GLOBAL == MAP_FAILED) {
        perror("mmap error");
    }

    printf("%% "); /* print prompt (printf requires %% to print %) */
    char *nused[MAXLINE] = {0};
    int count[MAXLINE] = {0};
    int nused_count = 0;
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        // Read argument from stdin.
        memset(nused, 0, sizeof(nused)); /* Fills nused and count with 0s once loop restarts. */
        memset(count, 0, sizeof(count));
        nused_count = 0;
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0; /* replace newline with null */

        int i = 0;
        char *token = strtok(buf, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        if (i == 0) {
            printf("%% ");
            continue;
        }

        /* Set up the arrays to be used later by the program */

        // For each input file, fork and exec the countnames program with the file as the argument.
        int parent_pid = getpid();
        for (int j = 1; j < i; j++) {
            // Loop where all processes are forked.
            pid_t pid = fork(); // Create new process for each file.

            if (pid == 0) // Child process.
            {
                int child_id = getpid() - parent_pid;
                NameCountData *child_region = (NameCountData *) ((char *) GLOBAL + child_id * region_size);
                char *child_argv[] = {args[0], args[j], NULL};
                // Creates arguments to pass to execvp for child process to execute.
                execvp(child_argv[0], child_argv); // Execute countnames.c

                /* The child process should not get here, if it did, then something is wrong. */

                perror("execvp failed");
                exit(1);
            }
        }
        // This is for the parent program to read the data sent to it through the pipe.
        for (int j = 1; j < i; j++) {
        }

        /* Parent waits until all children are finished */

        while (wait(NULL) > 0) {
        }
        nprinter(nused, count); // Prints the names to output.
        fflush(stdout);
        fflush(stderr);
        printf("%% ");
    }
    for (int i = 0; nused[i] != 0; i++) {
        free(nused[i]); // Frees memory which was allocated when reading from pipe.
    }
    munmap(GLOBAL, MLINE * sizeof(NameCountData)); // Unmaps mapped memory.
    exit(0);
}
