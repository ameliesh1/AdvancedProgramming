#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort.h"

#define MAX_STRLEN     64 // Not including '\0'
#define MAX_ELEMENTS 1024

/**
 * ./sort [-i|-d] [filename]
 */

void usage() {
    fprintf(stderr, "Usage: ./sort [-i|-d] [filename]\n");
    fprintf(stderr, "   -i: Specifies the input contains ints.\n");
    fprintf(stderr, "   -d: Specifies the input contains doubles.\n");
    fprintf(stderr, "   filename: The file to sort. If no file is supplied,");
    fprintf(stderr, " input is read from\n");
    fprintf(stderr, "             stdin.\n");
    fprintf(stderr, "   No flags defaults to sorting strings.\n");
}

int count_Lines(FILE *infile) {
    char buf[MAX_STRLEN+1];
    int line = 0;
    while(fgets(buf, MAX_STRLEN+1, infile) != NULL && line < MAX_ELEMENTS) {
        line++;
    }
    rewind(infile);
    return line;
}

void sortIntFile(FILE *infile) {
    int line = count_Lines(infile);
    int lineNum = line;
    int *ptr = malloc(line * sizeof(int));
    int *head = ptr;
    char buf[MAX_STRLEN+1];
    while(fgets(buf, MAX_STRLEN+1, infile) != NULL && line > 0) {
        if(ferror(infile)) {
                fprintf(stdout, "Cannot read data due to error %d\n", errno);
                break;
        }
        *ptr = (int) atoi(buf);
        ptr++;
        line--;
    }
    quicksort(head, lineNum, sizeof(int), int_cmp);
    for(int i = 0; i < lineNum; i++) {
        fprintf(stdout, "%d\n", head[i]);
        //free(head[i]);
    }
    free(head);
}

void sortDoubleFile(FILE *infile) {
    int line = count_Lines(infile);
    int lineNum = line;
    double *ptr = malloc(line * sizeof(double));
    double *head = ptr;
    char buf[MAX_STRLEN+1];
    while(fgets(buf, MAX_STRLEN+1, infile) != NULL && line > 0) {
        if(ferror(infile)) {
                fprintf(stderr, "Cannot read data due to error %d\n", errno);
                break;
        }
        *ptr = strtod(buf, NULL);
        ptr++;
        line--;
    }
    quicksort(head, lineNum, sizeof(double), dbl_cmp);
    for(int i = 0; i < lineNum; i++) {
        fprintf(stdout, "%lf\n", head[i]);
        //free(head[i]);
    }
    free(head);
}

void sortStringFile(FILE *infile) {
    int line = count_Lines(infile);
    int lineNum = line;
    char **ptr = malloc(line * sizeof(char *));
    char **head = ptr;
    char buf[MAX_STRLEN+1];
    while(fgets(buf, MAX_STRLEN+1, infile) != NULL && line > 0) {
        if(ferror(infile)) {
                fprintf(stderr, "Cannot read data due to error %d\n", errno);
                break;
        }
        *ptr = malloc(strlen(buf) + 1);
        buf[strlen(buf)-1] = '\0';
        strcpy(*ptr, buf);
        ptr++;
        line--;
    }
    quicksort(head, lineNum, sizeof(char *), str_cmp);
    char **toFree = head;
    for(int i = 0; i < lineNum; i++) {
        fprintf(stdout, "%s\n", head[i]);
        free(*toFree);
        toFree++;
    }
    free(head);
}

void sortIntInput() {
    int line = 0;
    char buf[MAX_STRLEN+1];
    int ptr[MAX_ELEMENTS];
    int *head = ptr;
    while(fgets(buf, MAX_STRLEN+1, stdin) != NULL && line < MAX_ELEMENTS) {
        if(ferror(stdin)) {
                fprintf(stderr, "Cannot read data due to error %d\n", errno);
                break;
        }
        *head = (int) atoi(buf);
        head++;
        line++;
    }
    quicksort(ptr, line, sizeof(int), int_cmp);
    for(int i = 0; i < line; i++) {
        fprintf(stdout, "%d\n", ptr[i]);
    }
}

void sortDoubleInput() {
    int line = 0;
    char buf[MAX_STRLEN+1];
    double ptr[MAX_ELEMENTS];
    double *head = ptr;
    while(fgets(buf, MAX_STRLEN+1, stdin) != NULL && line < MAX_ELEMENTS) {
        if(ferror(stdin)) {
                fprintf(stderr, "Cannot read data due to error %d\n", errno);
                break;
        }
        *head = strtod(buf, NULL);
        head++;
        line++;
    }
    quicksort(ptr, line, sizeof(double), dbl_cmp);
    for(int i = 0; i < line; i++) {
        fprintf(stdout, "%lf\n", ptr[i]);
    }
}

void sortStringInput() {
    int line = 0;
    char *ptr[MAX_ELEMENTS];
    char **head = ptr;
    char buf[MAX_STRLEN+1];
    while(fgets(buf, MAX_STRLEN+1, stdin) != NULL && line < MAX_ELEMENTS) {
        if(ferror(stdin)) {
                fprintf(stderr, "Cannot read data due to error %d\n", errno);
                break;
        }
        *head = malloc(strlen(buf) + 1);
        buf[strlen(buf)-1] = '\0';
        strcpy(*head, buf);
        head++;
        line++;
    }
    quicksort(ptr, line, sizeof(char *), str_cmp);
    char **toFree = ptr;
    for(int i = 0; i < line; i++) {
        fprintf(stdout, "%s\n", ptr[i]);
        free(*toFree);
        toFree++;
    }
}


int main(int argc, char **argv) {
    int iflag = 0;
    int dflag = 0;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, "id")) != -1) {
        switch (c) {
            case 'i':
                iflag = 1;
                break;
            case 'd':
                dflag = 1;
                break;
            case '?':
                fprintf(stderr, "Error: Unknown option '-%c' received.\n",
                        optopt);
                usage();
                return EXIT_FAILURE;
            default:
                return EXIT_FAILURE;
        }
    }
    
    if((iflag + dflag) > 1) {
        fprintf(stderr, "Error: Too many flags specified.\n");
        return EXIT_FAILURE;
    }

    if(iflag == 1 || dflag == 1) {
        argc -= 1;
        argv++;
    }

    char *filename;

    if(argc == 2) {
        filename = argv[1];
    } else if(argc > 2) {
        fprintf(stderr, "Error: Too many files specified.\n");
        return EXIT_FAILURE;
    } else {
        filename = NULL;
    }

    FILE *infile;

    if(filename != NULL) {
        infile = fopen(filename, "r");
        if (infile == NULL) {
            fprintf(stderr, "Error: Cannot open '%s'. %s.\n", filename,
                strerror(errno));
            return EXIT_FAILURE;
        }
        if(iflag == 1) {
            sortIntFile(infile);
        } else if(dflag == 1) {
            sortDoubleFile(infile);
        } else {
            sortStringFile(infile);
        }
        fclose(infile);
    } else {
        if(iflag == 1) {
            sortIntInput();
        } else if (dflag == 1) {
            sortDoubleInput();
        } else {
            sortStringInput();
        }
    }
    return EXIT_SUCCESS;
}
