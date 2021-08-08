#include "../include/stack.h"

#include<stdio.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

struct __node*  __lfstack_add_hp(struct lfstack_t* lfs, struct __node** ptr) {
	// find num of place
	int id = pthread_self();
	struct __node* loc_ptr = NULL;
	int place_i = 0;
	for(;place_i < lfs -> thread_n; ++place_i) {
		if(lfs -> id_list[place_i] == id) {		
			// add hp
			do {
				loc_ptr = atomic_load(ptr);
				lfs -> dlist[place_i] = ptr;
			} while(loc_ptr != atomic_load(ptr));
			break;	
		}
	}
	return loc_ptr;
}

int comp (const void* i, const void* j) {
	return *(int*)(i) - *(int*)(j);
}

void __lfstack_find_to_delete(struct lfstack_t* lfs) {
	// first lets try to understend, may be some one else now deleted elements
	int next_delete = 0;
	if(atomic_compare_exchange_strong(&(lfs -> in_delete), &next_delete, 1)) {
		size_t p = 0, new_dcount = 0;
		void** plist = calloc(2 * lfs -> thread_n, sizeof(void*));
		void** new_dlist = calloc(2 * lfs -> thread_n, sizeof(void*));
		// form list of HP
		for(size_t t = 0; t < lfs -> thread_n; ++t) {
			if(!lfs -> dlist[t]) {
				plist[p++] = lfs -> dlist[t];
			}
		}
		// sort to opt next step
		qsort(plist, p, sizeof(void*), comp);
		// check that node for delete is't HP for all thread
		for(size_t i = 0; i < 2 * (lfs -> thread_n); ++i) {
			if(bsearch(&(lfs -> dlist[i]), 
						plist, lfs -> thread_n, 
						sizeof(void*), comp)) {
				new_dlist[new_dcount++] = lfs -> dlist[i];
			} else {
				printf("here1 \n");
				free(lfs -> dlist[i]);
			}
		}
		// add new list
		for(size_t i = 0; i < new_dcount; ++i) {
			lfs -> dlist[i] = new_dlist[i];
		}
		lfs -> dlist_iter = new_dcount;
		
		free(plist);
		free(new_dlist);
		// open "door" for another threads
		atomic_store(&(lfs -> in_delete), 0);
	}
}


void __lfstack_add_delete(struct lfstack_t* lfs, struct __node* node) {
	// taking up space for new delete node
	int p = atomic_fetch_add(&(lfs -> dlist_iter), 1);
	
	// if dlist full
	if(lfs -> dlist_iter >= 2 * lfs -> thread_n) {
		// try to really delete some node to free space and try again
		__lfstack_find_to_delete(lfs);
		__lfstack_add_delete(lfs, node);
	} else {
		// add this node
		lfs -> dlist[p] = node;
	}
}


void lfstack_init(struct lfstack_t* lfs, size_t thread_n) {
	// start value for stack
	lfs -> first = NULL;
	lfs -> size = 0;
	// start value for thread
	lfs -> thread_n = thread_n; 
	lfs -> id_list_i = 0;
	lfs -> id_list = calloc(thread_n, sizeof(int));
	lfs -> HP_list = calloc(thread_n, sizeof(void*));
	// start dlist info
	lfs -> dlist_iter = 0;
	lfs -> dlist = calloc(2 * thread_n, sizeof(void*));
	lfs -> in_delete = 0;
}


void lfstack_add_thread(struct lfstack_t* lfs) {
	// find pthread_id to use as uid
	int id = pthread_self();
	// add new id if it's posibole
	if(lfs -> id_list_i == lfs -> thread_n) {
		return; // <- add message of error
	}
	int p = atomic_fetch_add(&(lfs -> id_list_i), 1);
	lfs -> id_list[p] = id; 
}


void lfstack_push(struct lfstack_t* lfs, void* data, size_t len) {
	// select memory for new node
	byte* node = malloc(len + sizeof(struct __node));
	// add infomation in __node
	struct __node* info = (struct __node*)(node);
	info -> __next = NULL;
	info -> __data_size = len;
	// copy information fron data to free space in node
	memcpy(node + sizeof(struct __node), data, len);
	// try to place this node in lfs -> first, 
	// but we now that we may do it with any other user
	// So when we try it, we must chache it atomicly with substitution
	struct __node* n = NULL;
	do {
		n = atomic_load(&(lfs -> first));
		info -> __next = n;
	} while(!atomic_compare_exchange_weak(&(lfs -> first), &n, info));
}


void lfstack_pop(struct lfstack_t* lfs, void* data, size_t len) {
	// remember and remove first element
	struct __node* n = NULL;
	do {
		n = atomic_load(&lfs -> first);
	} while(n && !atomic_compare_exchange_weak(&(lfs -> first), &n, n -> __next));
	if(n == NULL) {
		return;
	}
	// cpy info from node to data
	memcpy(data, (byte*)(n) + sizeof(struct __node), MIN(len, n -> __data_size));	
	// add it to delete list
	__lfstack_add_delete(lfs, n);
}


int lfstack_search(struct lfstack_t* lfs, void* data, size_t len) {
	// we think that it's no the same data
	int res = 0;
	// p -> first mustn't be deleted
	struct __node* p =  __lfstack_add_hp(lfs, &(lfs -> first));
	// try to find this data
	for(;p != NULL;) {
		// if data and information near __node is the same, 
		// we have the sane data is stack
		if(memcmp((byte*)(p) + sizeof(struct __node), data, len) == 0) {
			res = 1;
			break;
		}
		// We finish with this poiter so we my try to get next node
		p = __lfstack_add_hp(lfs, &(p -> __next));
	}
	return res;
}

void lfstack_free(struct lfstack_t* lfs) {
	free(lfs -> id_list);
	free(lfs -> HP_list);
	for(size_t i = 0; i < lfs -> dlist_iter; ++i) {
		free(lfs -> dlist[i]);
	}
}

// only for single mode
void lfstack_for_each(struct lfstack_t* lfs) {
	for(struct __node* p = lfs -> first; p != NULL; p = p -> __next) {
		byte* data = (byte*)(p) + sizeof(struct __node);
		int* n = (int*)(data);
		printf("[%d] -> ", *n);
	}
	printf("\n");
}
