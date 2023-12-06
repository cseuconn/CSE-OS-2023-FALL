#include <iostream>
#include <vector>
#include <algorithm>

#include "MLFQ_Process.h"

using namespace std;

// Function to compare BT
bool compareByBurstTime(const Process& a, const Process& b) {
    return a.getJobs().front().first < b.getJobs().front().first; //burst time associated with first element in job vector (ms, CPU/IO)
}

// Shortest Job First
void sjfScheduler(vector<Process>& processes) {
    // Sort processes based on burst time (shortest burst time first)
    sort(processes.begin(), processes.end(), compareByBurstTime);

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
    // Example processes
    vector<Process> processes;
    processes.push_back(Process("Process 1", 0, {{6, "CPU"}}));
    processes.push_back(Process("Process 2", 2, {{8, "CPU"}}));
    processes.push_back(Process("Process 3", 4, {{7, "CPU"}}));
    processes.push_back(Process("Process 4", 7, {{3, "CPU"}}));

    // Execute SJF scheduler
    sjfScheduler(processes);

    return 0;
}
 */

//job.first for the first element in the pair ms 
//will there only be one job vector per process?