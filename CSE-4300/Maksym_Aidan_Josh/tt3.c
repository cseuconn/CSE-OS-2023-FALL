/*
 * More thread test code.
 */
#include <types.h>
#include <lib.h>
#include <machine/spl.h>
#include <synch.h>
#include <thread.h>
#include <test.h>
#include <clock.h>

#include "opt-synchprobs.h"

/* dimension of matrices (cannot be too large or will overflow stack) */

#if OPT_SYNCHPROBS
#define DIM 10
#else
#define DIM 70
#endif

/* number of iterations for sleepalot threads */
#define SLEEPALOT_PRINTS      20	/* number of printouts */
#define SLEEPALOT_ITERS       4		/* iterations per printout */
/* polling frequency of waker thread */
#define WAKER_WAKES          100
/* number of iterations per compute thread */
#define COMPUTE_ITERS         10

#define NWAITADDRS 12
static const char waitaddrs[NWAITADDRS];  /* N distinct wait addresses */

static volatile int wakerdone;
static struct semaphore *wakersem;
static struct semaphore *donesem;

// Struct for keeping each thread's starting time.
struct starting_time {
	time_t secs;
	u_int32_t nsecs;
} s_time;

static
void
setup(void)
{
	if (wakersem == NULL) {
		wakersem = sem_create("wakersem", 1);
	}
	if (donesem == NULL) {
		donesem = sem_create("donesem", 0);
	}
	wakerdone = 0;
}

static
void
sleepalot_thread(void *junk, unsigned long num)
{
	int i, j, spl;

	time_t time_secs, interval_secs;
	u_int32_t time_nsecs, interval_nsecs;

	struct starting_time *s_times = (struct starting_time*) junk;
	
	// Response time calculated here.
	gettime(&time_secs, &time_nsecs);
	getinterval(s_times->secs, s_times->nsecs, 
		time_secs, time_nsecs,
		&interval_secs, &interval_nsecs);	
	kprintf("Sleepalot %lu response time: %lu.%09lu s\n",
		(unsigned long) num,
		(unsigned long) interval_secs,
		(unsigned long) interval_nsecs);
	

	for (i=0; i<SLEEPALOT_PRINTS; i++) {
		for (j=0; j<SLEEPALOT_ITERS; j++) {
			spl = splhigh();
			thread_sleep(&waitaddrs[random()%NWAITADDRS]);
			splx(spl);
		}
		kprintf("[%lu]", num);
	}

	// Turnaround time calculated here.
	gettime(&time_secs, &time_nsecs);
	getinterval(s_times->secs, s_times->nsecs, 
		time_secs, time_nsecs,
		&interval_secs, &interval_nsecs);	
	kprintf("Sleepalot %lu turnaround time: %lu.%09lu s\n",
		(unsigned long) num,
		(unsigned long) interval_secs,
		(unsigned long) interval_nsecs);

	V(donesem);
}

static
void
waker_thread(void *junk1, unsigned long junk2)
{
	int i, spl, done;

	(void)junk1;
	(void)junk2;

	while (1) {
		P(wakersem);
		done = wakerdone;
		V(wakersem);
		if (done) {
			break;
		}

		for (i=0; i<WAKER_WAKES; i++) {
			spl = splhigh();
			thread_wakeup(&waitaddrs[random()%NWAITADDRS]);
			splx(spl);

			thread_yield();
		}
	}
	V(donesem);
}

static
void
make_sleepalots(int howmany)
{
	char name[16];
	int i, result;

	struct starting_time start_time[howmany];
	
	for (i=0; i<howmany; i++) {
		snprintf(name, sizeof(name), "sleepalot%d", i);

		gettime(&start_time[i].secs, &start_time[i].nsecs);		

		result = thread_fork(name, &start_time[i], i, sleepalot_thread, NULL);
		if (result) {
			panic("thread_fork failed: %s\n", strerror(result));
		}
	}
	
	result = thread_fork("waker", NULL, 0, waker_thread, NULL);
	if (result) {
		panic("thread_fork failed: %s\n", strerror(result));
	}
}

static
void
compute_thread(void *junk1, unsigned long num)
{
	struct matrix {
		char m[DIM][DIM];
	};
	struct matrix *m1, *m2, *m3;
	unsigned char tot;
	int i, j, k, m;
	u_int32_t rand;

	time_t time_secs, interval_secs;
	u_int32_t time_nsecs, interval_nsecs;

	struct starting_time *s_times = (struct starting_time*) junk1;

	// Response time calculated here.
	gettime(&time_secs, &time_nsecs);
	getinterval(s_times->secs, s_times->nsecs, 
		time_secs, time_nsecs,
		&interval_secs, &interval_nsecs);	
	kprintf("Compute %lu response time: %lu.%09lu s\n",
		(unsigned long) num,
		(unsigned long) interval_secs,
		(unsigned long) interval_nsecs);
	
	m1 = kmalloc(sizeof(struct matrix));
	assert(m1 != NULL);
	m2 = kmalloc(sizeof(struct matrix));
	assert(m2 != NULL);
	m3 = kmalloc(sizeof(struct matrix));
	assert(m3 != NULL);

	for (m=0; m<COMPUTE_ITERS; m++) {

		for (i=0; i<DIM; i++) {
			for (j=0; j<DIM; j++) {
				rand = random();
				m1->m[i][j] = rand >> 16;
				m2->m[i][j] = rand & 0xffff;
			}
		}
		
		for (i=0; i<DIM; i++) {
			for (j=0; j<DIM; j++) {
				tot = 0;
				for (k=0; k<DIM; k++) {
					tot += m1->m[i][k] * m2->m[k][j];
				}
				m3->m[i][j] = tot;
			}
		}
		
		tot = 0;
		for (i=0; i<DIM; i++) {
			tot += m3->m[i][i];
		}

		kprintf("{%lu: %u}", num, (unsigned) tot);
		thread_yield();
	}

	kfree(m1);
	kfree(m2);
	kfree(m3);

	// Turnaround time calculated here.
	gettime(&time_secs, &time_nsecs);
	getinterval(s_times->secs, s_times->nsecs, 
		time_secs, time_nsecs,
		&interval_secs, &interval_nsecs);	
	kprintf("Compute %lu turnaround time: %lu.%09lu s\n",
		(unsigned long) num,
		(unsigned long) interval_secs,
		(unsigned long) interval_nsecs);

	V(donesem);
}

static
void
make_computes(int howmany)
{
	char name[16];
	int i, result;

	struct starting_time start_time[howmany]; 

	for (i=0; i<howmany; i++) {
		snprintf(name, sizeof(name), "compute%d", i);
		
		gettime(&start_time[i].secs, &start_time[i].nsecs);
		
		result = thread_fork(name, &start_time[i], i, compute_thread, NULL);
		if (result) {
			panic("thread_fork failed: %s\n", strerror(result));
		}
	}
}

static
void
finish(int howmanytotal)
{
	int i;
	for (i=0; i<howmanytotal; i++) {
		P(donesem);
	}
	P(wakersem);
	wakerdone = 1;
	V(wakersem);
	P(donesem);
}

static
void
runtest3(int nsleeps, int ncomputes)
{
	setup();
	kprintf("Starting thread test 3 (%d [sleepalots], %d {computes}, "
		"1 waker)\n",
		nsleeps, ncomputes);
	make_sleepalots(nsleeps);
	make_computes(ncomputes);
	finish(nsleeps+ncomputes);
	kprintf("\nThread test 3 done\n");
}

int
threadtest3(int nargs, char **args)
{
	if (nargs==1) {
		runtest3(5, 2);
	}
	else if (nargs==3) {
		runtest3(atoi(args[1]), atoi(args[2]));
	}
	else {
		kprintf("Usage: tt3 [sleepthreads computethreads]\n");
		return 1;
	}
	return 0;
}
