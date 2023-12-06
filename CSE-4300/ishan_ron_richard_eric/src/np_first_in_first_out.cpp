#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include "FProcess.h"

using namespace std;

// Sort by arrival time 
bool compareByArrivalTime(const FProcess& a, const FProcess& b) {
    return a.arrivalTime < b.arrivalTime;
}

void fifoScheduler(vector<FProcess>& processes) {
    // Sort processes based on arrival time
    sort(processes.begin(), processes.end(), compareByArrivalTime);

    queue<FProcess> processQueue;

    // Push to queue after sorting S
    for (const FProcess& process : processes) {
        processQueue.push(process);
    }

    int currentTime = 0;
    while (!processQueue.empty()) {
        FProcess currentProcess = processQueue.front();
        processQueue.pop();

        cout << "Executing process " << currentProcess.id << " with burst time " << currentProcess.burstTime << endl;
        currentTime += currentProcess.burstTime;
        cout << "Completing process " << currentProcess.id << " at " << currentTime << endl;
    }
}

// int main() {
    
//     vector<FProcess> processes = {
//         {1, 0, 10},
//         {2, 2, 5},
//         {3, 8, 8},
//         {4, 1, 2}
//     };

//     // Execute FIFO scheduler with sorted arrival times
//     fifoScheduler(processes);

//     return 0;
// }
