#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <stdbool.h>
#define GROW 16

int cmp_int(const void* a, const void* b) {
    return (*(int*) b - *(int*) a);
}

int cmp_int_r(const void* a, const void* b) {
    return (*(int*) a - *(int*) b);
}

int cmp_str(const void * a, const void * b) {
    const char *pa = *(const char**) a;
    const char *pb = *(const char**) b;

    return strcmp(pa,pb);
}

int cmp_str_r(const void * a, const void * b) {
    const char *pa = *(const char**) a;
    const char *pb = *(const char**) b;

    return strcmp(pb, pa);
}

void* cmp(bool numeric, bool reverse) {
    if(numeric) {
        return reverse ? cmp_int_r : cmp_int;
    } else {
        return reverse ? cmp_str_r : cmp_str;
    }
}

int main(int argc, char *argv[]) {
    size_t optind;
    bool numeric = false, reverse = false;
    FILE *in = stdin, *out = stdout;
    for (optind = 1; optind < argc && argv[optind][0] == '-'; optind ++) {
        switch (argv[optind][1]) {
            case 'r':
                reverse = true;
                break;
            case 'o':
                out = fopen( argv[optind + 1], "w");
                optind ++;
                break;
            case 'n':
                numeric = true;
                break;
            case 'h':
                printf("Usage: %s [-o outfile] [-r] [-n] (infile)\n", argv[0]);
                printf(" -> Options o, r & n should be working fine, program can read from stdin or file and output to stdout or file.\n");
                exit(EXIT_FAILURE);
            default:
                printf("Use '%s -h' for help.\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(argv[optind] != NULL) {
        in = fopen(argv[optind], "r");
        if(in == NULL) {
            printf("No such file.\n");
            exit(EXIT_FAILURE);
        }
    }

    char buff[255];
    int len = 0, arr_size = GROW;
    char** arr = malloc(arr_size * sizeof(char*));

    while (fscanf(in, "%s", buff) == 1) {
        if(len == arr_size) {
            arr_size += GROW;
            arr = realloc(arr, arr_size * sizeof(char*));
        }

        arr[len] = (char*) malloc(strlen(buff) * sizeof(char));
        strcpy(arr[len], buff);
        len ++;
    }

    qsort(arr, len, sizeof(char*), cmp(numeric, reverse));

    for(int i = 0; i < len; i ++) {
        fprintf(out, "%s\n", arr[i]);
        free(arr[i]);
    }

    free(arr);
    fclose(out);
    fclose(in);
}