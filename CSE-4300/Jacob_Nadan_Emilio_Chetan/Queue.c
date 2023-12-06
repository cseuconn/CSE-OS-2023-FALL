#include <stdio.h>
#include <stdlib.h>
#include "Queue.h"

Task *createTask(int pid, int burst_time, int arrival_time) {
    Task *task = (Task *)malloc(sizeof(Task));
    task->pid = pid;
    task->burst_time = burst_time; // set time passed to 0 because the task hasn't done anything yet
    task->initial_bt = burst_time;
    task->arrival_time = arrival_time;
    task->turnaround_time = 0;
    task->response_time = -1;
    task->next = NULL;
    return task;  // Return the created node
}

Queue *createQueue(){
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
}

int isEmpty(Queue *q){
    return q->front == NULL;
}

void enqueue(Queue *q, Task *t){
    if (isEmpty(q)){
        q->front = q->rear = t;
    }
    else{
        t->next = NULL;
        q->rear->next = t;
        q->rear = t;
    }
}

int dequeue(Queue *q){
    if (isEmpty(q)){
        return -1;
    }
    else{
        Task *temp = q->front;
        int pid = temp->pid;
        q->front = temp->next;

        if (q->front == NULL){
            q->rear = NULL;
        }
        return pid;
    }
}

void freeQueue(Queue *q) {
    while (!isEmpty(q)) {
        Task *temp = q->front;
        q->front = temp->next;
        free(temp);
    }
    free(q);
}

void printQueue(Queue* q) {
    Task *t = q->front;
    int sum_tat = 0;
    int sum_res = 0;
    int len = 0;
    printf("Process ID      Burst Time      Arrival Time    Turnaround Time Response Time\n");
    while (t != NULL) {
        sum_tat += t->turnaround_time;
        sum_res += t->response_time;
        len++;
        printf("%-16d%-16d%-16d%-16d%-16d\n\n\n"
        , t->pid, t->initial_bt, t->arrival_time, t->turnaround_time, t->response_time);
        t = t->next;
    }
    printf("Average Turnaround Time: %.2f\n", (float)sum_tat / len);
    printf("Average Response Time: %.2f\n\n", (float)sum_res / len);

}