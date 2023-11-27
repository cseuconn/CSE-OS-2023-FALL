#ifndef program_h
#define program_h

#include <string>

class program_to_run {
public:
    // Class declaration
    program_to_run(){};
    
    // Getter functions
    int getID()  {return ID;}
    int getRuntimeTotal() {return runtime_total;}
    int getRuntimeLeft() { return runtime_left; }
    int getPriority() {return priority;}
    double getPriorityRatio() {return priority_ratio;}

    // Check if the program finished running this cycle
    bool isDone() const {return (runtime_left <= 0);}

    // Setter functions
    void setID(int id_in) {ID = id_in;}
    void setRuntimes(int runtime_in) {runtime_total = runtime_in; runtime_left = runtime_in;}
    void setPriority(int priority_in) {priority = priority_in;}
    void setPriorityRatio(double ratio_in) {priority_ratio = ratio_in;}

    // Run a cycle
    void cycleRun() {runtime_left--;};

private:
    int ID = 1;
    int runtime_total = 1;
    int runtime_left = 1;
    int priority = 1;
    double priority_ratio = 0;
};

#endif