#include "list.h"

struct list_head* init_list() {
    struct list_head* head;

    head = malloc(sizeof *head);
    head->front = NULL;
    head->in_use = 0;

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
    new_node->value = malloc(value_len * sizeof(*new_node->value));
    memcpy(new_node->key, key, key_len);
    memcpy(new_node->value, value, value_len);

    /* Signal that hash entry is in use if this is it's first entry */
    if (head->front == NULL) {
        head->in_use = 1;
    }

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
    new_node->value = malloc(value_len * sizeof(*new_node->value));
    memcpy(new_node->key, key, key_len);
    memcpy(new_node->value, value, value_len);

    /* Push as first cell in the list, if applicable */
    if (head->front == NULL) {
        head->front = new_node;
        head->in_use = 1;

        return;
    }

    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    ptr->next = new_node;
}

struct node* pop_head(struct list_head* head) {
    struct node* return_value = head->front;

    if (return_value == NULL) {
        return NULL;
    }

    if (return_value->next == NULL) {
        head->front = NULL;
        head->in_use = 0;
    } else {
        head->front = return_value->next;
    }

    return return_value;
}

void remove_node(struct list_head* head, const char* key, const char* value) {
    struct node* tmp;
    struct node* pred;

    if (!head->in_use) {
        return;
    }
    
    tmp = head->front;
    pred = tmp;
    
    /* Search for desired entry */
    while(tmp != NULL && strncmp(tmp->key, key, tmp->key_len) && strncmp(tmp->value, value, tmp->value_len)) {
        pred = tmp;
        tmp = tmp->next;
    }

    /* If found, free it and manage list links */
    if(tmp != NULL) {

        /* If there's only this entry in the list */
        if (pred == tmp) {
            head->front = NULL;
            head->in_use = 0;
        }

        pred = tmp->next;
        free_node(tmp);
    }
}

void free_node(struct node* node) {
    free(node->key);
    free(node->value);
    free(node);
}

void destroy(struct list_head* head) {
    struct node* ptr;
    struct node* tmp;

    ptr = head->front;
    while(ptr != NULL) {
        tmp = ptr;
        ptr = ptr->next;

        free_node(tmp);
    }
    free(head);
}