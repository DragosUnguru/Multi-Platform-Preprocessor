/*
 * list.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAILURE 12
#define OK		0

#define DIE(assertion, ret_value)					\
	do {											\
		if (assertion) {							\
			return(ret_value);      				\
		}											\
	} while(0)

#define EXIT(assertion, exit_value)					\
	do {											\
		if (assertion) {							\
			exit(exit_value);						\
		}											\
	} while(0)

struct node {
    char* key;
    char* value;
    size_t key_len;
    size_t value_len;
    struct node* next;
};

struct list_head {
    struct node* front;
    char in_use;
};

/* 
 * Initializes a new empty list head
 * and returns it
*/
int init_list(struct list_head** head);

/*
 * Pushes a new cell into the front of the list
 * and manages the new head of the list
*/
int push_front(struct list_head* head, const char* key, const char* value);

/*
 * Pushes a new cell at the end of the list
*/
int push_back(struct list_head* head, const char* key, const char* value);

/*
 * Erases the first element in the list
 * (list's head) and returns it. Returns
 * NULL if list is empty
*/
struct node* pop_head(struct list_head* head);

/*
 * Removes, if found, from the list every entry containing
 * that matches the given key
*/
void remove_occurence(struct list_head* head, const char* key);

/*
 * Frees all the memory hoarded
 * by the list received as parameter
*/
void destroy(struct list_head* head);

/*
 * Frees all the memory hoarded by a node
*/
void free_node(struct node* node);