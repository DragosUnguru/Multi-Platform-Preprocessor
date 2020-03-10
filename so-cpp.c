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

    /* Init structures */
    no_header_dirs = count_include_dirs(argc, argv);
    header_dirs = malloc(no_header_dirs * sizeof *header_dirs);
    defines = hashmap_init();
    fin = NULL;
    fout = NULL;

    /* Solve homework, pula mea */

    parse_input(argc, argv, defines, header_dirs, &fin, &fout);
    process_defines(defines, fin, fout);

    /* Free everything */
    destroy_hash(defines);

    for (i = 0; i < no_header_dirs; ++i) {
        free(header_dirs[i]);
    }
    free(header_dirs);

    if (fin != NULL) {
        fclose(fin);
    }
    if (fout != NULL) {
        fclose(fout);
    }

    return 0;
}