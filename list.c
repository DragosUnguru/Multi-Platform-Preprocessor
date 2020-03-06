#include "list.h"

struct list_head* init_list() {
    struct list_head* head = malloc(sizeof *head);
    head->front = NULL;

    return head;
}

void push_front(struct list_head* head, const char* key,
                const char* value, size_t key_len, size_t value_len) {
    struct node* new_node;

    /* Manage new list entry */
    new_node = malloc(sizeof *new_node);
    new_node->next = head->front;
    new_node->key_len = key_len;
    new_node->value_len = value_len;

    new_node->key = malloc(key_len * sizeof(*new_node->key));
    new_node->key = malloc(value_len * sizeof(*new_node->key));
    memcpy(new_node->key, key, key_len);
    memcpy(new_node->value, value, value_len);

    /* Push as first cell in the list */
    head->front = new_node;
}

void push_back(struct list_head* head, const char* key,
                const char* value, size_t key_len, size_t value_len) {
    struct node* new_node;
    struct node* ptr;

    ptr = head->front;

    /* Manage new list entry */
    new_node = malloc(sizeof *new_node);
    new_node->key_len = key_len;
    new_node->value_len = value_len;

    new_node->key = malloc(key_len * sizeof(*new_node->key));
    new_node->key = malloc(value_len * sizeof(*new_node->key));
    memcpy(new_node->key, key, key_len);
    memcpy(new_node->value, value, value_len);

    /* Push as first cell in the list, if applicable */
    if (head->front == NULL) {
        head->front = new_node;
        return;
    }

    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    ptr->next = new_node;
}

struct node* pop(struct list_head* head) {
    struct node* return_value = head->front;
    head->front = (head->front == NULL) ? NULL : head->front->next;

    return return_value;
}

void destroy(struct list_head* head) {
    struct node* ptr;
    struct node* tmp;

    ptr = head->front;
    while(ptr->next != NULL) {
        tmp = ptr;
        ptr = ptr->next;

        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    free(head);
}