#include "hashmap.h"

#define NO_SUCH_FILE    2
#define BAD_ARG         7
#define MAX_LINE_LEN    256
#define MAX_WORD_LEN    64
#define DEFINE_OFFSET   8

#define DELIMS "\t[]{}<>=+-*/%!&|^.,:;() \n"


enum arg_type {IN_FILE, OUT_FILE, INCLUDE_PATH, DEFINE, BAD_PARAM,
            STICHED_OUT_FILE, STICHED_INCLUDE_PATH, STICHED_DEFINE};


/*
 * Populates the 'defines' hashtable with entries
 * found along the 'line' param. This manages
 * nested defines and multi-line defines
*/
int parse_define(hash_t defines, char* line);

/*
 * Given a line and a hashtable of <symbol, mapping>
 * it will replace all symbols found in the given line
 * and replace them. Outputs the line at 'fout'
*/
int map_line(hash_t defines, char* line);

/*
 * Reads the input file line by line, hashing any symbols
 * found, maps and replaces  all the defined symbols
 * found while resolving #ifdefs and #ifndefs. 
*/
int map_defines(hash_t defines);

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
int parse_input(int argc, char* argv[], hash_t defines, char** header_paths, FILE** inf_ptr, FILE** outf_ptr);
