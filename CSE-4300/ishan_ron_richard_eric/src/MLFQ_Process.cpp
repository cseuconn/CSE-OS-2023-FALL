#include "MLFQ_Process.h"

std::string Process::getName() const {
    return name;
}

int Process::getArrivalTime() const {
    return arrival_time;
}

int Process::getPriority() const { 
    return priority;
}

const std::vector<Job>& Process::getJobs() const { 
    return jobs;
}

void Process::addJob(const Job& job) { 
    jobs.push_back(job);
}

bool Process::isFinished() const { 
    return jobs.empty();
}

void Process::setPriority(int priority) { 
    this->priority = priority;
}

std::string Process::getCurJobType() const {
    if(this->isFinished())
        return "finished";
    Job cur = jobs.front();
    return cur.second; //second attribute of Job vector 
}

int Process::getCurJobTime() const {
    Job cur = jobs.front();
    return cur.first;
}

void Process::decrementJobTime(int time, int curtime) {
    if(!this->isFinished()){
        Job &cur = jobs.front();
        if(cur.first >= time)
            cur.first -= time;

        if(cur.first <= 0) //removes job if it is finished
            removeCurJob(curtime);
    }
    else{
        std::cout << "Process " << this->name << " finished at " << curtime << " ms.\n";
    }
}

void Process::removeCurJob(int curtime) {
    if(!this->isFinished())
        jobs.erase(jobs.begin());

    if(curtime != -1 && this->isFinished())
        std::cout << "Process " << this->name << " finished at " << curtime << " ms.\n";
}

void Process::printDetails() const { 
    std::cout << "\nProcess Name: " << name << std::endl;
    std::cout << "Process Priority: " << priority << std::endl;
    std::cout << "Jobs: " << std::endl;
    for (const auto& job : jobs) {
        std::cout << "  Time: " << job.first << "ms, Type: " << job.second << std::endl;
    }
}

/* int main() {
// Example usage
Process p1("Process 1", 5, {{20, "CPU"}, {10, "IO"}}); //process p1, queue of process 
Process p2("Process 2", 3, {{10, "CPU"}}); //process p2
p1.printDetails(); //prints process p1 details
p2.printDetails();
return 0;
} */