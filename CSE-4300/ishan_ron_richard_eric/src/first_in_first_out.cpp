#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>

#include "MLFQ_Process.h"

using namespace std;

// Sort by arrival time 
bool compareByArrivalTime(const Process& a, const Process& b) {
    return a.getArrivalTime() < b.getArrivalTime();
}

void fifoScheduler(vector<Process>& processes) {
    // Sort processes based on arrival time
    sort(processes.begin(), processes.end(), compareByArrivalTime);

    queue<Process> processQueue;

    // Push to queue after sorting 
    for (const Process& process : processes) {
        processQueue.push(process);
    }

    while (!processQueue.empty()) {
        Process currentProcess = processQueue.front();
        processQueue.pop();

        if (!currentProcess.getJobs().empty()) { //if there is no more jobs in the current process queue 
            Job currentJob = currentProcess.getJobs().front(); //gets first job from job vector
            cout << "Executing " << currentProcess.getName() << " with job time " << currentJob.first << "ms" << endl;
        }
    }
}

/* int main() {
    vector<Process> processes;
    processes.push_back(Process("Process 1", 5, {{10, "CPU"}}));
    processes.push_back(Process("Process 2", 2, {{5, "CPU"}}));
    processes.push_back(Process("Process 3", 8, {{8, "CPU"}}));
    processes.push_back(Process("Process 4", 1, {{2, "CPU"}}));

    // Execute FIFO scheduler with sorted arrival times
    fifoScheduler(processes);

    return 0;
} */
