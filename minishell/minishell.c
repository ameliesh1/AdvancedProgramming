#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <sys/wait.h>


#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"

/**
 * ./minishell
 */


volatile sig_atomic_t interrupted = 0; 

// signal handler
void catch_signal(int sig) {
    interrupted = 1;
    printf("\n");
}

void free_commands(char **commands, int i) {
    int j;
    for (j = 0; j < i; j++) {
        free(commands[j]);
    }
}

int main(int argc, char **argv) {
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = catch_signal;
    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction(SIGINT)");
        return EXIT_FAILURE;
    }

    if(argc > 1) {
        printf("Error: Too many arguments specified.\n");
        printf("Usage: ./minishell\n");
        return EXIT_FAILURE;
    }
    char buf[128];
    while(true) {
        interrupted = 0;
        if((getcwd(buf, sizeof(buf))) == NULL) {
            printf("Error: getcwd failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }   
        printf("[");
        printf("%s%s", BRIGHTBLUE, buf);
        printf("%s]$ ", DEFAULT);
        fflush(stdout);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            if(interrupted) {
                interrupted = 0;
                if (errno == EINTR) {
                    //printf("Read interrupted.\n");
                    errno = 0;
                }
                //printf("\n");
                continue;
            }
            
            else if (feof(stdin)) {
                printf("\n");
                interrupted = 0;
                return EXIT_SUCCESS;
            } else if (ferror(stdin)) {
                printf("Error: Failed to read from stdin. %s.\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }
        char *eoln = strchr(buf, '\n');
        if (eoln != NULL) {
            *eoln = '\0';
        }

        if (!strcmp(buf, "exit")) {
            break;
        }

        if(!strncmp(buf, "cd ", 3) || !strcmp(buf, "cd")) {
            struct passwd *p = getpwuid(getuid());
            if(p == NULL) {
                printf("Error: Cannot get passwd entry. %s.\n", strerror(errno));
                continue;
            }
            char nextBuf[128];
            memset(nextBuf, 0, sizeof(nextBuf));
            if (!strcmp(buf, "cd") || !strcmp(buf, "cd ")) {
                nextBuf[0] = '\0';
            } else {
                strncpy(nextBuf, buf+3, sizeof(nextBuf) - 1);
            }
            char *c;
            if((c = strchr(nextBuf, '\"')) != NULL) { // extra credit!
                if(nextBuf[strlen(nextBuf)-1] != '\"') {
                    printf("Error: Missing end quote.\n");
                    continue;
                }
                char temp[128];
                memset(temp, 0, sizeof(temp));
                int a = 0;
                int b = 0;
                int count = 0;
                int withinQuote = 1;
                while(nextBuf[a] != '\0') {
                    if(nextBuf[a] == '\"') {
                        count++;
                        withinQuote = !withinQuote;
                    } else if(nextBuf[a] == ' ' && withinQuote == 1) {
                        printf("Error: Incorrect usage of double quotes.\n");
                        return EXIT_FAILURE;
                    } else {
                        temp[b] = nextBuf[a];
                        b++;
                    }
                    a++;
                }
                strcpy(nextBuf, temp);
                if(count % 2 != 0) {
                    printf("Error: Odd number of quotes.\n");
                    continue;
                }
                nextBuf[b] = '\0';
            } else {
                if(strchr(nextBuf, ' ') != NULL) {
                    printf("Error: Too many arguments to cd.\n");
                    continue;
                }
            }
            if(!strcmp(nextBuf, "") || !strcmp(nextBuf, "~")) {
                if(chdir(p->pw_dir) != 0) {
                    printf("Error: Cannot change to home directory. %s.\n", strerror(errno));
                    continue;
                }
            } else {
                if(nextBuf[0] == '~') {
                    char saveBuf[128];
                    strcpy(saveBuf, nextBuf + 1);
                    strcpy(nextBuf, p->pw_dir);
                    strcpy(nextBuf+strlen(p->pw_dir), "/");
                    strcpy(nextBuf+strlen(p->pw_dir)+1, saveBuf);
                } 
                struct stat statdir;
                if (stat(nextBuf, &statdir) < 0) {
                    fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", nextBuf, strerror(errno));
                    continue;
                }
                if (!S_ISDIR(statdir.st_mode)) {
                    fprintf(stderr, "Error: '%s' is not a regular directory.\n", nextBuf);
                    continue;
                }
                if(chdir(nextBuf) != 0) {
                    printf("Error: Cannot change to home directory. %s.\n", strerror(errno));
                    continue;
                }      
            }   
            // end of cd section
        } else {
            // storing commands from stdin into a similar form as argv[]
            char *commands[2048];
            memset(commands, 0, sizeof(commands));
            char *c;
            int i=0;
            char *ptr = buf;
            while((c = strchr(ptr, ' ')) != NULL && i<2048) {
                size_t len = c - ptr;
                if(*ptr == ' ') {
                    ptr++;
                    continue;
                }
                commands[i] = malloc(len+1);
                if(commands[i] == NULL) {
                    fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
                    free_commands(commands, i);
                    return EXIT_FAILURE;
                }
                strncpy(commands[i], ptr, len);
                commands[i][len] = '\0';
                ptr = c + 1;
                i++;
            }
            if((ptr != NULL) && i<2048) {
                size_t len = strlen(ptr);
                commands[i] = malloc(len+1);
                if(commands[i] == NULL) {
                    fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
                    free_commands(commands, i);
                    return EXIT_FAILURE;
                }
                strncpy(commands[i], ptr, len);
                commands[i][len] = '\0';
                i++;
            }
            commands[i] = NULL;

            //time to fork and exec!
            pid_t pid;
            if((pid = fork()) < 0) {
                fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
                free_commands(commands, i);
                return EXIT_FAILURE;
            }
            if(pid == 0) {
                interrupted = 0;
                struct sigaction action;
                memset(&action, 0, sizeof(struct sigaction));
                action.sa_handler = catch_signal;
                action.sa_flags = SA_RESTART;
                if (sigaction(SIGINT, &action, NULL) == -1) {
                    perror("sigaction(SIGINT)");
                    free_commands(commands, i);
                    return EXIT_FAILURE;
                }
                if(interrupted) {
                    interrupted = 0;
                    return EXIT_SUCCESS;
                }
                if(execvp(commands[0], commands) < 0) {
                    fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                    free_commands(commands, i);
                    return EXIT_FAILURE;
                }
                return EXIT_SUCCESS;
                
            } else {
                wait(NULL);
                free_commands(commands, i);
                int status;
                if (wait(&status) == -1) {
                    continue;
                }
                interrupted = 0;
                continue;
            }

        }

    }
    return EXIT_SUCCESS;
}