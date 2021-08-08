/*
 *	This progremm is written for 64b cache line size
 *  If you don't now you proc cache line size README for you ;)
 */ 
#pragma pack(1)

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<pthread.h>
#include<sys/time.h>

#define COUNT 4
#define MASS_SIZE 8
#define TIME_DEL 1000000000

struct { // 64b.
	uint8_t val;
	uint8_t pud0; // puding for full cacheline
	uint16_t pud1;
	uint32_t pud2;
} cacheliky_val[COUNT];

long LIM = 0;
int LIM_DEG_MASS[MASS_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};

uint8_t val[COUNT];

// mod indicator
uint8_t cacheliky_mode_on;

// pthread lib
int nums[COUNT]; 
pthread_t pthreads[COUNT];
pthread_barrier_t barrier;
pthread_mutex_t mutex;

// time values
long globall_time;

void* show_problem_1(void* num) {
	int my_num = *(int*)(num);

	pthread_barrier_wait(&barrier);
	struct timeval start_time;
	gettimeofday(&start_time, NULL);

	for(long i = 0; i < LIM; ++i) {
		cacheliky_val[my_num].val += cacheliky_val[(my_num + 1) % COUNT].val;
	}


	struct timeval end_time;
	gettimeofday(&end_time, NULL);
	long time = (end_time.tv_sec - start_time.tv_sec) * TIME_DEL + end_time.tv_usec - start_time.tv_usec;

	pthread_mutex_lock(&mutex);
	globall_time += time;
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

void* show_problem_2(void* num) {
	int my_num = *(int*)(num);

	pthread_barrier_wait(&barrier);
	struct timeval start_time;
	gettimeofday(&start_time, NULL);

	for(long i = 0; i < LIM; ++i) {
		val[my_num] += val[(my_num + 1) % COUNT];
	}

	struct timeval end_time;
	gettimeofday(&end_time, NULL);
	long time = (end_time.tv_sec - start_time.tv_sec) * TIME_DEL + end_time.tv_usec - start_time.tv_usec;

	pthread_mutex_lock(&mutex);
	globall_time += time;
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

void start_procces(FILE* f) {
	globall_time = 0;
	pthread_barrier_init(&barrier, NULL, COUNT);
	for(int j = 0; j < 10; j++) {
		for(int i = 0; i < COUNT; i++) {
			nums[i] = i;
			if(cacheliky_mode_on) {
				pthread_create(&pthreads[i], NULL, show_problem_2, &nums[i]);
			} else {
				pthread_create(&pthreads[i], NULL, show_problem_1, &nums[i]);
			}
		}	
		for(int i = 0; i < COUNT; i++) {
			pthread_join(pthreads[i], NULL);
		}
	}
	printf("Time: %lf \n", globall_time / (10.0 * COUNT * TIME_DEL));
	fprintf(f, "%lf ", globall_time / (10.0 * COUNT * TIME_DEL));	
}

int main() {
	pthread_mutex_init(&mutex, NULL);
	FILE* f = fopen("data.txt", "w");

	for(int k = 0; k < MASS_SIZE; k++) {
		LIM = (1l << 28) * LIM_DEG_MASS[k];
		fprintf(f, "%d ", LIM_DEG_MASS[k]);

			
		printf("(LIM = 2^28 * %d) Time for one thread: \n", LIM_DEG_MASS[k]);
		globall_time = 0;
		for(int j = 0; j < 10; j++) {
			struct timeval start_time;
			gettimeofday(&start_time, NULL);
			
			for(int l = 0; l < COUNT; ++l) {
				for(long i = 0; i < LIM; ++i) {
					val[l] += val[(l + 1) % COUNT];
				}
			}

			struct timeval end_time;
			gettimeofday(&end_time, NULL);
			long time = (end_time.tv_sec - start_time.tv_sec) * TIME_DEL + end_time.tv_usec - start_time.tv_usec;

			globall_time += time;
		}
		printf("Time: %lf \n", globall_time / (10.0 * TIME_DEL));
		fprintf(f, "%lf ", globall_time / (10.0 * TIME_DEL));	

		cacheliky_mode_on = 0;
		printf("(LIM = 2^28 * %d) Time with common vars: \n", LIM_DEG_MASS[k]);
		start_procces(f);

		cacheliky_mode_on = 1;
		printf("(LIM = 2^28 * %d) Time with vars with pudings: \n", LIM_DEG_MASS[k]);
		start_procces(f);
		fprintf(f, "\n");
	}

	fclose(f);

	return 0;
}


