#include "utils.h"

enum arg_type resolve_input_type(char* arg) {
    enum arg_type result;

    if (arg[0] == '-') {
        switch(arg[1]) {
        case 'D':
            result = DEFINE;
            break;
        case 'I':
            result = INCLUDE_PATH;
            break;
        case 'o':
            result = OUT_FILE;
            break;
        default:
            result = BAD_PARAM;
        }
    } else {
        result = IN_FILE;
    }

    /* Input argument is stiched together with effective value. eg: -I/some/dir */
    if (arg[2] != 0x00 && result != BAD_PARAM && result != IN_FILE) {
        result += 4;
    }

    return result;
}

size_t count_include_dirs(int argc, char* argv[]) {
    size_t result = 0;
    int i;
    char* arg;

    for (i = 1; i < argc; ++i) {
        arg = argv[i];

        if (arg[1] == 'I') {
            ++result;
        }
    }

    return result;
}

int parse_input(int argc, char* argv[], hash_t defines, char** header_paths, FILE* inf_ptr, FILE* outf_ptr) {
    int i, header_idx = 0;
    size_t arg_len;
    char* arg_indicator;
    char* arg_value;
    enum arg_type type;

    char* symbol;
    char* mapping;

    inf_ptr = NULL;
    outf_ptr = NULL;

    for (i = 1; i < argc; ++i) {
        arg_indicator = argv[i];
        type = resolve_input_type(arg_indicator);

        if (type == IN_FILE) {
            arg_value = arg_indicator;
        }
        else if (type < BAD_PARAM) {
            ++i;
            arg_value = argv[i];
        }
        else {
            arg_value = arg_indicator + 2;
        }
        /*
         * At this point we have the argument's type in 'type' and
         * argument's effective value (symbol and mapping / path) in
         * 'arg_value'.
        */

        if (type == DEFINE || type == STICHED_DEFINE) {
            symbol = strtok(arg_value, "=");
            mapping = strtok(NULL, "=");

            hashmap_insert(defines, symbol, mapping);
        }
        else if (type == BAD_PARAM) {
            return BAD;
        }
        else if (type == INCLUDE_PATH || type == STICHED_INCLUDE_PATH) {
            arg_len = strlen(arg_value);

            header_paths[header_idx] = malloc(arg_len * sizeof(char));
            memcpy(header_paths[header_idx++], arg_value, arg_len);
        }
        else if (type == IN_FILE) {
            inf_ptr = fopen(arg_value, "r");
        }
        else if (type == OUT_FILE) {
            outf_ptr = fopen(arg_value, "w");
        }
    }

    if (inf_ptr == NULL) {
        inf_ptr = stdin;
    }
    if (outf_ptr == NULL) {
        outf_ptr = stdout;
    }

    return OK;
}