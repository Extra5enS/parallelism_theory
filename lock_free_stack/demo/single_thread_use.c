#include<stdio.h>
#include<stack.h>

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
	lfstack_for_each(&st);
	printf("Start search \n");
	for(int i = 0; i < 10; i++) {
		if(!lfstack_search(&st, &nums[i], sizeof(int))) {
			printf("SEARCH ERROR ELEM=%d \n", nums[i]);
			return -1;
		}
	}
	printf("Start out \n");
	for(int i = 0; i < 10; i++) {
		int n = 0;
		lfstack_pop(&st, &n, sizeof(int));
		printf("%d ", n);
	}
	printf("\n");
	printf("Free \n");
	lfstack_free(&st);
	return 0;
}
