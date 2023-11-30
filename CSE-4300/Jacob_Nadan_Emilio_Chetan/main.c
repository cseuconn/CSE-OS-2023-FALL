#include "Scheduler.h"
#include "Queue.h"

int main(){
    Task* tasks[6];

    tasks[0] = createTask(1, 3, 7);

    tasks[1] = createTask(2, 4, 5);

    tasks[2] = createTask(3, 2, 2);

    tasks[3] = createTask(4, 2, 3);
 
    tasks[4] = createTask(5, 4, 1);

    tasks[5] = createTask(6, 2, 0);

    Queue *q = orderTasks(tasks, 6);

    roundRobinScheduler(q);
    return 0;
}