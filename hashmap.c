#include "hashmap.h"

int hashmap_init(hash_t* hash) {
    int i, ret;

    *hash = malloc(CAPACITY * sizeof **hash);
    DIE(*hash == NULL, FAILURE);
    
    for (i = 0; i < CAPACITY; ++i) {
        ret = init_list(&((*hash)[i]));
        DIE(ret == FAILURE, FAILURE);
    }

    return OK;
}

unsigned long hash_code(const char* key) {
    char delims[] = "\t[]{}<>=+-*/%!&|^.,:;() \n";
    unsigned long code = 5381;
    int c;

    while ((c = *key++)) {
        if (strchr(delims, c)) {
            continue;
        }
        code = ((code << 5) + code) + c;
    }

    return code % CAPACITY;
}

int hashmap_insert(hash_t hash, const char* key, const char* value) {

    return push_front(hash[hash_code(key)], key, value);
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

int is_key_mapped(hash_t hash, const char* key) {
    entry_t entry;
    unsigned long idx = hash_code(key);

    if (!hash[idx]->in_use) {
        return 0;
    }

    entry = hash[idx]->front;

    while(entry != NULL && strncmp(entry->key, key, entry->key_len)) {
        entry = entry->next;
    }

    return (entry == NULL) ? 0 : 1;
}

int append_value(hash_t hash, const char* key, const char* appending_string) {
    entry_t entry;
    size_t new_len;
    unsigned long idx = hash_code(key);
    
    if (!hash[idx]->in_use) {
        return OK;
    }

    entry = hash[idx]->front;

    new_len = strlen(entry->value) + strlen(appending_string) + 1;

    while(entry != NULL && strncmp(entry->key, key, entry->key_len)) {
        entry = entry->next;
    }

    entry->value = realloc(entry->value, new_len);
    DIE(entry->value == NULL, FAILURE);

    strcat(entry->value, appending_string);
    entry->value[new_len - 1] = '\0';
    
    return OK;
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

void erase_entry(hash_t hash, const char* key) {
    unsigned long idx = hash_code(key);

    remove_occurence(hash[idx], key);
}

void destroy_hash(hash_t hash) {
    int i;

    for (i = 0; i < CAPACITY; ++i) {
        destroy(hash[i]);
    }
    free(hash);
}