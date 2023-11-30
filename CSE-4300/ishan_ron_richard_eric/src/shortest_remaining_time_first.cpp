#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

#include "MLFQ_Process.h"

using namespace std;

struct CompareByBurstTime {
    bool operator()(const Process& a, const Process& b) const {
        return a.getCurJobTime() > b.getCurJobTime();
    }
};

void srtfScheduler(vector<Process>& processes) {
    priority_queue<Process, vector<Process>, CompareByBurstTime> processQueue;
    int currentTime = 0;

     // Sort processes by arrival time
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.getArrivalTime() < b.getArrivalTime();
    });

    while (!processes.empty() || !processQueue.empty()) {
        // Move processes to the queue if they have arrived
        while (!processes.empty() && processes.front().getArrivalTime() <= currentTime) {
            processQueue.push(processes.front());
            processes.erase(processes.begin());
        }

        // If the queue is not empty, execute process with the shortest burst time
        if (!processQueue.empty()) {
            Process currentProcess = processQueue.top();
            processQueue.pop();

            // Check the minimum of the currentProcess's remaining time to the process that has just arrived 
            // First check if there are any processes left in the queue, and if queue is empty, there are no more processes arriving
            // If there are no more processes, then the curren process run for its remaining time
            // Otherwise, calculate the time until the next process arrives
            int nextArrivalTime = processes.empty() ? INT_MAX : processes.front().getArrivalTime();
            int executeTime = min(currentProcess.getCurJobTime(), nextArrivalTime - currentTime);
            cout << "Executing " << currentProcess.getName() << " for " << executeTime << " units" << endl;

            // Update burst time for the current process
            currentProcess.decrementJobTime(executeTime);

            // If the process still has burst time remaining, add it back to the queue
            if (!currentProcess.isFinished()) {
                processQueue.push(currentProcess);
            }

            currentTime += executeTime;
        } else {
            // If the queue is empty, advance time to the next process arrival
            currentTime = processes.empty() ? currentTime : processes.front().getArrivalTime();
        }
    }
}

/* int main() {
    vector<Process> processes = {
        Process("Process 1", 0, {{12, "CPU"}}),
        Process("Process 2", 2, {{5, "CPU"}}),
        Process("Process 3", 4, {{8, "CPU"}}),
        Process("Process 4", 7, {{2, "CPU"}})
    };

    // Execute SRTF scheduler 
    srtfScheduler(processes);

    return 0;
}
 */