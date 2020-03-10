#include "list.h"

typedef struct node* entry_t;
typedef struct list_head** hash_t;

#define CAPACITY    12289

/*
 * Allocates, initializes and returns
 * a newly created hashmap
*/
int hashmap_init(hash_t* hash);

/*
 * Hash function. Returns an index
 * corresponding to the key.
*/
unsigned long hash_code(const char* key);

/*
 * Inserts a new pair (key, value) into
 * the hashmap
*/
int hashmap_insert(hash_t hash, const char* key, const char* value);

/*
 * Appends 'appending_string' at the end of the value
 * found at 'hash's hashtable key
*/
int append_value(hash_t hash, const char* key, const char* appending_string);

/*
 * Returns the value associated with the key
 * mapped in the hash table. Returns NULL if
 * not found
*/
char* get_value(hash_t hash, const char* key);

/*
 * Returns the key associated with the value
 * mapped in the hash table. Returns NULL if
 * not found. Highly inefficient!
*/
char* get_key(hash_t hash, const char* value);

/*
 * Returns 1 if the "key" is mapped in "hash",
 * 0 otherwise
*/
int is_key_mapped(hash_t hash, const char* key);

/*
 * Removes and frees every entry found in the
 * hash table matching the given key
*/
void erase_entry(hash_t hash, const char* key);

/*
 * Frees all the memory used by 'hash'
*/
void destroy_hash(hash_t hash);