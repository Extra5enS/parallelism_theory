#include<stdio.h>
#include<stack.h>

void pr(void* num) {
	printf("[%d] -> ", *(int*)(num));
}

void ch(void* num) {
	*(int*)(num) *= 2;
}

int main() {
	printf("Hello, here I will show you work of lock free stack in singe thread mode \n");
	struct lfstack_t st;
	lfstack_init(&st, 1);
	lfstack_add_thread(&st);
	int nums[10] = {1, 6, 89, 2, -5, 100, 300, 324, 6, 90};	
	printf("Start enter \n");
	for(int i = 0; i < 10; i++) {
		lfstack_push(&st, &nums[i], sizeof(int));
	}
	printf("[NULL]\n");
	printf("Start search \n");
	lfstack_for_each(&st, pr);
	printf("[NULL]\n");
	for(int i = 0; i < 10; i++) {
		if(!lfstack_search(&st, &nums[i], sizeof(int))) {
			printf("SEARCH ERROR ELEM=%d \n", nums[i]);
			return -1;
		} else {
			lfstack_doif(&st, &nums[i], sizeof(int), ch);
		}
	}
	printf("Start out \n");
	for(int i = 0; i < 10; i++) {
		int n = 0;
		lfstack_pop(&st, &n, sizeof(int));
		lfstack_for_each(&st, pr);
		printf("[NULL]\n");
	}
	printf("\n");
	printf("Free \n");
	lfstack_free(&st);
	return 0;
}
