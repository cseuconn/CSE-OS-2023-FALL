#include "core.h"
#include "program.h"
#include <iostream>

void queue::addProgram(program_to_run prog){
    programs.push_back(prog);
    programs[0].setRuntimes(7);
    addTime(getLastTime());
}

void queue::addUserInput(program_to_run prog){
    programs.insert(programs.begin(), prog);
    addTime(prog.getRuntimeTotal());
}

void queue::runCycleQueue(int que, int core){
    if(programs.size() > 0){
        runtime_remaining--; 
        if(programs[0].isDone()){;
            std::cout << "Program " << programs[0].getID() << " completed on core " <<core << " in queue " << que << std::endl;
            programs.erase(programs.begin());
        }
        programs[0].cycleRun();
    }
}
