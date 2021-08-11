#include<stack.h>
#include<stdio.h>

#define PUSHER_NUM 10
#define POPER_NUM 5
#define SEARCHER_NUM 10
#define COUNT 3

struct lfstack_t lfs;
pthread_barrier_t bar;

void pr(void* num) {
    printf("[%d] -> ", *(int*)(num));
}


void* __pusher(void* n) {
	int my_num = (int)(n);
	lfstack_add_thread(&lfs);
	pthread_barrier_wait(&bar);
	for(int i = 0; i < COUNT; i++) {
		int num = i % 10;
		lfstack_push(&lfs, &num, sizeof(int));
		printf("Pusher[%d]: %d -> push \n", my_num, num);
	}
	pthread_exit(NULL);
}

void* __poper(void* n) {
	int my_num = (int)(n);
	lfstack_add_thread(&lfs);
	pthread_barrier_wait(&bar);
	for(int i = 0; i < COUNT; i++) {
		int num = 0;
		int res = lfstack_pop(&lfs, &num, sizeof(int));
		if(res) {
			printf("Poper[%d]: pop <- %d \n", my_num, num);
		} else {
			printf("Poper[%d]: pop <- nothing \n", my_num);
		}
	}
	pthread_exit(NULL);
}

void* __searcher(void* n) {
	int my_num = (int)(n);
	lfstack_add_thread(&lfs);
	pthread_barrier_wait(&bar);
	for(int i = 0; i < COUNT; i++) {
		int num = i % 10;
		int res = lfstack_search(&lfs, &num , sizeof(int));
		printf("Searcher[%d]: %d:%d \n", my_num, num, res);
	}
	pthread_exit(NULL);
}

int main() {
	pthread_barrier_init(&bar, NULL, PUSHER_NUM + POPER_NUM + SEARCHER_NUM);
	lfstack_init(&lfs, PUSHER_NUM + POPER_NUM + SEARCHER_NUM);
	pthread_t pusher[PUSHER_NUM];
	pthread_t poper[POPER_NUM];
	pthread_t searcher[SEARCHER_NUM];
	
	for(int i = 0; i < PUSHER_NUM; ++i) {
		pthread_create(&pusher[i], NULL, __pusher, (void*)(i));
	}
	for(int i = 0; i < POPER_NUM; ++i) {
		pthread_create(&poper[i], NULL, __poper, (void*)(i));
	}
	for(int i = 0; i < SEARCHER_NUM; ++i) {
		pthread_create(&searcher[i], NULL, __searcher, (void*)(i));
	}

	
	for(int i = 0; i < PUSHER_NUM; ++i) {
		pthread_join(pusher[i], NULL);
	}
	for(int i = 0; i < POPER_NUM; ++i) {
		pthread_join(poper[i], NULL);
	}
	for(int i = 0; i < SEARCHER_NUM; ++i) {
		pthread_join(searcher[i], NULL);
	}

	lfstack_for_each(&lfs, pr);
	printf("[NULL]\n");
	lfstack_free(&lfs);
	return 0;
}
