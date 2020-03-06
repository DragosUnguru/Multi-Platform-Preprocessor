#include <stdio.h>
#include <stdlib.h>

#include "hashmap.h"
int main() {

    hash_t hashmap = hashmap_init();
    hashmap_insert(hashmap, "key1", "value1", 4, 6);
    hashmap_insert(hashmap, "key2", "value2", 4, 6);
    hashmap_insert(hashmap, "key3", "value3", 4, 6);
    hashmap_insert(hashmap, "key4", "value4", 4, 6);
    hashmap_insert(hashmap, "key5", "value5", 4, 6);
    hashmap_insert(hashmap, "key6", "value6", 4, 6);
    hashmap_insert(hashmap, "key7", "value7", 4, 6);
    hashmap_insert(hashmap, "key8", "value8", 4, 6);
    hashmap_insert(hashmap, "key9", "value9", 4, 6);

    hashmap_remove(hashmap, "key1");

    printf("%s", get_value(hashmap, "key5"));
    printf("%s", get_key(hashmap, "value9"));

    return 0;
}