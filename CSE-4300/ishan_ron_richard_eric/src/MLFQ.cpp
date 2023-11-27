#include <iostream>
#include "MLFQ.h"

using namespace std;

MLFQ::MLFQ(int timeslice, int numQs, int age_num, const vector<Process *> &processes) 
: finished(false), age_count(0), age_num(age_num), cur_time_count(0), time_count(0), timeslice(timeslice), jobs(processes)
{
  io_container = vector<Process *>();
  queues = vector<queue<Process *> >(numQs);

  //set up queues vector
  for(int i = 0; i < numQs; i++){ 
    queues[i] = queue<Process *>();
  }
}

void MLFQ::run_simulation()
{
  Process *cur = NULL;
  while(!finished){
    add_jobs();
    cur = simulate(cur);
  }
  cout << "simulation finished\n";
}

//for adding jobs initially
void MLFQ::add_jobs(){
  for(int i = 0; i < jobs.size(); i++){
    if(jobs[i]->getArrivalTime() == time_count){
      cout << "Process " << jobs[i]->getName() << " arriving at "<< time_count << " ms.\n";
      
      string jobtype = jobs[i]->getCurJobType();
      if(jobtype == "IO")
        io_container.push_back(jobs[i]);
      else
        queues[0].push(jobs[i]);
      jobs.erase(jobs.begin() + i);
      i--;
    }
  }
}

//returns current job with highest priority, if none, returns NULL
Process *MLFQ::get_curJob()
{
  for(int i = 0; i < queues.size(); i++)
    if(!queues[i].empty())
      return queues[i].front();
  return NULL;
}

Process *MLFQ::simulate(Process *current)
{
  Process *cur = (current == NULL) ? get_curJob(): current;   
  cur_time_count = (current == NULL) ? 0: cur_time_count;

  if(cur == NULL && io_container.size() == 0 && jobs.size() == 0){ // checks if finished or not
    finished = true;
    return cur;
  }

  if(cur != NULL){ 
    string jobtype = cur->getCurJobType();
    if(jobtype == "CPU"){
      cur->decrementJobTime(1, time_count+1);
    }
    else if(jobtype == "IO"){
      io_container.push_back(cur);
      queues[cur->getPriority()].pop();
      age_count++;

      return NULL;
    }
  }

  cur_time_count++;
  time_count++;

  decrement_IO();

  if(cur != NULL && cur->isFinished()){
    age_count++;
    int priority = cur->getPriority();
    if(!queues[priority].empty())
      queues[priority].pop();
    cur = NULL;
  }
  else if(cur != NULL && cur_time_count == timeslice){ 
    age_count++;
    int priority = cur->getPriority();
    queues[priority].pop();
    if(!cur->isFinished()){
      reduce_priority(cur);
      queues[cur->getPriority()].push(cur);
    }
    cur = NULL;
  }

  if(age_count == age_num){
    age_count = 0;
    age_priority();
  }

  return cur;
}

//decrements job time of all jobs in the io_container + removes and re-adds jobs if IO is complete
void MLFQ::decrement_IO(){
  string jobtype;
  for(int i = 0; i < io_container.size(); i++){
    io_container[i]->decrementJobTime(1, time_count); 
    if(!io_container[i]->isFinished()){
      jobtype = io_container[i]->getCurJobType();
      if(jobtype == "CPU"){
        int priority = io_container[i]->getPriority();
        queues[priority].push(io_container[i]);
        io_container.erase(io_container.begin() + i);
        i--;
      }
    }
    else{
      io_container.erase(io_container.begin() + i);
      i--;
    }
  }
}


//assumes the job you want to reduce priority of is already removed from queues
void MLFQ::reduce_priority(Process *job){
  int priority = job->getPriority();
  if (priority < queues.size() - 1){
    job->setPriority(priority + 1);
  }
}


//starts at second queue, and moves all of the low priority jobs up
void MLFQ::age_priority()
{
  Process *buff;
  for(int i = 1; i < queues.size(); i++){
    while(!queues[i].empty()){
      buff = queues[i].front();
      buff->setPriority(0);
      queues[0].push(buff);
      queues[i].pop();
    }
  }
}
