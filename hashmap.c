#include "hashmap.h"

hash_t hashmap_init() {
    hash_t hash;
    int i;

    hash = malloc(CAPACITY * sizeof *hash);
    
    for (i = 0; i < CAPACITY; ++i) {
        hash[i] = init_list();
    }

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

    push_front(hash[hash_code(key)], key, value, key_len, value_len);
}

char* get_value(hash_t hash, const char* key) {
    entry_t entry;
    unsigned long idx = hash_code(key);

    if (!hash[idx]->in_use) {
        return NULL;
    }

    entry = hash[idx]->front;

    while(entry != NULL && strncmp(entry->key, key, entry->key_len)) {
        entry = entry->next;
    }

    return entry->value;
}

char* get_key(hash_t hash, const char* value) {
    entry_t entry;
    int i;

    for (i = 0; i < CAPACITY; ++i) {
        if (!hash[i]->in_use) {
            continue;
        }

        entry = hash[i]->front;
        while (entry != NULL && strncmp(entry->value, value, entry->value_len)) {
            entry = entry->next;
        }

        if(entry != NULL) {
            return entry->key;
        }
    }
    return NULL;
}

void erase_entry(hash_t hash, const char* key, const char* value) {
    unsigned long idx = hash_code(key);

    remove_node(hash[idx], key, value);
}

void destroy_hash(hash_t hash) {
    int i;

    for (i = 0; i < CAPACITY; ++i) {
        destroy(hash[i]);
    }
    free(hash);
}