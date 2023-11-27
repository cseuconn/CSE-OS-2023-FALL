#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct FPProcess {
    int id;
    int arrivalTime;
    int burstTime;
    int priority; 

    FPProcess(int _id, int _arrivalTime, int _burstTime, int _priority)
        : id(_id), arrivalTime(_arrivalTime), burstTime(_burstTime), priority(_priority) {}
};



bool compareByArrivalTime(const FPProcess& a, const FPProcess& b) {
    return a.arrivalTime < b.arrivalTime;
}

struct CompareByRemainingTime {
    bool operator()(const FPProcess& a, const FPProcess& b) const {
        return a.burstTime > b.burstTime;
    }
};

bool compareByBurstTime(const FPProcess& a, const FPProcess& b) {
    return a.burstTime < b.burstTime;
}



FPProcess currentProcess = {-1, -1, -1, -1};
int currentPriority = -1;

void mlqScheduler(vector<FPProcess>& processes) {
    // Sort processes based on arrival time
    sort(processes.begin(), processes.end(), compareByArrivalTime);

    // 4 Queues
    queue<FPProcess> rrQueue;                                                     // Round Robin = 1st Priority
    priority_queue<FPProcess, vector<FPProcess>, CompareByRemainingTime> srtfQueue; // SRTF = 2nd Priority
    vector<FPProcess> sjfQueue;                                                   // SJF = 3rd Priority
    vector<FPProcess> fifoQueue;                                                  // FIFO = Last Priority

    int currentTime = 0;        // Keeps track of time

    int RRTime = 0;             // Keeps track of a RR's process time being ran
    int SRTFTime = 0;           // Keeps track of a SRTF's process time being ran
    int SJFTime = 0;            // Keeps track of a SJF's process time being ran
    int FIFOTime = 0;           // Keeps track of a FIFO's process time being ran

    // Loop until all processes and queues are done.
    while (!processes.empty() || !rrQueue.empty() || !srtfQueue.empty() || !sjfQueue.empty() || !fifoQueue.empty()) {
        // Move processes to the appropriate queues if they have arrived
        while (!processes.empty() && processes.front().arrivalTime <= currentTime) {
            int priority = processes.front().priority;

            switch (priority) {
                case 1: // Highest priority (Round Robin)
                    rrQueue.push(processes.front());
                    break;
                case 2: // Next priority (SRTF)
                    srtfQueue.push(processes.front());
                    break;
                case 3: // Next priority (SJF)
                    sjfQueue.push_back(processes.front());
                    sort(sjfQueue.begin(), sjfQueue.end(), compareByBurstTime);
                    break;
                default: // Lowest priority (FIFO)
                    fifoQueue.push_back(processes.front());
                    sort(fifoQueue.begin(), fifoQueue.end(), compareByArrivalTime);

            }
            // Remove from processes since it's placed in queue
            processes.erase(processes.begin());
        }

        // Execute processes based on scheduling algorithms

        // Highest priority queue first (Round Robin)
        if (!rrQueue.empty()) {
            currentPriority = currentProcess.priority;
            // cout << "Current Process ID: " << currentProcess.id << endl;
            // cout << "Current Process AT: " << currentProcess.arrivalTime << endl;
            // cout << "Current Process BT: " << currentProcess.burstTime << endl;
            // cout << "Current Process Priority: " << currentProcess.priority << endl;
            if (currentProcess.priority != 1)   // There's a chance that a process is being ran but is not of priority 1
            {
                // We need to terminate the current process that is not a priority of level 1
                switch (currentPriority) {
                    case 2: // Current process is at Priority 2 Queue
                        cout << "Executing SRTF process " << currentProcess.id << " for " << SRTFTime << " units" << endl;
                        currentProcess.burstTime -= SRTFTime;
                        srtfQueue.pop();
                        SRTFTime = 0;

                        if (currentProcess.burstTime > 0)      // If there's still remaining BT, then push it back to queue
                        {
                            srtfQueue.push(currentProcess);
                        }
                        
                        break;
                    case 3: // Current process is at Priority 3 Queue
                        cout << "Executing SJF process " << currentProcess.id << " for " << SJFTime << " units" << endl;
                        currentProcess.burstTime -= SJFTime;
                        sjfQueue.erase(sjfQueue.begin());       // Remove the process from SJF Queue first due to potential changes
                        SJFTime = 0;

                        if (currentProcess.burstTime > 0)      // If there's still remaining BT, then push it back to queue
                        {
                            sjfQueue.push_back(currentProcess);
                            sort(sjfQueue.begin(), sjfQueue.end(), compareByBurstTime);
                        }
                        
                        break;
                    default: // Lowest priority (FIFO)
                        cout << "Executing FIFO process " << currentProcess.id << " for " << FIFOTime << " units" << endl;
                        currentProcess.burstTime -= FIFOTime;
                        fifoQueue.erase(fifoQueue.begin());       // Remove the process from FIFO Queue first due to potential changes
                        FIFOTime = 0;

                        if (currentProcess.burstTime > 0)      // If there's still remaining BT, then push it back to queue
                        {
                            fifoQueue.push_back(currentProcess);
                            sort(fifoQueue.begin(), fifoQueue.end(), compareByArrivalTime);
                        }
                }
            }
            
            currentProcess = rrQueue.front();       // Current Process is RR
            rrQueue.pop();                          // Pop from RR queue
            int rrTimeQuantum = 3; // Set your time quantum for RR
            int rrExecuteTime = min(rrTimeQuantum, currentProcess.burstTime);   // Either it will finish early or within TQ     
            cout << "Executing RR process " << currentProcess.id << " for " << rrExecuteTime << " units" << endl;
            currentProcess.burstTime -= rrExecuteTime;
            if (currentProcess.burstTime > 0) {
                rrQueue.push(currentProcess);
            }

            currentTime += rrExecuteTime;

            if (currentProcess.burstTime <= 0) {
                cout << "Completing process " << currentProcess.id << " at " << currentTime << endl;
            }
        }
        // Next priority queue (Shortest Remaining Time First - SRTF)
        if (!srtfQueue.empty()) {                          
            if (rrQueue.empty())       // If RR queue empty
            {
                currentProcess = srtfQueue.top();      // Retrieve 
                
                 
                SRTFTime += 1;                              // Update amount of time its running
                if (currentProcess.burstTime - SRTFTime == 0)
                {
                    cout << "Executing SRTF process " << currentProcess.id << " for " << SRTFTime << " units" << endl;
                    currentProcess.burstTime = 0;
                    srtfQueue.pop();
                    currentTime += 1;
                    cout << "Completing process " << currentProcess.id << " at " << currentTime << endl;
                    SRTFTime = 0;
                    
                } 
                else
                {
                    currentTime += 1;
                } 
                

                
            }    
        }

        // Next priority queue (Shortest Job First - SJF)
        if (!sjfQueue.empty()) {
            if (rrQueue.empty() && srtfQueue.empty())
            {
                currentProcess = sjfQueue.front();
                SJFTime += 1;
                if (currentProcess.burstTime - SJFTime == 0)
                {
                    cout << "Executing SJF process " << currentProcess.id << " for " << SJFTime << " units" << endl;
                    currentProcess.burstTime = 0;
                    sjfQueue.erase(sjfQueue.begin());
                    currentTime += 1;
                    cout << "Completing process " << currentProcess.id << " at " << currentTime << endl;
                    SJFTime = 0;
                    
                }  
                else
                {
                    currentTime += 1;
                }
                
                
            }            
        }

        // Lowest priority queue (First-In-First-Out - FIFO)
        if (!fifoQueue.empty()) {
            if (rrQueue.empty() && srtfQueue.empty() && sjfQueue.empty())
            {
                currentProcess = fifoQueue.front();
                FIFOTime += 1;
                if (currentProcess.burstTime - FIFOTime == 0)
                {
                    cout << "Executing FIFO process " << currentProcess.id << " for " << FIFOTime << " units" << endl;
                    currentProcess.burstTime = 0;
                    fifoQueue.erase(fifoQueue.begin());
                    currentTime += 1;
                    cout << "Completing process " << currentProcess.id << " at " << currentTime << endl;
                    FIFOTime = 0;
                   
                }  
                else
                {
                    currentTime += 1;
                }

            }
        }
    }
}

// int main() {
//     vector<FPProcess> processes = {
//         {1, 1, 10, 1},
//         {2, 2, 5, 2},
//         {3, 5, 8, 3},
//         {4, 0, 2, 4},
//         {5, 4, 8, 3},
//         {6, 3, 2, 2},
//         {7, 2, 15, 4},
//         {8, 9, 4, 1}
//     };

//     mlqScheduler(processes);

//     return 0;
// }
