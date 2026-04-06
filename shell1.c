#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "countnames.h"

/*
*   NOTE: A history of this code is available on a GitHub repository.
*   This repository can be made available upon request.
*/


int main(int argc, char *argv[]) {
    //raise(SIGSTOP); // Comment if unneeded, this is for debugging purposes.
    mkdir("output", 0755);
    char buf[MAXLINE];
    char *args[MAXARGS];
    int mem_fd = shm_open("/shared_memory_i", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (mem_fd == -1) {
        perror("shm_open error");
    }
    size_t global_size = MSIZE * sizeof(NameCountData);

    if (ftruncate(mem_fd, global_size) == -1) {
        perror("ftruncate error");
    }

    void *GLOBAL = mmap(NULL, global_size, PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        mem_fd, 0); // Actually maps the memory in question.

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
        memset(GLOBAL, 0, global_size);
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


        for (int j = 1; j < i; j++) {
            // Loop where all processes are forked.
            pid_t pid = fork(); // Create new process for each file.

            if (pid == 0) // Child process.
            {
                /* USE ONLY IF THIS METHOD FAILS */
                // int child_id = getpid() - parent_pid;
                // NameCountData *child_region = (NameCountData *) ((char *) GLOBAL + child_id * region_size);
                if (fcntl(mem_fd, F_SETFD, 0) == -1) {  // Clear FD_CLOEXEC flag
                    perror("fcntl error");
                }

                char tempbuf[25];
                sprintf(tempbuf, "%d", j - 1);
                char *child_argv[] = {args[0], args[j], tempbuf, NULL}; // Creates arguments to pass to execvp for child process to execute.
                execvp(child_argv[0], child_argv); // Execute countnames.c

                /* The child process should not get here, if it did, then something is wrong. */

                perror("execvp failed");
                exit(1);
            }
        }

        /* Parent waits until all children are finished */

        while (wait(NULL) > 0) {
        }
        NameCountData *total = GLOBAL;
        for (int j = 1; j < i; j++) {

            int slot = j-1;
            NameCountData *child_slot = (NameCountData*)GLOBAL + slot * MNAME;

            for (int k = 0; k < MNAME && child_slot[k].name[0] != '\0'; k++) {
                NameCountData temp = total[j];
                int index = check_in(temp.name, nused);
                if (index == -1) {
                    nused[nused_count++] = strdup(temp.name);
                    count[j] = temp.count;
                } else {
                    count[index] += temp.count;
                }
            }

        }
        nprinter(nused, count); // Prints the names to output.
        fflush(stdout);
        fflush(stderr);
        printf("%% ");
    }
    for (int i = 0; nused[i] != 0; i++) {
        free(nused[i]); // Frees memory which was allocated when reading from pipe.
    }
    munmap(GLOBAL, global_size); // Unmaps mapped memory.
    exit(0);
}
