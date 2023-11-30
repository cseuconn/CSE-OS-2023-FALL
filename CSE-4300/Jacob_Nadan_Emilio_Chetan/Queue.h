#ifndef QUEUE_H
#define QUEUE_H

typedef struct Task {
    int pid;
    int burst_time;
    int initial_bt;
    int arrival_time;
    int turnaround_time;
    int response_time;
    struct Task* next;
} Task;

typedef struct Queue {
    Task* front;
    Task* rear;
} Queue;

Task *createTask(int pid, int burst_time, int arrival_time);

Queue *createQueue();

int isEmpty(Queue* q);

void enqueue(Queue* q, Task* t);

int dequeue(Queue* q);

void freeQueue(Queue *q);

void printQueue(Queue* q);

#endif