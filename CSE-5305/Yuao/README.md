Proposal 
In this project, I will implement CPU scheduling algorithm.

To simplify, we will just demonstrate from algorithm perspective.

First of all, I will discuss the application scenario of each algorithm. The discussed algorithms include: FCFS, SJF, Roundrobin.
Suppose only one processor，and i have bunch of calculations/job to do.  FCFS

But right now i have a bunch of users want to use cpu, and the job length varies.
 one is computing intensive, cpu time is very long.
 the other is far less cpu intensive, in order to ensure fairness and improve responsive we adopt SJF
What else job?  Now preemptive scheduling algorithm came out
we ensure fairness between same kind of job, by Roundrobin. time slice but FCFS
what else ?  the policy is sheduled round. but I don't FCFS I would add more intelligence same as SJF, Shortest Remaining job first.

To be continue:
what metrics you would like to provide?
FIFO, Round robin is the basic ones.
How about synchronization, and how about asynchronous IO optimization.


After our discussion, The scheduling algorithm implementation will include two basic ones : FIFO, RoundRobin.
