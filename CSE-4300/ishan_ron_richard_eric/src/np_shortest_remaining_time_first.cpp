#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include "FProcess.h"

using namespace std;

struct CompareByBurstTime
{
    bool operator()(const FProcess &a, const FProcess &b) const
    {
        return a.burstTime > b.burstTime;
    }
};

void srtfScheduler(vector<FProcess>& processes) {
    priority_queue<FProcess, vector<FProcess>, CompareByBurstTime> processQueue;
    int currentTime = 0;

    while (!processes.empty() || !processQueue.empty()) {
        // Move processes to the queue if they have arrived
        while (!processes.empty() && processes.front().arrivalTime <= currentTime) {
            processQueue.push(processes.front());
            processes.erase(processes.begin());
        }

        // If the queue is not empty, execute process with the shortest burst time
        if (!processQueue.empty()) {
            FProcess currentProcess = processQueue.top();
            processQueue.pop();

            // Check the minimum of the currentProcess's remaining time to the process that has just arrived 
            // First check if there are any processes left in the queue, and if queue is empty, there are no more processes arriving
            // If there are no more processes, then the curren process run for its remaining time
            // Otherwise, calculate the time until the next process arrives
            int executeTime = min(currentProcess.burstTime, processes.empty() ? currentProcess.burstTime : processes.front().arrivalTime - currentTime);
            cout << "Executing process " << currentProcess.id << " for " << executeTime << " units" << endl;

            // Update burst time for the current process
            currentProcess.burstTime -= executeTime;

            // If the process still has burst time remaining, add it back to the queue
            if (currentProcess.burstTime > 0) {
                processQueue.push(currentProcess);
            }
           

            currentTime += executeTime;
            if (currentProcess.burstTime <= 0) {
                cout << "Completing process " << currentProcess.id << " at " << currentTime << endl;
            }
        } else {
            // If the queue is empty, advance time to the next process arrival
            currentTime = processes.front().arrivalTime;
        }
    }
}

// int main() {
//     // Example processes with different arrival times
//     vector<FProcess> processes = {
//         {1, 0, 12},
//         {2, 2, 5},
//         {3, 4, 8},
//         {4, 7, 2}
//     };

//     // Execute SRTF scheduler 
//     srtfScheduler(processes);

//     return 0;
// }
