#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pfind_to_sort[2], sort_to_parent[2];
    pipe(pfind_to_sort);
    pipe(sort_to_parent);
    
    pid_t pid[2];

    if ((pid[0] = fork()) < 0) {
        fprintf(stderr, "Error: fork failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    
    if(pid[0] == 0) {
        close(pfind_to_sort[0]);
        dup2(pfind_to_sort[1], STDOUT_FILENO);
        close(sort_to_parent[0]);
        close(sort_to_parent[1]);

        if(execvp("./pfind", argv) < 0) {
            fprintf(stderr, "Error: pfind failed.\n");
            return EXIT_FAILURE;
        }
    }

    if ((pid[1] = fork()) < 0) {
        fprintf(stderr, "Error: fork failed. %s.\n", strerror(errno));
    }
    
    if(pid[1] == 0) {
        dup2(pfind_to_sort[0], STDIN_FILENO);
        close(pfind_to_sort[1]);
        close(sort_to_parent[0]);
        dup2(sort_to_parent[1], STDOUT_FILENO);

        if(execlp("sort", "sort", NULL) < 0) {
            fprintf(stderr, "Error: sort failed.\n");
            return EXIT_FAILURE;
        }
    }

    close(pfind_to_sort[0]);
    close(pfind_to_sort[1]);
    close(sort_to_parent[1]);

    char buffer[128];
    int count;
    while((count = read(sort_to_parent[0], buffer, sizeof(buffer) - 1)) != 0) {
        if (count == -1) {
            perror("read()");
            exit(EXIT_FAILURE);
        }
        write(STDOUT_FILENO, buffer, count);
    }
    close(sort_to_parent[0]);
    wait(NULL);
    wait(NULL);
    return EXIT_SUCCESS;
}
