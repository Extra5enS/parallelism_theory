#ifndef LOCK_FREE_STACK
#define LOCK_FREE_STACK

#include<stdlib.h>
#include<stdatomic.h>
#include<pthread.h>
#include<string.h>

#define byte char

#define HP_COUNT 10
#define DELETE_SIZE 10
#define data(node) ((byte*)(node) + sizeof(node))

/*
 * struct node {
 *   struct __node; <- part of node, that include all info for stack 
 *	 ... <- your data, it may by any numer of vars with any types
 * }	
 */
struct __node {
	struct __node* __next;
	size_t __data_size;
};

struct lfstack_t {
	struct __node* first; // <- pointer to 
	size_t size; // <- stack size

	size_t thread_n;
	size_t id_list_i;
	int* id_list; // list with pthread_id
	void** HP_list; // HP_list[id_list_size] only one HP

	size_t dlist_iter;
	void** dlist;
	
	int in_delete;
};


void lfstack_init(struct lfstack_t* lfs, size_t thread_n);

void lfstack_add_thread(struct lfstack_t* lfs);

void lfstack_push(struct lfstack_t* lfs, void* data, size_t len);
int lfstack_pop(struct lfstack_t* lfs, void* data, size_t len);

int lfstack_search(struct lfstack_t* lfs, void* data, size_t len);
int lfstack_doif(struct lfstack_t* lfs, void* data, size_t len, void(*func)(void*));

void lfstack_free(struct lfstack_t*);

void lfstack_for_each(struct lfstack_t* lfs, void(*func)(void*));

#endif
