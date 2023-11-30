#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Queue.h"
#define TIME_QUANTUM 2

int executeProcess(Task *t);
void roundRobinScheduler(Queue* q);
void swap(Task **a, Task **b);
Queue *orderTasks(Task* arr[], int len);


#endif