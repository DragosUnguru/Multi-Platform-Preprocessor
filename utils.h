#include "hashmap.h"

#define OK      0
#define BAD     1

#define DELIMS "\t[]{}<>=+-*/%!&|^.,:;()\."

enum arg_type {IN_FILE, OUT_FILE, INCLUDE_PATH, DEFINE, BAD_PARAM,
            STICHED_OUT_FILE, STICHED_INCLUDE_PATH, STICHED_DEFINE};

/*
 * Given an argument from the command line,
 * returns the type of that argument. Types are
 * listed in 'arg_type' enum.
*/
enum arg_type resolve_input_type(char* arg);

/*
 * Counts the number of '-I'
 * directive appearences
*/
size_t count_include_dirs(int argc, char* argv[]);

/*
 * Manages and returns all the data given
 * in the command line: defines, paths to headers
 * input file and output file
*/
int parse_input(int argc, char* argv[], hash_t defines, char** header_paths, FILE* inf_ptr, FILE* outf_ptr);
