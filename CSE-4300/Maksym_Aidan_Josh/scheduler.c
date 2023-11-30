/*
 * Scheduler.
 *
 * The default scheduler is very simple, just a round-robin run queue.
 * You'll want to improve it.
 */

#include <types.h>
#include <lib.h>
#include <scheduler.h>
#include <thread.h>
#include <machine/spl.h>
#include <queue.h>
#include <clock.h>

/*
 *  Scheduler data
 */

// Queues of runnable threads
static struct queue *currentqueue;
static struct queue *highpriorityqueue;
static struct queue *mediumpriorityqueue;
static struct queue *lowpriorityqueue;
extern int lbolt_counter;
// extern int reset_counter;
static int prev_counter;
/*
 * Setup function
 */
void
scheduler_bootstrap(void)
{
	highpriorityqueue = q_create(32);
	mediumpriorityqueue = q_create(32);
	lowpriorityqueue = q_create(32);
	currentqueue = highpriorityqueue;
	if (highpriorityqueue == NULL) {
		panic("scheduler: Could not create high priority queue\n");
	}
	if (mediumpriorityqueue == NULL) {
		panic("scheduler: Could not create medium priority queue\n");
	}
	if (lowpriorityqueue == NULL) {
		panic("scheduler: Could not create low priority queue\n");
	}
}

/*
 * Ensure space for handling at least NTHREADS threads.
 * This is done only to ensure that make_runnable() does not fail -
 * if you change the scheduler to not require space outside the 
 * thread structure, for instance, this function can reasonably
 * do nothing.
 */
int
scheduler_preallocate(int nthreads)
{
	assert(curspl>0);
	int res1, res2, res3;
	res1 = q_preallocate(highpriorityqueue, nthreads);
	res2 = q_preallocate(mediumpriorityqueue, nthreads);
	res3 = q_preallocate(lowpriorityqueue, nthreads);
	return res1 || res2 || res3;
}

/*
 * This is called during panic shutdown to dispose of threads other
 * than the one invoking panic. We drop them on the floor instead of
 * cleaning them up properly; since we're about to go down it doesn't
 * really matter, and freeing everything might cause further panics.
 */
void
scheduler_killall(void)
{
	assert(curspl>0);
	while (!q_empty(highpriorityqueue)) {
		struct thread *t = q_remhead(highpriorityqueue);
		kprintf("scheduler: Dropping thread %s.\n", t->t_name);
	}
	while (!q_empty(mediumpriorityqueue)) {
		struct thread *t = q_remhead(mediumpriorityqueue);
		kprintf("scheduler: Dropping thread %s.\n", t->t_name);
	}
	while (!q_empty(lowpriorityqueue)) {
		struct thread *t = q_remhead(lowpriorityqueue);
		kprintf("scheduler: Dropping thread %s.\n", t->t_name);
	}
}

/*
 * Cleanup function.
 *
 * The queue objects to being destroyed if it's got stuff in it.
 * Use scheduler_killall to make sure this is the case. During
 * ordinary shutdown, normally it should be.
 */
void
scheduler_shutdown(void)
{
	scheduler_killall();

	assert(curspl>0);
	q_destroy(highpriorityqueue);
	q_destroy(mediumpriorityqueue);
	q_destroy(lowpriorityqueue);
	currentqueue = NULL;
	highpriorityqueue = NULL;
	mediumpriorityqueue = NULL;
	lowpriorityqueue = NULL;
}

/*
 * Actual scheduler. Returns the next thread to run.  Calls cpu_idle()
 * if there's nothing ready. (Note: cpu_idle must be called in a loop
 * until something's ready - it doesn't know whether the things that
 * wake it up are going to make a thread runnable or not.) 
 */
struct thread *
scheduler(void)
{
	// meant to be called with interrupts off
	assert(curspl>0);
	
	while (q_empty(highpriorityqueue) && q_empty(mediumpriorityqueue) && q_empty(lowpriorityqueue)) {
		cpu_idle();
	}

	// You can actually uncomment this to see what the scheduler's
	// doing - even this deep inside thread code, the console
	// still works. However, the amount of text printed is
	// prohibitive.
	// 
	//print_run_queue();
	// logic for boosting all threads to highest priority queue:
	if (lbolt_counter != prev_counter) {
		prev_counter = lbolt_counter;
		while (!q_empty(mediumpriorityqueue)) {
			struct thread *t = q_remhead(mediumpriorityqueue);
			t->priority = 2;
			q_addtail(highpriorityqueue, t);
			//kprintf("scheduler: Priority boost for thread %s.\n", t->t_name);
		}
		while (!q_empty(lowpriorityqueue)) {
			struct thread *t = q_remhead(lowpriorityqueue);
			t->priority = 2;
			q_addtail(highpriorityqueue, t);
			//kprintf("scheduler: Priority boost for thread %s.\n", t->t_name);
		}
	}

/*	if (reset_counter >= 5) {
		reset_counter = 0;    THIS WILL NOT WORK, SINCE HARDCLOCK DOES NOT HAVE EXTERN INT
		int i = 0;
		while (!q_empty(mediumpriorityqueue) && i < 5) {
			struct thread *t = q_remhead(mediumpriorityqueue);
			t->priority = 2;
			q_addtail(highpriorityqueue, t);
		}
		i = 0;
		while (!q_empty(lowpriorityqueue) && i < 3) {
			struct thread *t = q_remhead(lowpriorityqueue);
			t->priority = 2
			q_addtail(highpriorityqueue, t);
		}
	}		
*/

	if (!q_empty(highpriorityqueue)) {
		currentqueue = highpriorityqueue;
		clock_quantum = 50;
	} else if (!q_empty(mediumpriorityqueue)) {
		currentqueue = mediumpriorityqueue;
		clock_quantum = 100;
	} else {
		currentqueue = lowpriorityqueue;
		clock_quantum = 1000;
	}
	
	return q_remhead(currentqueue);
}

/* 
 * Make a thread runnable.
 * With the base scheduler, just add it to the end of the run queue.
 */
int
make_runnable(struct thread *t)
{
	// meant to be called with interrupts off
	assert(curspl>0);

	if (t->priority == 2) {
		return q_addtail(highpriorityqueue, t);
	} else if (t->priority == 1) {
		return q_addtail(mediumpriorityqueue, t);
	} else {
		return q_addtail(lowpriorityqueue, t);
	}
}

/*
 * Debugging function to dump the run queue.
 */
void
print_run_queue(void)
{
	/* Turn interrupts off so the whole list prints atomically. */
	int spl = splhigh();

	int i,k=0;
	i = q_getstart(currentqueue);
	
	while (i!=q_getend(currentqueue)) {
		struct thread *t = q_getguy(currentqueue, i);
		kprintf("  %2d: %s %p\n", k, t->t_name, t->t_sleepaddr);
		i=(i+1)%q_getsize(currentqueue);
		k++;
	}
	
	splx(spl);
}
