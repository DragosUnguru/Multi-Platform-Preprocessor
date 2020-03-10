#include "utils.h"

void map_defines(hash_t defines, FILE* fin, FILE* fout) {
    char line[MAX_LINE_LEN];
    char* end;
    char* start;
    char* mapping;
    char between_quotations = 0;
    char word[64];
    char terminator;

    while(fgets(line, MAX_LINE_LEN, fin) != NULL) {
        /* Tokenize without losing delimitators:
         * start = pointer to the starting address of the token
         * end = pointer to the ending address of the token
        */
        start = line;
        end = strpbrk(line, DELIMS);

        /* Don't map the effective define */
        if (!strncmp(line, "#define ", DEFINE_OFFSET)) {
            fprintf(fout, "%s", line);
            continue;
        }

        /* For every token in line */
        while (end != NULL) {
            terminator = '\0';

            /* If we've got a match on a symbol, ignore it if it
             * is hugged by quotation marks
            */
            if (end[0] == '\"') {
                between_quotations = (between_quotations) ? 0 : 1;
            }

            /* Manage token */
            memset(word, 0, 64);
            memcpy(word, start, end - start + 1);

            /* There might be a delim at the end of our token.
             * Ignore it when searching for symbol
            */
            if (strchr(DELIMS, word[end - start])) {
                terminator = word[end - start];
                word[end - start] = '\0';
            }

            mapping = get_value(defines, word);
            word[end - start] = terminator;

            /* If we have a mapping for the found symbol,
             * replace with it accordingly or simply print
             * if not
            */
            if (mapping != NULL && !between_quotations) {
                fprintf(fout, "%s%c", mapping, terminator);
            } else {
                fprintf(fout, "%s", word);
            }

            start = end + 1;
            end = strpbrk(end + 1, DELIMS);
        }
    }
}

void process_defines(hash_t defines, FILE* fin, FILE* fout) {
    char line[MAX_LINE_LEN];
    char tmp[MAX_LINE_LEN];
    char* symbol;
    char* mapping;

    while(fgets(line, MAX_LINE_LEN, fin) != NULL) {

        if (!strncmp(line, "#define ", DEFINE_OFFSET)) {
            memcpy(tmp, line, strlen(line));
            symbol = strtok(tmp + DEFINE_OFFSET, " ");
            mapping = strtok(NULL, "\n");

            hashmap_insert(defines, symbol, mapping);
        }
    }

    fseek(fin, 0, SEEK_SET);
    map_defines(defines, fin, fout);
}

enum arg_type resolve_input_type(char* arg) {
    static char input_exists = 0;
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
        result = (input_exists) ? OUT_FILE : IN_FILE;
        input_exists = 1;
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

int parse_input(int argc, char* argv[], hash_t defines, char** header_paths, FILE** inf_ptr, FILE** outf_ptr) {
    int i, header_idx = 0;
    size_t arg_len;
    char* arg_indicator;
    char* arg_value;
    enum arg_type type;

    char* symbol;
    char* mapping;

    *inf_ptr = NULL;
    *outf_ptr = NULL;

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
            *inf_ptr = fopen(arg_value, "r");
        }
        else if (type == OUT_FILE) {
            *outf_ptr = fopen(arg_value, "w");
        }
    }

    if (*inf_ptr == NULL) {
        *inf_ptr = dup2(*inf_ptr, stdin);
    }
    if (*outf_ptr == NULL) {
        *outf_ptr = dup2(*outf_ptr, stdout);
    }    

    return OK;
}