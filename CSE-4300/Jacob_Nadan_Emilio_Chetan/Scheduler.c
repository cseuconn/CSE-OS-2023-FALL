#include "Scheduler.h"


int executeProcess(Task *t){
    t->burst_time -= TIME_QUANTUM;
    int pid = t->pid;
    printf("Task %d executed\n", pid);
    return t->burst_time;
}

Queue *orderTasks(Task* arr[], int len){
    Queue *q = createQueue();
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len - i; j++) {
            if (arr[j]->arrival_time > arr[j + 1]->arrival_time) {
                Task *temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j+ 1] = temp;
            }
        }
    }

    for(int x = 0; x < len; x++){
        enqueue(q, arr[x]);
    }
    return q;
}


void roundRobinScheduler(Queue* q) {
    // create timer and set to 0
    int time = 0;
    Queue *finished = createQueue();
    while(1){ // loop indefinetly
        Task *currentTask = q->front; // current task
        if (!isEmpty(q) && time >= currentTask->arrival_time){ // idle waiting for tasks to arrive
            if(currentTask->response_time < 0){
                currentTask->response_time = time;
            }
            int res = executeProcess(currentTask); // execute current process
            if (res <= 0){ // if burst time <= 0 dequeue task
                time += TIME_QUANTUM + res; // res is <= 0 so time will be incremented by time quantum or less
                currentTask->turnaround_time = time;
                dequeue(q);
                enqueue(finished, currentTask);
                printf("Task %d finished\n", currentTask->pid);
            }
            else{
                dequeue(q);
                enqueue(q, currentTask); // move task to end
                time += TIME_QUANTUM;
            }

        }
        else if (isEmpty(q)){
            printQueue(finished);
            freeQueue(finished);
            freeQueue(q);
            return;
        }
        else{
            time++; // increment timer
        }
        usleep(200000); // sleep to make sure loops aren't happening to fast
    }
}