#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

typedef struct node* entry_t;
typedef struct list_head** hash_t;

#define CAPACITY            12289

/*
 * Allocates, initializes and returns
 * a newly created hashmap
*/
hash_t hashmap_init();

/*
 * Hash function. Returns an index
 * corresponding to the key.
*/
unsigned long hash_code(const char* key);

/*
 * Inserts a new pair (key, value) into
 * the hashmap
*/
void hashmap_insert(hash_t hash, const char* key,
                    const char* value, size_t key_len, size_t value_len);

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
 * Removes and frees the entry found in the
 * hash table matching the (key, value) pair
*/
void erase_entry(hash_t hash, const char* key, const char* value);

/*
 * Frees all the memory used by 'hash'
*/
void destroy_hash(hash_t hash);