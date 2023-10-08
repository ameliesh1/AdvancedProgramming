#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * ./pfind -d <directory> -p <permissions string> [-h]
 */

void usage(FILE *std) {
    fprintf(std, "Usage: ./pfind -d <directory> -p <permissions string> [-h]\n");
}

int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
               S_IRGRP, S_IWGRP, S_IXGRP,
               S_IROTH, S_IWOTH, S_IXOTH};

char* perm_str(struct stat *statbuf) {
    char *perm_string;
    if ((perm_string = malloc(10 * sizeof(char))) == NULL) {
        fprintf(stderr, "Error: malloc failed. %s.\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 9; i += 3) {
        // Using the ternary operator for succinct code.
        perm_string[i] = statbuf->st_mode & perms[i] ? 'r' : '-';
        perm_string[i+1] = statbuf->st_mode & perms[i+1] ? 'w' : '-';
        perm_string[i+2] = statbuf->st_mode & perms[i+2] ? 'x' : '-';
    }
    perm_string[9] = '\0';
    return perm_string;
}

int output = 0;

int recurseThruDirs(char *arg, char *perms) {
    char path[PATH_MAX];
    if (realpath(arg, path) == NULL) {
        fprintf(stderr, "Error: Cannot get full path of file '%s'. %s.\n",
                arg, strerror(errno));
        return EXIT_FAILURE;
    }

    DIR *dir;
    if ((dir = opendir(path)) == NULL) {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n",
                path, strerror(errno));
        return EXIT_FAILURE;
    }
    char full_filename[PATH_MAX + 1];
    full_filename[0] = '\0';

    if (strcmp(path, "/")) {
        size_t copy_len = strnlen(path, PATH_MAX);
        memcpy(full_filename, path, copy_len);
        full_filename[copy_len] = '\0';
    }

    size_t pathlen = strlen(full_filename) + 1;
    full_filename[pathlen - 1] = '/';
    full_filename[pathlen] = '\0';
    struct dirent *entry;
    struct stat sb;

    errno = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        strncpy(full_filename + pathlen, entry->d_name, PATH_MAX - pathlen);
        if (lstat(full_filename, &sb) < 0) {
            fprintf(stderr, "Error: Cannot stat file '%s'. %s.\n",
                    full_filename, strerror(errno));
            continue;
        }
        if (S_ISDIR(sb.st_mode)) {
            recurseThruDirs(full_filename, perms);
        } else {
            char *filePerms = perm_str(&sb);
            if(strcmp(filePerms, perms) == 0){
                printf("%s\n", full_filename);
                output++;
            }
        }
    }

    if(errno != 0) {
        return EXIT_FAILURE;
    }
    
    closedir(dir);
    return EXIT_SUCCESS;
}


int main(int argc, char **argv) {
    int dflag = 0;
    int pflag = 0;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, "d:p:h")) != -1) {
        switch (c) {
            case 'd':
                dflag = 1;
                break;
            case 'p':
                pflag = 1;
                break;
            case 'h':
                usage(stdout);
                return EXIT_SUCCESS;
            case '?':
                fprintf(stderr, "Error: Unknown option '-%c' received.\n",
                        optopt);
                return EXIT_FAILURE;
            default:
                return EXIT_FAILURE;
        }
    }

    if(dflag + pflag < 2) {
        if(dflag + pflag == 0) {
            fprintf(stderr, "Error: Must provide arguments.\n");
            usage(stderr);
        } else if(dflag == 0) {
            fprintf(stderr, "Error: Required argument -d <directory> not found.\n");
        } else if(pflag == 0) {
            fprintf(stderr, "Error: Required argument -p <permissions string> not found.\n");
        }
        return EXIT_FAILURE;
    } else if(argc > 5) {
        fprintf(stderr, "Error: Too many arguments.\n");
        usage(stderr);
        return EXIT_FAILURE;
    }

    struct stat statdir;
    if (stat(argv[2], &statdir) < 0) {
        fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", argv[2],
                strerror(errno));
        return EXIT_FAILURE;
    }
    if (!S_ISDIR(statdir.st_mode)) {
        fprintf(stderr, "Error: '%s' is not a regular file.\n", argv[2]);
        return EXIT_FAILURE;
    }
    
    char *perms = argv[4];
    if(strlen(perms) != 9) {
        fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", argv[4]);
        return EXIT_FAILURE;
    }
    for(int i = 0; i < 9; i+=3) {
        if(perms[i] != 'r' && perms[i] != '-') {
            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", argv[4]);
            return EXIT_FAILURE;
        }
        if(perms[i+1] != 'w' && perms[i+1] != '-') {
            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", argv[4]);
            return EXIT_FAILURE;
        }
        if(perms[i+2] != 'x' && perms[i+2] != '-') {
            fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", argv[4]);
            return EXIT_FAILURE;
        }
    }

    char path[PATH_MAX];
    if (realpath(argv[2], path) == NULL) {
        fprintf(stderr, "Error: Cannot get full path of file '%s'. %s.\n",
                argv[1], strerror(errno));
        return EXIT_FAILURE;
    }
    
    if(recurseThruDirs(path, perms) == 0) {
        if(output == 0) {
            fprintf(stdout, "<no output>\n");
        }
    }

    return EXIT_SUCCESS;
}