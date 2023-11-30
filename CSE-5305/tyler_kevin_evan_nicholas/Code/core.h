
#ifndef core_h
#define core_h
#include <ctime>
#include <string>
#include <vector>
#include "program.h"

class queue {
public:
    // Class declaration
    queue(){};
    
    // Getter functions
    int getTimeLeft() { return this->runtime_remaining; }
    int getPriority() {return this->priority;}
    std::vector<program_to_run> getPrograms(){return programs;}

    // Setter functions
    void setPriority(int priority_in) {priority = priority_in;}

    //function to add program to queue, and increment runtime, by adding the time of the final element in queue
    void addProgram(program_to_run);
    void addUserInput(program_to_run);
    void addTime(int time_to_add){runtime_remaining += time_to_add;}
    int getLastTime() {return programs.back().getRuntimeTotal();}
    

    // Run a cycle decreasing the queue runtime, and speci
    void runCycleQueue(int que, int core) ;

    //getter function for priority
    int getPriority() const {return priority;}

private:
    std::vector<program_to_run> programs = {};
    int runtime_remaining = 0;
    int priority;

};

#endif