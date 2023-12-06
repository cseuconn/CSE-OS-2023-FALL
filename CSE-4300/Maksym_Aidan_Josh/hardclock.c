#include <types.h>
#include <lib.h>
#include <machine/spl.h>
#include <thread.h>
#include <clock.h>
#include <queue.h>

/* 
 * The address of lbolt has thread_wakeup called on it once a second.
 */
int lbolt;

int lbolt_counter;

// The following is considered bad practice, but it achieves the correct functionality
// int reset_counter;

int clock_quantum = 100;

/*
 * This is called HZ times a second by the timer device setup.
 */

void
hardclock(void)
{
	/*
	 * Collect statistics here as desired.
	 */
	// HZ is being replaced by global bariable clock_quantum.
	
	lbolt_counter++;
	if (lbolt_counter >= clock_quantum) {
		lbolt_counter = 0;
	//  reset_counter++;
		thread_wakeup(&lbolt);
	}

	thread_yield();
}

/*
 * Suspend execution for n seconds.
 */
void
clocksleep(int num_secs)
{
	int s;

	s = splhigh();
	while (num_secs > 0) {
		thread_sleep(&lbolt);
		num_secs--;
	}
	splx(s);
}
