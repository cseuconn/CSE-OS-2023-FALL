#ifndef __MLFQ_PROCESS_H_
#define __MLFQ_PROCESS_H_

#include <iostream>
#include <queue>
#include <string>

using Job = std::pair<int, std::string>;  //pair of time (in ms) and job type (CPU/IO)

class Process {
private:
    std::string name;
    int priority;
    int arrival_time;
    std::vector<Job> jobs; //vector of type Job (int, string) 

public:
    Process(std::string name, int arrival, std::vector<Job> jobs) : name(name), priority(0), arrival_time(arrival), jobs(jobs) {} 
    
    std::string getName() const;

    int getArrivalTime() const; 

    int getPriority() const;  

    std::string getCurJobType() const;

    const std::vector<Job>& getJobs() const;  

    void addJob(const Job& job);  

    void printDetails() const; 

    int getCurJobTime() const;

    void decrementJobTime(int time, int curtime=-1);
                                
    bool isFinished() const; 

    void setPriority(int priority);  

    void removeCurJob(int curtime);
};


#endif

