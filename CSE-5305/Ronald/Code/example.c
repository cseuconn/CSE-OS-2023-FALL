#include "example_tests.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_RUNS 1000

void series(int *array) {
	int i, j;
	for (i = 0; i < ARRAY_SIZE; ++i) {
		array[i] = i * NUM_ITER;
	}
}

void add_time(run_time_t *current_time, run_time_t new_time) {
	current_time->cycles += new_time.cycles;
	current_time->wall_time += new_time.wall_time;
}

int main() {
	int answer[ARRAY_SIZE] = {0};
	series(answer);

	run_time_t atomic = {0, 0};
	run_time_t spin = {0, 0};
	run_time_t mutex = {0, 0};
	run_time_t semaphore = {0, 0};

	for (int i = 0; i < NUM_RUNS; ++i) {
		add_time( &atomic, test_atomic(answer) );
		add_time( &spin, test_spinlock(answer) );
		add_time( &mutex, test_mutex(answer) );
		add_time( &semaphore, test_semaphore(answer) );
	}

	FILE *f = fopen("results/example/times.txt", "w");
	fprintf(f, "atomic: %ld cycles \t %f seconds\n", atomic.cycles / NUM_RUNS, atomic.wall_time / NUM_RUNS);
	fprintf(f, "spinlock: %ld cycles \t %f seconds\n", spin.cycles / NUM_RUNS, spin.wall_time / NUM_RUNS);
	fprintf(f, "mutex: %ld cycles \t %f seconds\n", mutex.cycles / NUM_RUNS, mutex.wall_time / NUM_RUNS);
	fprintf(f, "semaphore: %ld cycles \t %f seconds\n", semaphore.cycles / NUM_RUNS, semaphore.wall_time / NUM_RUNS);
}