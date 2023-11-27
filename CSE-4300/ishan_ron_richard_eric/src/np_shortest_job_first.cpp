#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include "FProcess.h"


using namespace std;

// Function to compare BT
bool compareByBurstTime(const FProcess& a, const FProcess& b) {
    return a.burstTime < b.burstTime;
}
struct CompareByBurstTime {
    bool operator()(const FProcess& a, const FProcess& b) const {
        return a.burstTime > b.burstTime;
    }
};

// Sort by arrival time 
bool compareByArrivalTimeSJF(const FProcess& a, const FProcess& b) {
    return a.arrivalTime < b.arrivalTime;
}

// Shortest Job First
void sjfScheduler(vector<FProcess>& processes) {
    // Sort processes based on burst time (shortest burst time first)
    sort(processes.begin(), processes.end(), compareByArrivalTimeSJF);
    priority_queue<FProcess, vector<FProcess>, CompareByBurstTime> processQueue;

    int currentTime = 0;
    // Execute processes in sorted order

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
            cout << "Executing process " << currentProcess.id << " with burst time " << currentProcess.burstTime << endl;
            currentTime += currentProcess.burstTime;
            cout << "Completing process " << currentProcess.id << " at " << currentTime << endl;
        }
        
    }

    // for (const Process& process : processes) {
    //     cout << "Executing process " << process.id << " with burst time " << process.burstTime << endl;
    //     currentTime += process.burstTime;
    //     cout << "Completing process " << process.id << " at " << currentTime << endl;

    // }
}

// int main() {
//     // Example processes
//     vector<FProcess> processes = {
//         {1, 0, 10},
//         {2, 2, 5},
//         {3, 8, 8},
//         {4, 0, 2}
//     };

//     // Execute SJF scheduler
//     sjfScheduler(processes);

//     return 0;
// }
