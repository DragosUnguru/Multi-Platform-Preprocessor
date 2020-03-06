#include "hashmap.h"

hash_t hashmap_init() {
    hash_t hash;

    hash = malloc(CAPACITY * sizeof *hash);
    memset(hash, 0, CAPACITY * sizeof *hash);

    return hash;
}

unsigned long hash_code(const char* key) {
    unsigned long code = 5381;
    int c;

    while ((c = *key++))
        code = ((code << 5) + code) + c;

    return code % CAPACITY;
}

void hashmap_insert(hash_t hash, const char* key,
                    const char* value, size_t key_len, size_t value_len) {

    unsigned long key_hash = hash_code(key);

    hash[key_hash].key_len = key_len;
    hash[key_hash].value_len = value_len;
    hash[key_hash].in_use = 1;

    hash[key_hash].key = malloc(key_len * sizeof *key);
    hash[key_hash].value = malloc(value_len * sizeof *value);

    memcpy(hash[key_hash].key, key, key_len);
    memcpy(hash[key_hash].value, value, value_len);
}

char* get_value(hash_t hash, const char* key) {
    entry_t entry;
    entry = hash[hash_code(key)];

    return (entry.in_use) ? entry.value : NULL;
}

char* get_key(hash_t hash, const char* value) {
    int i;

    for (i = 0; i < CAPACITY; ++i) {
        /* Must be a better way */
        if (!hash[i].in_use) {
            continue;
        }

        if (!strcmp(hash[i].value, value)) {
            return hash[i].key;
        }
    }
    return NULL;
}

void hashmap_remove(hash_t hash, const char* key) {
    unsigned long idx = hash_code(key);

    if (!hash[idx].in_use) {
        return;
    }
    hash[idx].in_use = 0;

    free(hash[idx].key);
    free(hash[idx].value);
}