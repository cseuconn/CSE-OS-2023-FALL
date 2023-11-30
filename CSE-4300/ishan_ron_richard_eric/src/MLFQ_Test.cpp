#include "MLFQ_Process.h"
#include "MLFQ.h"
#include "np_first_in_first_out.cpp"
#include "np_shortest_remaining_time_first.cpp"
#include <cstdlib>
#include <algorithm>
#include <string>
#include <ctime>

using Job = std::pair<int, std::string>;  //pair of time (in ms) and job type (CPU/IO)

int limitjobtime=10;  //set the ceil limit of random number of jobs per process
int limitProcesses=3;  //set number of processes for the test
int timeslice=5;       //limit of quantom

void MakeProcesses(std::vector<Process *> &proclist){      //make random jobs based on set parameters, given a container for processes.
    std::vector<Job> jobs;
    std::string jobname;
    int jobmax;
    for(int i=0;i<limitProcesses;i++){        //setting up the processes
        jobs.clear();
        jobmax=(rand() % 5) + 1;          //getting max for amount of job per process
        for(int j=1;j<=jobmax;j++){      //making a list of jobs
          jobname = "CPU";
        // if(rand() %2 == 0){
        //     jobname="CPU";
        // }
        // else{
        //     jobname="IO";
        // }
            Job job=Job((rand() % limitjobtime)+1,jobname);
            //std::cout<<"Creating and pushing {"<<to_string(job.first)+" "+job.second+"}"<<std::endl;
            jobs.push_back(job);   //push new job into the list, using random int
        }
        std::cout<<"Creating and pushing "<<"Process "<<i<<std::endl;
        proclist.push_back(new Process(to_string(i),rand()%20,jobs));    //using new list of jobs to create a new process and
                                                                                    //add to the referenced vector
    }
    //jobs.clear();    
}

int main(){
   // srand(static_cast<unsigned int>(time(nullptr))); //set random seed based on time
    srand(4);
    std::vector<Process *> processes;
    std::vector<Job> jobs;
    MakeProcesses(processes);
    std::cout<<"Made process list: "<<std::endl;
    
    for(auto i : processes){
        std::cout<<i->getName()+" arrives at "+to_string(i->getArrivalTime())<<" with Jobs: "<<std::endl;
        for(auto j : i->getJobs()){
            std::cout<<"{"+to_string(j.first)+", "+j.second+"}, ";
        }
        std::cout<<std::endl;
    } 

    vector<FProcess> processess2;
    processess2.push_back(FProcess(0, 8, 22));
    processess2.push_back(FProcess(1, 15, 18));
    processess2.push_back(FProcess(2, 18, 33));

    vector<FProcess> processess3;
    processess3.push_back(FProcess(0, 8, 22));
    processess3.push_back(FProcess(1, 15, 18));
    processess3.push_back(FProcess(2, 18, 33));

    std::cout<<"now making MLFQ"<<std::endl;
    MLFQ *MLFQScheduler=new MLFQ(timeslice, 3, 3, processes);
    std::cout<<"Running MLFQ"<<std::endl;
    MLFQScheduler->run_simulation();

    std::cout << "now running FIFO" << std::endl;
    fifoScheduler(processess2);

    std::cout << "now making SRTF" << std::endl;
    srtfScheduler(processess3);

    for (auto &process : processes) {
        delete process;  // Make sure this is the only delete for each process object
    }
    processes.clear();  // Clear the vector after deleting all objects
    return 0;
}


/*
To test for MLFQ:
    -correctness
    -IO
    -aging 
*/
