#include <assert.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <dispatch/dispatch.h>
#include "spinlock.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "unixtimer.c"

#define ARRAY_SIZE 100
#define NUM_ITER 100

typedef struct {
	clock_t cycles;
	double wall_time;
} run_time_t;

typedef struct {
	int iterations;
	atomic_int *array;
} atomic_thread_arg_t;

typedef struct {
	int iterations;
	int *array;
	pthread_spinlock_t *spin;
} spin_thread_arg_t;

typedef struct {
	int iterations;
	int *array;
	pthread_mutex_t *mutex;
} mutex_thread_arg_t;

typedef struct {
	int iterations;
	int *array;
	dispatch_semaphore_t *sem;
} sem_thread_arg_t;

void atomic_thread(void *t) {
	atomic_thread_arg_t *arg = t;
	int i, j;
	for (i = 0; i < arg->iterations; ++i) {
		for (j = 0; j < ARRAY_SIZE; ++j) {
			arg->array[j] += j;
		}
	}
}

run_time_t test_atomic(int *answer) {
	pthread_t tid[2];
	atomic_thread_arg_t args[2];
	atomic_int array[ARRAY_SIZE] = {0};

	clock_t start, end;
	double time;

	int i;
	start_wall_clock_timer();
	start = clock();
	for (i = 0; i < 2; ++i) {
		args[i].iterations = (int)(NUM_ITER / 2);
		args[i].array = array;
		pthread_create(&tid[i], NULL, (void*)atomic_thread, (void*)&args[i]);
	}

	for (i = 0; i < 2; ++i) {
		pthread_join(tid[i], NULL);
	}
	end = clock();
	time = wall_clock_seconds();

	for (i = 0; i < ARRAY_SIZE; ++i) {
		assert(answer[i] == array[i]);
	}

	run_time_t result = {end - start, time};
	return result;
}

void spin_thread(void *t) {
	spin_thread_arg_t *arg = t;
	int i, j;
	for (i = 0; i < arg->iterations; ++i) {
		pthread_spin_lock(arg->spin);
		for (j = 0; j < ARRAY_SIZE; ++j) {
			arg->array[j] += j;
		}
		pthread_spin_unlock(arg->spin);
	}
}

run_time_t test_spinlock(int *answer) {
	pthread_t tid[2];
	spin_thread_arg_t args[2];

	int array[ARRAY_SIZE] = {0};
	pthread_spinlock_t spin;

	clock_t start, end;
	double time;

	int i;
	start_wall_clock_timer();
	start = clock();
	pthread_spin_init(&spin, 0);
	for (i = 0; i < 2; ++i) {
		args[i].iterations = (int)(NUM_ITER / 2);
		args[i].array = array;
		args[i].spin = &spin;
		pthread_create(&tid[i], NULL, (void*)spin_thread, (void*)&args[i]);
	}

	for (i = 0; i < 2; ++i) {
		pthread_join(tid[i], NULL);
	}
	pthread_spin_destroy(&spin);
	end = clock();
	time = wall_clock_seconds();

	for (i = 0; i < ARRAY_SIZE; ++i) {
		assert(answer[i] == array[i]);
	}

	run_time_t result = {end - start, time};
	return result;
}

void mutex_thread(void *t) {
	mutex_thread_arg_t *arg = t;
	int i, j;
	for (i = 0; i < arg->iterations; ++i) {
		pthread_mutex_lock(arg->mutex);
		for (j = 0; j < ARRAY_SIZE; ++j) {
			arg->array[j] += j;
		}
		pthread_mutex_unlock(arg->mutex);
	}
}

run_time_t test_mutex(int *answer) {
	pthread_t tid[2];
	mutex_thread_arg_t args[2];

	int array[ARRAY_SIZE] = {0};
	pthread_mutex_t mutex;

	clock_t start, end;
	double time;

	int i;
	start_wall_clock_timer();
	start = clock();
	pthread_mutex_init(&mutex, NULL);
	for (i = 0; i < 2; ++i) {
		args[i].iterations = (int)(NUM_ITER / 2);
		args[i].array = array;
		args[i].mutex = &mutex;
		pthread_create(&tid[i], NULL, (void*)mutex_thread, (void*)&args[i]);
	}

	for (i = 0; i < 2; ++i) {
		pthread_join(tid[i], NULL);
	}
	pthread_mutex_destroy(&mutex);
	end = clock();
	time = wall_clock_seconds();

	for (i = 0; i < ARRAY_SIZE; ++i) {
		assert(answer[i] == array[i]);
	}

	run_time_t result = {end - start, time};
	return result;
}

void sem_thread(void *t) {
	sem_thread_arg_t *arg = t;
	int i, j;
	for (i = 0; i < arg->iterations; ++i) {
		dispatch_semaphore_wait(*arg->sem, DISPATCH_TIME_FOREVER);
		for (j = 0; j < ARRAY_SIZE; ++j) {
			arg->array[j] += j;
		}
		dispatch_semaphore_signal(*arg->sem);
	}
}

run_time_t test_semaphore(int *answer) {
	pthread_t tid[2];
	sem_thread_arg_t args[2];

	int array[ARRAY_SIZE] = {0};
	dispatch_semaphore_t sem;

	clock_t start, end;
	double time;

	int i;
	start_wall_clock_timer();
	start = clock();
	sem = dispatch_semaphore_create(1);
	for (i = 0; i < 2; ++i) {
		args[i].iterations = (int)(NUM_ITER / 2);
		args[i].array = array;
		args[i].sem = &sem;
		pthread_create(&tid[i], NULL, (void*)sem_thread, (void*)&args[i]);
	}

	for (i = 0; i < 2; ++i) {
		pthread_join(tid[i], NULL);
	}
	dispatch_release(sem);
	end = clock();
	time = wall_clock_seconds();

	for (i = 0; i < ARRAY_SIZE; ++i) {
		if (answer[i] != array[i]) {
			printf("%d %d %d\n", i, answer[i], array[i]);
		}
		assert(answer[i] == array[i]);
	}

	run_time_t result = {end - start, time};
	return result;
}