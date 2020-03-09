#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

int main(int argc, char* argv[]) {
    unsigned int i;
    size_t no_header_dirs;
    char** header_dirs;
    hash_t defines;
    FILE* fin;
    FILE* fout;

    /* Parse input from command line */
    fin = NULL;
    fout = NULL;
    no_header_dirs = count_include_dirs(argc, argv);
    header_dirs = malloc(no_header_dirs * sizeof *header_dirs);

    defines = hashmap_init();

    /* Solve homework, pula mea */

    parse_input(argc, argv, defines, header_dirs, fin, fout);
    printf("\n\n%s\n%s\n", get_value(defines, "asdf"), get_value(defines, "altceva"));

    for (i = 0; i < no_header_dirs; ++i) {
        printf("%s\n", header_dirs[i]);
    }

    /* Free everything */
    destroy_hash(defines);

    for (i = 0; i < no_header_dirs; ++i) {
        free(header_dirs[i]);
    }
    free(header_dirs);

    return 0;
}