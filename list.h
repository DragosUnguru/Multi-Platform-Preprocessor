/*
 * list.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAILURE 12

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(FAILURE);      			\
		}									\
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
struct list_head* init_list();

/*
 * Pushes a new cell into the front of the list
 * and manages the new head of the list
*/
void push_front(struct list_head* head, const char* key, const char* value);

/*
 * Pushes a new cell at the end of the list
*/
void push_back(struct list_head* head, const char* key, const char* value);

/*
 * Erases the first element in the list
 * (list's head) and returns it. Returns
 * NULL if list is empty
*/
struct node* pop_head(struct list_head* head);

/*
 * Removes, if found, from the list the entry containing
 * the pair (key, value) given as parameters
*/
void remove_node(struct list_head*, const char* key, const char* value);

/*
 * Frees all the memory hoarded
 * by the list received as parameter
*/
void destroy(struct list_head* head);

/*
 * Frees all the memory hoarded by a node
*/
void free_node(struct node* node);