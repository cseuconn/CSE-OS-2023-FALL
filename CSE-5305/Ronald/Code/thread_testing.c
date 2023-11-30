#include <assert.h>
#include <pthread.h>

#include "matrix.h"
#include "matrix.c"
#include "unixtimer.c"

#define MAT_SIZE 1000
#define NUM_TESTS 15

typedef struct{
	int start;
	int end;
	TMatrix* m;
	TMatrix* n;
	TMatrix* result;
}thread_info;

void MultMat_threads(void* arg){
	thread_info* my_data = (thread_info*)arg;
	TMatrix *m = my_data->m, *n = my_data->n;
	TMatrix *result = my_data->result;
	
	for(int i = my_data->start; i < my_data->end; i++){
		for(int k = 0; k < m->ncol; k++){
			for(int j = 0; j < n->ncol; j++){
				result->data[i][j] += m->data[i][k] * n->data[k][j];
			}
		}
	}
}

double thread_test(int num_threads) {
	assert(num_threads < MAT_SIZE);

	// Randomly generate two matrices and multiply them in series
	TMatrix *m = newMatrix(MAT_SIZE, MAT_SIZE);
	TMatrix *n = newMatrix(MAT_SIZE, MAT_SIZE);

	fillMatrix(m);
	fillMatrix(n);

	TMatrix *answer = mulMatrix(m, n);

	// Create result matrix
	TMatrix *result = newMatrix(m->nrow, n->ncol);
	memset( &(result->data[0][0]), 0, sizeof(TElement) * result->nrow * result->ncol);

	// Setup threads
	int part = MAT_SIZE / num_threads;
	pthread_t tid[num_threads];
	thread_info threads[num_threads];

	int i;
	start_wall_clock_timer();
	for (i = 0; i < num_threads; ++i) {
		threads[i].start = i * part;
		threads[i].end = (i+1) * part;
		threads[i].m = m;
		threads[i].n = n;
		threads[i].result = result;

		if(i == num_threads - 1){
    		threads[i].end = MAT_SIZE;
    	}

		pthread_create(&tid[i], NULL, (void*)MultMat_threads, (void*)&threads[i]);
	}

	for (i = 0; i < num_threads; ++i) {
		pthread_join(tid[i], NULL);
	}
	double time = wall_clock_seconds();

	// Assert correct answer from threads
	int ret = compareMatrix(answer, result);
	assert( ret == 0 );
	
	return time;
}

int main() {
	double results[NUM_TESTS];
	
	int i;
	for (i = 0; i < NUM_TESTS; ++i) {
		printf("%d\n", i);
		results[i] = thread_test(i+1);
	}

	FILE *f = fopen("results/threads/times.txt", "w");
	for (i = 0; i < NUM_TESTS; ++i) {
		fprintf(f, "%f seconds\n", results[i]);
	}
}