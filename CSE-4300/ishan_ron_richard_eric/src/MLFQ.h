#ifndef __MLFQ_H_
#define __MLFQ_H_

using namespace std;
#include <vector>
#include <queue>
#include "MLFQ_Process.h"

//uses Process Class, runs simulations

class MLFQ
{
  public:
    MLFQ(int timeslice, int numQs, int age_num, const vector<Process *> &processes);
    ~MLFQ() {}
                     
    void run_simulation();

  private:
    void add_jobs(); 

    Process *get_curJob();

    Process *simulate(Process *current);

    void decrement_IO();

    void reduce_priority(Process *job);

    void age_priority();

    bool finished; 
    int age_count; // counts how many cycles have happened (for aging purposes)
    int age_num; // how many cycles to age after
    int cur_time_count; // counts time a single process has been running for (ms)
    int time_count; // overall time elapsed (ms)
    int timeslice; // for how long to run each slice
    vector<Process *> jobs; //holds the jobs until they enter one of the queues
    vector<Process *> io_container; // holds processes doing IO
    vector<queue<Process *> > queues; // holds the pointers to the queues
};

#endif
