#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include "FProcess.h"


using namespace std;

// Round Robin Scheduling Algorithm, Parameters: A queue of FProcesses, Time Quantum
// Note: probably need some sort of timer that keeps track of every time quantum in case of interrupts/new processes entering/etc
void roundRobinScheduler(vector<FProcess>& processes, int timeQuantum) {
    // First sort the processes based on AT
    sort(processes.begin(), processes.end(), [](const FProcess& a, const FProcess& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    queue<FProcess> processQueue;
    int currentTime = 0;


    while (!processes.empty() || !processQueue.empty()) {          // Keeps iterating through the queue of processes until no more processes
        // Move processes to the queue if they have arrived
        while (!processes.empty() && processes.front().arrivalTime <= currentTime) {
            processQueue.push(processes.front());       // Push to Queue
            processes.erase(processes.begin());         // Erase from the list of processes
        }

       
        
       
       // If the queue is not empty, execute processes
        if (!processQueue.empty()) {
            FProcess currentProcess = processQueue.front();      // Head of queue is the process to be executed
            processQueue.pop();                                 // Take it out of the Queue

            // Simulate execution for time quantum
            
            if (currentProcess.burstTime > timeQuantum) {
                cout << "Executing process " << currentProcess.id << " for time quantum " << timeQuantum << endl;
                currentProcess.burstTime -= timeQuantum;
                currentTime += timeQuantum;
                processQueue.push(currentProcess); // Add back to the queue for the next turn
            } else {
                cout << "Executing process " << currentProcess.id << " for " << currentProcess.burstTime << " units" << endl;
                currentTime += currentProcess.burstTime;        // Update current time.
            }
        } else {
            // If the queue is empty, advance time to the next process arrival
            currentTime = processes.front().arrivalTime;
        }
    }
}

// int main() {
//     // Example processes
//     vector<FProcess> processes = {
//         {1, 3, 10},
//         {2, 0, 5},
//         {3, 5, 8},
//         {4, 8, 2}
//     };

//     // Time quantum for Round Robin
//     int timeQuantum = 3;

//     // Execute Round Robin scheduler
//     roundRobinScheduler(processes, timeQuantum);

//     return 0;

//     return 0;
// }
