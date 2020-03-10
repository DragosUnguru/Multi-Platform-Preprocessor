#include "utils.h"

int parse_define(hash_t defines, char* line) {
    static char multi_line = 0;
    char tmp[MAX_LINE_LEN];
    char word[MAX_WORD_LEN];
    char* symbol;
    char* mapping;
    char* start;
    char* end;
    int ret;

    memcpy(tmp, line, strlen(line));
    symbol = strtok(tmp + DEFINE_OFFSET, " ");
    mapping = strtok(NULL, "\n");

    /* Manage multi-line defines */
    if (strchr(line, '\\') == NULL && multi_line) {
        ret = append_value(defines, symbol, mapping);
        DIE(ret != OK, ret);

        multi_line = 0;

        return OK;
    }

    if (multi_line) {
        ret = append_value(defines, symbol, mapping);
        DIE(ret != OK, ret);

        return OK;
    }

    if (strchr(line, '\\') != NULL && !multi_line) {
        multi_line = 1;
    }

    ret = hashmap_insert(defines, symbol, mapping);
    DIE(ret == FAILURE, FAILURE);
    
    start = strtok(mapping, " ");
    memset(word, 0, MAX_WORD_LEN);

    /* Manage hashtable entry in case our mapping
     * contains other previously defined values
    */
    while (start != NULL) {
        end = start;

        if (start[0] != '\"' && start[0] != '+' && start[0] != '-' && (start[0] < '0' || start[0] > '9')) {
            /* Found a nested mapping */
            end = get_value(defines, start);
        }

        /* Build the <MAPPING> value in "word" */
        strcat(word, end);
        strcat(word, " ");
        start = strtok(NULL, " ");
    }

    word[strlen(word) - 1] = '\0';

    ret = hashmap_insert(defines, symbol, word);
    DIE(ret == FAILURE, FAILURE);

    return OK;
}

int map_line(hash_t defines, char* line) {
    char* mapping;
    char between_quotations = 0;
    char word[MAX_WORD_LEN];
    char terminator;
    char* start;
    char* end;

    /* Hash <symbol, mapping>, if '#define' found */
    if (!strncmp(line, "#define ", DEFINE_OFFSET)) {
        return parse_define(defines, line);
    }

    /* Tokenize without losing delimitators:
     * start = pointer to the starting address of the token
     * end = pointer to the ending address of the token
    */
    start = line;
    end = strpbrk(line, DELIMS);

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
        memset(word, 0, MAX_WORD_LEN);
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
            printf("%s%c", mapping, terminator);
        } else {
            printf("%s", word);
        }

        start = end + 1;
        end = strpbrk(end + 1, DELIMS);
    }

    return OK;
}

int evaluate_if(hash_t defines, char* cond_value) {
    char* mapping;

    cond_value = strtok(cond_value, DELIMS);
    mapping = get_value(defines, cond_value);

    if (mapping != NULL) {
        cond_value = mapping;
    }

    /* We have in 'cond_value' the if condition statement. Evaluate it */
    if (cond_value[0] != '0') {
        return 1;
    }

    return 0;
}

int map_defines(hash_t defines) {
    char line[MAX_LINE_LEN];
    char* symbol;
    char* mapping;
    char if_result = 0;
    int ret, passed = 0;

    while(fgets(line, MAX_LINE_LEN, stdin) != NULL) {

        if(!strncmp(line, "int main()", 10)) {
            passed = 1;
        }

        if(!passed && line[0] == '\n') {
            continue;
        }

        if (!strncmp(line, "#undef ", 7)) {
            symbol = line + 7;
            erase_entry(defines, symbol);
            continue;
        }

        if (!strncmp(line, "#ifndef ", 8)) {
            symbol = line + 7;
            mapping = get_value(defines, symbol);

            /* Skip this line containing #ifndef.
             * This doesn't have to show in the final output
            */
            fgets(line, MAX_LINE_LEN, stdin);

            /* Evaluate if and manage the result
             * for a potential next 'else'
            */
            if (mapping == NULL) {
                if_result = 1;
            } else {
                if_result = 0;
                continue;
            }
        }

        if (!strncmp(line, "#ifdef ", 7)) {
            symbol = line + 7;
            mapping = get_value(defines, symbol);

            fgets(line, MAX_LINE_LEN, stdin);

            /* Evaluate if and memorize it's value
             * for a potential next 'else'
             GRIJA MAREEEE CU NULLUL ASTA LA GET VALUE AOLO*/
            if (mapping != NULL) {
                if_result = 1;
            } else {
                if_result = 0;
                continue;
            }
        }

        if (!strncmp(line, "#if ", 4)) {
            if_result = evaluate_if(defines, line + 4);
            fgets(line, MAX_LINE_LEN, stdin);

            /* If condition failed, skip all lines in this block */
            while (!if_result && strncmp(line, "#else", 5) && strncmp(line, "#endif", 6) && strncmp(line, "#elif ", 6)) {
                fgets(line, MAX_LINE_LEN, stdin);
            }
        }

        if (!strncmp(line, "#elif ", 6)) {
            /* If previous condition was evaluated to true, skip all lines in this block */
            if (if_result) {
                while (strncmp(line, "#endif", 6)) {
                    fgets(line, MAX_LINE_LEN, stdin);
                }

                if_result = 0;
                continue;
            }

            /* if not, evaluate the present condition */
            if_result = evaluate_if(defines, line + 6);
            fgets(line, MAX_LINE_LEN, stdin);

            /* If condition failed, skip all lines in this block */
            while (!if_result && strncmp(line, "#else", 5) && strncmp(line, "#endif", 6) && strncmp(line, "#elif ", 6)) {
                fgets(line, MAX_LINE_LEN, stdin);
            }
        }

        if (!strncmp(line, "#else", 5)) {
            /* Don't print this line (#else) */
            fgets(line, MAX_LINE_LEN, stdin);

            /* Evaluate else statement */
            while (if_result && strncmp(line, "#endif", 6)) {
                fgets(line, MAX_LINE_LEN, stdin);
            }
            if_result = 0;
        }

        /* This doesn't have to show in the output */
        if (!strncmp(line, "#endif", 6)) {
            if_result = 0;
            continue;
        }

        ret = map_line(defines, line);
        DIE(ret == FAILURE, FAILURE);
    }

    return OK;
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
        result = (input_exists) ? ((input_exists == 1) ? OUT_FILE : BAD_PARAM) : IN_FILE;
        ++input_exists;
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
    int i, header_idx = 0, ret;
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

            ret = hashmap_insert(defines, symbol, mapping);
            DIE(ret == FAILURE, FAILURE);
        }
        else if (type == BAD_PARAM) {
            return BAD_ARG;
        }
        else if (type == INCLUDE_PATH || type == STICHED_INCLUDE_PATH) {
            arg_len = strlen(arg_value);

            header_paths[header_idx] = malloc(arg_len * sizeof(char));
            DIE(header_paths[header_idx] == NULL, FAILURE);

            memcpy(header_paths[header_idx++], arg_value, arg_len);
        }
        else if (type == IN_FILE) {
            *inf_ptr = freopen(arg_value, "r", stdin);

            if (*inf_ptr == NULL) {
                /* File not found. Bad param */
                return NO_SUCH_FILE;
            }
        }
        else if (type == OUT_FILE) {
            *outf_ptr = freopen(arg_value, "w", stdout);
        }
    }

/*
    if (*inf_ptr == NULL) {
        *inf_ptr = dup2(*inf_ptr, stdin);
    }
    if (*outf_ptr == NULL) {
        *outf_ptr = dup2(*outf_ptr, stdout);
    }    
*/
    return OK;
}