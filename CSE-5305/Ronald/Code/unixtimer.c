// Provided to students in CSE 3100

#include <sys/time.h>

void start_CPU_timer()
{
  struct itimerval pval, poval;

  /*Now set the process timer*/
  pval.it_value.tv_sec = 1000000;
  pval.it_value.tv_usec = 0;
  pval.it_interval.tv_sec = 0;
  pval.it_interval.tv_usec = 0;
  setitimer(ITIMER_PROF, &pval, &poval);
}

void start_wall_clock_timer()
{
  struct itimerval pval, poval;

  /*Now set the process timer*/
  pval.it_value.tv_sec = 1000000;
  pval.it_value.tv_usec = 0;
  pval.it_interval.tv_sec = 0;
  pval.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &pval, &poval);
}


double cpu_seconds()
{
  struct itimerval val;
  long secs, usecs;
  double temp;

  getitimer(ITIMER_PROF, &val);
  secs = 1000000 - val.it_value.tv_sec - 1;
  usecs = 1000000 - val.it_value.tv_usec;

  temp = (double) secs + ((double) usecs/1000000.0);

  return(temp > 0 ? temp : 0.0 );
}


double wall_clock_seconds()
{
  struct itimerval val;
  long secs, usecs;
  double temp;

  getitimer(ITIMER_REAL, &val);
  secs = 1000000 - val.it_value.tv_sec - 1;
  usecs = 1000000 - val.it_value.tv_usec;

  temp = (double) secs + ((double) usecs/1000000.0);

  return(temp > 0 ? temp : 0.0 );
}

