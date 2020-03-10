#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

int main(int argc, char* argv[]) {
    unsigned int i;
    int ret;
    size_t no_header_dirs;
    char** header_dirs;
    hash_t defines;
    FILE* fin;
    FILE* fout;

    /* Init structures */
    no_header_dirs = count_include_dirs(argc, argv);

    header_dirs = malloc(no_header_dirs * sizeof *header_dirs);
    EXIT(header_dirs == NULL, FAILURE);

    ret = hashmap_init(&defines);
    EXIT(ret == FAILURE, FAILURE);

    fin = NULL;
    fout = NULL;

    /* Solve homework, pula mea */

    ret = parse_input(argc, argv, defines, header_dirs, &fin, &fout);
    EXIT(ret != OK, ret);

    ret = map_defines(defines);
    EXIT(ret == FAILURE, FAILURE);

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

    return OK;
}