#include <assert.h>
#include <pthread.h>
#include <dispatch/dispatch.h>
#include "spinlock.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "unixtimer.c"

const int NUM_RUNS = 10000;

typedef struct {
	clock_t cycles;
	double wall_time;
} run_time_t;

run_time_t *test_atomic() {
	run_time_t *results = (run_time_t*)malloc(NUM_RUNS*sizeof(run_time_t));

	clock_t start, end;
	double time;

	atomic_int x;
	int i, j;
	for (i = 0; i < NUM_RUNS; ++i) {
		x = 0;

		start_wall_clock_timer();
		start = clock();
		for (j = 0; j <= i; ++j) {
			x++;
		}
		end = clock();
		time = wall_clock_seconds();

		assert(x == i+1);
		results[i].cycles = end - start;
		results[i].wall_time = time;
	}

	return results;
}

run_time_t *test_spinlock() {
	run_time_t *results = (run_time_t*)malloc(NUM_RUNS*sizeof(run_time_t));
	pthread_spinlock_t spinlock;
	pthread_spin_init(&spinlock, 0);
	
	clock_t start, end;
	double time;

	int x;
	int i, j;
	for (i = 0; i < NUM_RUNS; ++i) {
		x = 0;

		start_wall_clock_timer();
		start = clock();
		//pthread_spin_init(&spinlock, 0);
		for (j = 0; j <= i; ++j) {
			pthread_spin_lock(&spinlock);
			x++;
			pthread_spin_unlock(&spinlock);
		}
		//pthread_spin_destroy(&spinlock);
		end = clock();
		time = wall_clock_seconds();

		assert(x == i+1);
		results[i].cycles = end - start;
		results[i].wall_time = time;
	}

	pthread_spin_destroy(&spinlock);
	return results;
}

run_time_t *test_mutex() {
	run_time_t *results = (run_time_t*)malloc(NUM_RUNS*sizeof(run_time_t));
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

	clock_t start, end;
	double time;

	int x;
	int i, j;
	for (i = 0; i < NUM_RUNS; ++i) {
		x = 0;

		start_wall_clock_timer();
		start = clock();
		//pthread_mutex_init(&mutex, NULL);
		for (j = 0; j <= i; ++j) {
			pthread_mutex_lock(&mutex);
			x++;
			pthread_mutex_unlock(&mutex);
		}
		//pthread_mutex_destroy(&mutex);
		end = clock();
		time = wall_clock_seconds();

		assert(x == i+1);
		results[i].cycles = end - start;
		results[i].wall_time = time;
	}

	pthread_mutex_destroy(&mutex);
	return results;
}

run_time_t *test_semaphore() {
	run_time_t *results = (run_time_t*)malloc(NUM_RUNS*sizeof(run_time_t));
	dispatch_semaphore_t sem;
	sem = dispatch_semaphore_create(1);

	clock_t start, end;
	double time;

	int x;
	int i, j;
	for (i = 0; i < NUM_RUNS; ++i) {
		x = 0;

		start_wall_clock_timer();
		start = clock();
		//sem = dispatch_semaphore_create(1);
		for (j = 0; j <= i; ++j) {
			dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
			x++;
			dispatch_semaphore_signal(sem);
		}
		//dispatch_release(sem);
		end = clock();
		time = wall_clock_seconds();

		assert(x == i+1);
		results[i].cycles = end - start;
		results[i].wall_time = time;
	}

	dispatch_release(sem);
	return results;
}

run_time_t *test_conditional() {
	run_time_t *results = (run_time_t*)malloc(NUM_RUNS*sizeof(run_time_t));
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	clock_t start, end;
	double time;

	int x;
	int i, j;
	for (i = 0; i < NUM_RUNS; ++i) {
		x = 0;

		start_wall_clock_timer();
		start = clock();
		//pthread_mutex_init(&mutex, NULL);
		//pthread_cond_init(&cond, NULL);
		for (j = 0; j <= i; ++j) {
			pthread_mutex_lock(&mutex);
			x++;
			pthread_mutex_unlock(&mutex);
		}
		//pthread_mutex_destroy(&mutex);
		//pthread_cond_destroy(&cond);
		end = clock();
		time = wall_clock_seconds();

		assert(x == i+1);
		results[i].cycles = end - start;
		results[i].wall_time = time;
	}

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	return results;
}

int main() {
	run_time_t *atomic = test_atomic();
	run_time_t *spin = test_spinlock();
	run_time_t *mutex = test_mutex();
	run_time_t *semaphore = test_semaphore();
	run_time_t *conditional = test_conditional();
	
	int i; FILE *f;

	f = fopen("results/overhead/single_init/atomic.txt", "w");
	for (i = 0; i < NUM_RUNS; ++i) {
		fprintf(f, "%ld cycles\t %f seconds\n", atomic[i].cycles, atomic[i].wall_time);
	}

	f = fopen("results/overhead/single_init/spin.txt", "w");
	for (i = 0; i < NUM_RUNS; ++i) {
		fprintf(f, "%ld cycles\t %f seconds\n", spin[i].cycles, spin[i].wall_time);
	}

	f = fopen("results/overhead/single_init/mutex.txt", "w");
	for (i = 0; i < NUM_RUNS; ++i) {
		fprintf(f, "%ld cycles\t %f seconds\n", mutex[i].cycles, mutex[i].wall_time);
	}

	f = fopen("results/overhead/single_init/semaphore.txt", "w");
	for (i = 0; i < NUM_RUNS; ++i) {
		fprintf(f, "%ld cycles\t %f seconds\n", semaphore[i].cycles, semaphore[i].wall_time);
	}

	f = fopen("results/overhead/single_init/conditional.txt", "w");
	for (i = 0; i < NUM_RUNS; ++i) {
		fprintf(f, "%ld cycles\t %f seconds\n", conditional[i].cycles, conditional[i].wall_time);
	}

	free(atomic);
	free(spin);
	free(mutex);
	free(semaphore);
	free(conditional);
}