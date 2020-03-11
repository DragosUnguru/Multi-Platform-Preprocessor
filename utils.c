#include "utils.h"

int resolve_includes(hash_t defines, char* header_name, char** include_paths, size_t no_of_paths) {
    FILE* header_ptr;
    unsigned int path_idx = 0;
    char* just_name;
    int ret;
    char line[MAX_LINE_LEN];
    char next_header_name[MAX_WORD_LEN];

    header_ptr = fopen(header_name, "r+");

    while (header_ptr == NULL) {
        /* Header file not found. Search in other giver directories */
        DIE(path_idx >= no_of_paths, NO_SUCH_FILE);

        memset(next_header_name, 0, MAX_WORD_LEN);

        if (include_paths[path_idx] == NULL)
            ++path_idx;

        memcpy(next_header_name, include_paths[path_idx], strlen(include_paths[path_idx]) + 1);
        strcat(next_header_name, header_name);

        header_ptr = fopen(next_header_name, "r+");
        ++path_idx;
    }

    /* Parse header file */
    while(fgets(line, MAX_LINE_LEN, header_ptr) != NULL) {
        if (strstr(line, "#include")) {
            just_name = strtok(line, "\"");
            just_name = strtok(NULL, "\"");

            ret = resolve_includes(defines, just_name, include_paths, no_of_paths);
            DIE(ret != OK, ret);
        } else {
            ret = resolve_line(defines, line, header_ptr);
            DIE(ret != OK, ret);
        }
    }

    return OK;
}

int parse_define(hash_t defines, char* line, FILE* fin) {
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

    ret = hashmap_insert(defines, symbol, mapping);
    DIE(ret == FAILURE, FAILURE);

    /* Manage multi-line defines */
    while (strchr(line, '\\') != NULL) {
        fgets(line, MAX_LINE_LEN, fin);
        ret = append_value(defines, symbol, line + 1);
        DIE(ret != OK, ret);
    }
    
    start = strtok(mapping, " ");
    memset(word, 0, MAX_WORD_LEN);

    /* Manage hashtable entry in case our mapping
     * contains other previously defined values
    */
    while (start != NULL) {
        end = start;

        if (is_key_mapped(defines, start)) {
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

int evaluate_if(hash_t defines, char* cond_value) {
    char* mapping;

    cond_value = strtok(cond_value, DELIMS);
    mapping = get_value(defines, cond_value);

    if (mapping != NULL)
        cond_value = mapping;

    /* We have in 'cond_value' the if condition statement. Evaluate it */
    if (cond_value[0] != '0')
        return 1;

    return 0;
}

int resolve_line(hash_t defines, char* line, FILE* fin) {
    static int if_result = 0;
    static int passed = 0;
    int is_mapped, offset;
    char* symbol;
    char* mapping;
    char between_quotations = 0;
    char word[MAX_WORD_LEN];
    char terminator;
    char* start;
    char* end;

    if(!strncmp(line, "int main()", MAIN))
        passed = 1;

    if(!passed && line[0] == '\n')
        return OK;

    if (!strncmp(line, "#ifndef ", IFNDEF)) {
        symbol = line + IFNDEF;
        is_mapped = is_key_mapped(defines, symbol);

        /* Skip this line containing #ifndef.
         * This doesn't have to show in the final output
        */
        fgets(line, MAX_LINE_LEN, fin);

        /* Evaluate if and manage the result
         * for a potential next 'else'
        */
        if (!is_mapped)
            if_result = 1;
        else {
            if_result = 0;
            return OK;
        }
    }

    if (!strncmp(line, "#ifdef ", IFDEF)) {
        symbol = line + IFDEF;
        is_mapped = is_key_mapped(defines, symbol);

        fgets(line, MAX_LINE_LEN, fin);

        /* Evaluate if and memorize it's value
            * for a potential next 'else'
        */
        if (is_mapped)
            if_result = 1;
        else {
            if_result = 0;
            return OK;
        }
    }

    while (!strncmp(line, "#if ", IF) || !strncmp(line, "#elif ", ELIF)) {
        offset = (line[1] == 'i') ? IF : ELIF;

        if_result = evaluate_if(defines, line + offset);
        fgets(line, MAX_LINE_LEN, fin);

        /* If condition failed, skip all lines in this block */
        while (!if_result && strncmp(line, "#else", ELSE)
            && strncmp(line, "#endif", ENDIF) && strncmp(line, "#elif ", ELIF))
            fgets(line, MAX_LINE_LEN, fin);
    }

    if (!strncmp(line, "#else", ELSE)) {
        /* Don't print this line (#else) */
        fgets(line, MAX_LINE_LEN, fin);

        /* Evaluate else statement */
        while (if_result && strncmp(line, "#endif", ENDIF))
            fgets(line, MAX_LINE_LEN, fin);

        if_result = 0;
    }

    /* Hash <symbol, mapping>, if '#define' found */
    if (!strncmp(line, "#define ", DEFINE_OFFSET)) {
        return parse_define(defines, line, fin);
    }

    /* This doesn't have to show in the output */
    if (!strncmp(line, "#endif", ENDIF)) {
        if_result = 0;
        return OK;
    }

    /* Manage if it's an '#undef' line */
    if (!strncmp(line, "#undef ", UNDEF)) {
        mapping = line + UNDEF;
        erase_entry(defines, mapping);

        return OK;
    }

    /* Tokenize without losing delimitators */
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
        if (mapping != NULL && !between_quotations)
            printf("%s%c", mapping, terminator);
        else
            printf("%s", word);

        start = end + 1;
        end = strpbrk(end + 1, DELIMS);
    }

    return OK;
}

int process_file(hash_t defines, char** include_paths, int no_paths) {
    char line[MAX_LINE_LEN];
    char* header_name;
    int ret;

    while(fgets(line, MAX_LINE_LEN, stdin) != NULL) {
        if (!strncmp(line, "#include  ", INCLUDE)) {
            header_name = strtok(line, "\"");
            header_name = strtok(NULL, "\"");
            
            ret = resolve_includes(defines, header_name, include_paths, no_paths);
            DIE(ret != OK, ret);
        } else {
            ret = resolve_line(defines, line, stdin);
            DIE(ret == FAILURE, FAILURE);
        }
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
    if (arg[2] != 0x00 && result != BAD_PARAM && result != IN_FILE)
        result += 4;

    return result;
}

size_t count_include_dirs(int argc, char* argv[]) {
    size_t result = 0;
    int i;
    char* arg;

    for (i = 1; i < argc; ++i) {
        arg = argv[i];

        if (arg[1] == 'I')
            ++result;
    }

    return result;
}

int parse_input(int argc, char* argv[], hash_t defines, char*** header_paths, size_t* no_header_paths, FILE** inf_ptr, FILE** outf_ptr) {
    int i, header_idx = 1, ret;
    size_t arg_len;
    char* arg_indicator;
    char* arg_value;
    enum arg_type type;

    char* symbol;
    char* mapping;
    char* path;

    *inf_ptr = NULL;
    *outf_ptr = NULL;

    *no_header_paths = count_include_dirs(argc, argv) + 1;
    *header_paths = malloc(*no_header_paths * sizeof **header_paths);
    DIE(header_paths == NULL, FAILURE);

    /* Reserved for input file's directory */
    (*header_paths)[0] = NULL;

    for (i = 1; i < argc; ++i) {
        arg_indicator = argv[i];
        type = resolve_input_type(arg_indicator);

        if (type == IN_FILE)
            arg_value = arg_indicator;
        else if (type < BAD_PARAM) {
            ++i;
            arg_value = argv[i];
        }
        else
            arg_value = arg_indicator + 2;
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

        else if (type == BAD_PARAM)
            return BAD_ARG;

        else if (type == INCLUDE_PATH || type == STICHED_INCLUDE_PATH) {
            arg_len = strlen(arg_value) + 2;

            (*header_paths)[header_idx] = malloc(arg_len * sizeof(char));
            DIE((*header_paths)[header_idx] == NULL, FAILURE);

            memcpy((*header_paths)[header_idx], arg_value, arg_len - 2);

            (*header_paths)[header_idx][arg_len - 2] = '/';
            (*header_paths)[header_idx++][arg_len - 1] = '\0';
        }

        else if (type == IN_FILE) {
            *inf_ptr = freopen(arg_value, "r", stdin);

            if (*inf_ptr == NULL)
                /* File not found. Bad param */
                return NO_SUCH_FILE;

            /* Insert the input's file directory as header dir
             * with high priority
            */
            path = strrchr(arg_value, '/');

            if (path != NULL) {
                path[0] = '\0';

                arg_len = strlen(arg_value) + 2;
                (*header_paths)[0] = malloc(arg_len * sizeof(*((*header_paths)[0])));

                memcpy((*header_paths)[0], arg_value, arg_len);

                (*header_paths)[0][arg_len - 2] = '/';
                (*header_paths)[0][arg_len - 1] = '\0';
            }
        }

        else if (type == OUT_FILE || type == STICHED_OUT_FILE) {
            *outf_ptr = freopen(arg_value, "w", stdout);
        }
    }

    return OK;
}