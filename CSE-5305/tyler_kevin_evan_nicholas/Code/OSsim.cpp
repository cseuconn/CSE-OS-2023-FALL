#include "core.h"
#include "program.h"
//#include <stdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include<unistd.h>
// These are used in multiple functions, are are defined globally as such
// For simplicity and output readability, all programs have a maximum length of 30 cycles, and a priority level of 1-6
// These numbers could be changed arbitrarily to be more or less realistic, but are being used to have nice output.
double runtime_const = 30;
double prio_const = 6;
// Every 3 cycles a new program is added; this will keep the queues generally full
// This number was chosen since avg runtime is 15, and avg runtimes/cycle is 4. floor(15/4) = 3.
int add_every_cycles = 3;

// This determines which queue should be run for the CPU cycle. For every 3 that top priority runs, middle runs 2, and bottom runs 1
int queue_cycle_switch = 0;

// Returns core with least total runtime remaining. This core will be used by the scheduler this cycle
int getLeastRuntimeCore(std::vector<queue> &core1, std::vector<queue> &core2 ,std::vector<queue> &core3 ,std::vector<queue> &core4){
    std::vector<int> mins = {};
    mins.push_back(core1[0].getTimeLeft() + core1[1].getTimeLeft() + core1[2].getTimeLeft());
    mins.push_back(core2[0].getTimeLeft() + core2[1].getTimeLeft() + core2[2].getTimeLeft());
    mins.push_back(core3[0].getTimeLeft() + core3[1].getTimeLeft() + core3[2].getTimeLeft());
    mins.push_back(core4[0].getTimeLeft() + core4[1].getTimeLeft() + core4[2].getTimeLeft());
    int min_val = *min_element(mins.begin(), mins.end());
    std::vector<int>::iterator min_iterator = std::find(mins.begin(), mins.end(), min_val);
    int min_idx = std::distance(mins.begin(), min_iterator);
    std::cout << "Core with least total runtime remaining: " << min_idx+1 << " with " << min_val << " cycles remaining" <<std::endl;
    return min_idx;
}

// Function to get all runtimes of the correct priority queue
std::vector<int> getRuntimes(std::vector<queue> &core1, std::vector<queue> &core2 ,std::vector<queue> &core3 ,std::vector<queue> &core4, int priority){
    std::vector<int> runtime_remainings_temp = {};
    runtime_remainings_temp.push_back(core1[priority].getTimeLeft());
    runtime_remainings_temp.push_back(core2[priority].getTimeLeft());
    runtime_remainings_temp.push_back(core3[priority].getTimeLeft());
    runtime_remainings_temp.push_back(core4[priority].getTimeLeft());
    return runtime_remainings_temp;
}

// These functions get all runtime totals of the queues, then find the minimum value, then its index
int getMinIndex(std::vector<int> runtime_remainings){
    int min_val = *min_element(runtime_remainings.begin(), runtime_remainings.end());
    std::vector<int>::iterator min_iterator = std::find(runtime_remainings.begin(), runtime_remainings.end(), min_val);
    int min_idx = std::distance(runtime_remainings.begin(), min_iterator);
    return min_idx;   
}

void scheduleOneProgram(program_to_run &prog, std::vector<queue> &core1, std::vector<queue> &core2 ,std::vector<queue> &core3 ,std::vector<queue> &core4, int priority){
    std::vector<int> runtime_remainings = getRuntimes(core1, core2, core3, core4, priority);
    std::cout << "Adding program " << prog.getID() << " to queue " << priority + 1 << " in core ";
    
    // Determines which queue to add to
    int min_idx = getMinIndex(runtime_remainings);
    if(min_idx == 0){
        std::cout << "1" << std::endl;
        core1[priority].addProgram(prog);
    }else if(min_idx == 1){
        std::cout << "2" << std::endl;
        core2[priority].addProgram(prog);
    }else if(min_idx == 2){
        std::cout << "3" << std::endl;
        core3[priority].addProgram(prog);
    }else if(min_idx == 3){
        std::cout << "4" << std::endl;
        core4[priority].addProgram(prog);
    }
}

void scheduleUserInputProgram(program_to_run &prog, std::vector<queue> &core1, std::vector<queue> &core2 ,std::vector<queue> &core3 ,std::vector<queue> &core4){
    std::vector<int> runtime_remainings = getRuntimes(core1, core2, core3, core4, 0);
    int min_idx = getMinIndex(runtime_remainings);
    if(min_idx == 0){
        core1[0].addUserInput(prog);
    }else if(min_idx == 1){
        core2[0].addUserInput(prog);
    }else if(min_idx == 2){
        core3[0].addUserInput(prog);
    }else if(min_idx == 3){
        core4[0].addUserInput(prog);
    }
}

void scheduleProgs(std::vector<program_to_run> &progs_to_schedule, std::vector<queue> &core1, std::vector<queue> &core2 ,std::vector<queue> &core3 ,std::vector<queue> &core4){
    for(int i = 0; i < progs_to_schedule.size(); i++){
        if(progs_to_schedule[i].getPriorityRatio() == 0){// designated user input program, top priority no matter what
            scheduleUserInputProgram(progs_to_schedule[i], core1, core2, core3, core4);
        }else if(progs_to_schedule[i].getPriorityRatio() > 1.4){ //top set of priority ratios will be above 1.4
            scheduleOneProgram(progs_to_schedule[i], core1, core2, core3, core4, 0);
        }else if(progs_to_schedule[i].getPriorityRatio() < 0.75){ //bottom set of priority ratios will be below 0.75
            scheduleOneProgram(progs_to_schedule[i], core1, core2, core3, core4, 2);
        }else{ //middle set of priority ratios
            scheduleOneProgram(progs_to_schedule[i], core1, core2, core3, core4, 1);       
        }
    }
}

void addRandomProgram(queue &to_add_to, int &pid){
    pid++;
    program_to_run temp_pr;
    int run = rand() % 30;
    int prio = rand() % 6;
    temp_pr.setID(pid);
    temp_pr.setRuntimes(run);
    temp_pr.setPriority(prio);
    to_add_to.addProgram(temp_pr);
}

program_to_run getRandomProgram(int &pid){
    pid++;
    program_to_run temp_pr;
    double ratio_const = runtime_const/prio_const;
    int run = rand() % (int)runtime_const + 1;
    int prio = rand() % (int)prio_const + 1;
    // This determines the priority ratio of the progam. It is priority/(runtime/(max_prio/max_runtime))
    double ratio = (double)prio/((double)run/ratio_const);
    temp_pr.setID(pid);
    temp_pr.setRuntimes(run);
    temp_pr.setPriority(prio);
    temp_pr.setPriorityRatio(ratio);
    std::cout << "Ratio for program " << pid << ": " << ratio << std::endl;
    return temp_pr;
}

program_to_run getUserInputProgram(int &pid){
    pid++;
    program_to_run user_pr;
    // User input should not take long to run
    int run = rand() % 3 + 1;
    // User level programs are highest priority
    int prio = 7;
    double ratio = 0;
    user_pr.setID(pid);
    user_pr.setRuntimes(run);
    user_pr.setPriority(prio);
    user_pr.setPriorityRatio(ratio);
    std::cout << "User progam #" << user_pr.getID() << " created. Maximum priority level" << std::endl;
    return user_pr;
}

// Checks if a program should be created; should have a 1/cycles chance to return true
bool checkForCreate(){
    return (add_every_cycles-1) == (rand() % add_every_cycles);
}

// 1 in 5 programs will be user input; should have a 1/5 chance to return true
bool checkForUserInput(){
    return (5 == (rand() % 5+1));
}

void fillCore(std::vector<queue> &core){
    // Put queues in the core. There is a queue for low, medium and high priority programs
    queue queueLow;
    queue queueMiddle;
    queue queueHigh;
    core.push_back(queueHigh);
    core.push_back(queueMiddle);
    core.push_back(queueLow);
}

int main(){
    // The program_id will forever increment with programs being added
    // The cycle count will be used to force lower priority queues to be run
    int program_id = 0;
    int cycle_count = 0;
    
    // Initialize 4 vectors to be cores, and fill them with their queues.
    std::vector<queue> Core1;
    std::vector<queue> Core2;
    std::vector<queue> Core3;
    std::vector<queue> Core4;
    fillCore(Core1);
    fillCore(Core2);
    fillCore(Core3);
    fillCore(Core4);

    // Generate and schedule an initial set of programs
    std::vector<program_to_run> waiting_for_scheduling = {};
    for(int j = 0; j < 6; j++){
        for(int i = 0; i < 4; i++){
            waiting_for_scheduling.push_back(getRandomProgram(program_id));
        }
        scheduleProgs(waiting_for_scheduling, Core1, Core2, Core3, Core4);
        waiting_for_scheduling = {};
    }

    // NOTE: THIS PROGRAM WILL RUN FOREVER UNLESS TERMINATED BY THE USER
    // Please take this into consideration when running

    while(true){
        queue_cycle_switch++;
        // Checks if a new program should be added to be scheduled
        if(checkForCreate()){
            std::cout << "Made new program #" << program_id+1 << " to schedule" << std::endl;
            if(checkForUserInput()){
                waiting_for_scheduling.push_back(getUserInputProgram(program_id));    
            }else{
                waiting_for_scheduling.push_back(getRandomProgram(program_id));
            }
        }
        // Increment a check if programs should be added
        cycle_count++;
        if (cycle_count >= add_every_cycles && waiting_for_scheduling.size() != 0){
            cycle_count = 0;
            int low = getLeastRuntimeCore(Core1, Core2, Core3, Core4);
            scheduleProgs(waiting_for_scheduling, Core1, Core2, Core3, Core4);
            waiting_for_scheduling = {};
            if(queue_cycle_switch < 4){
                std::cout << "Queue 1 runnning" << std::endl;
                if(low == 0){ // If core 1 is scheduling
                    std::cout << "Core 1 scheduling" << std::endl;
                    Core2[0].runCycleQueue(1, 2); Core3[0].runCycleQueue(1, 3); Core4[0].runCycleQueue(1, 4);
                }else if(low == 1){ // If core 2 is scheduling
                    std::cout << "Core 2 scheduling" << std::endl;
                    Core1[0].runCycleQueue(1, 1); Core3[0].runCycleQueue(1, 3); Core4[0].runCycleQueue(1, 4);
                }else if(low == 2){ // If core 3 is scheduling
                    std::cout << "Core 3 scheduling" << std::endl;
                    Core1[0].runCycleQueue(1, 1); Core2[0].runCycleQueue(1, 2); Core4[0].runCycleQueue(1, 4);
                }else{ // if core 4 is scheduling
                    std::cout << "Core 4 scheduling" << std::endl;
                    Core1[0].runCycleQueue(1, 1); Core2[0].runCycleQueue(1, 2); Core3[0].runCycleQueue(1, 3);
                }
            }else if(queue_cycle_switch < 6){ // If Queue 2 should run
                std::cout << "Queue 2 running" << std::endl;
                if(low == 0){ // If core 1 is scheduling
                    std::cout << "Core 1 scheduling" << std::endl;
                    Core2[1].runCycleQueue(2, 2); Core3[1].runCycleQueue(2, 3); Core4[1].runCycleQueue(2, 4);
                }else if(low == 1){ // If core 2 is scheduling
                    std::cout << "Core 2 scheduling" << std::endl;
                    Core1[1].runCycleQueue(2, 1); Core3[1].runCycleQueue(2, 3); Core4[1].runCycleQueue(2, 4);
                }else if(low == 2){ // If core 3 is scheduling
                    std::cout << "Core 3 scheduling" << std::endl;
                    Core1[1].runCycleQueue(2, 1); Core2[1].runCycleQueue(2, 2); Core4[1].runCycleQueue(2, 4);
                }else{ // if core 4 is scheduling
                    std::cout << "Core 4 scheduling" << std::endl;
                    Core1[1].runCycleQueue(2, 1); Core2[1].runCycleQueue(2, 2); Core3[1].runCycleQueue(2, 3);
                }
            }else{ // If queue 3 should run
                std::cout << "Queue 3 running" << std::endl;
                if(low == 0){ // If core 1 is scheduling
                    std::cout << "Core 1 scheduling" << std::endl;
                    Core2[2].runCycleQueue(3, 2); Core3[2].runCycleQueue(3, 3); Core4[2].runCycleQueue(3, 4);
                }else if(low == 1){ // If core 2 is scheduling
                    std::cout << "Core 2 scheduling" << std::endl;
                    Core1[2].runCycleQueue(3, 1); Core3[2].runCycleQueue(3, 3); Core4[2].runCycleQueue(3, 4);
                }else if(low == 2){ // If core 3 is scheduling
                    std::cout << "Core 3 scheduling" << std::endl;
                    Core1[2].runCycleQueue(3, 1); Core2[2].runCycleQueue(3, 2); Core4[2].runCycleQueue(3, 4);
                }else{ // if core 4 is scheduling
                    std::cout << "Core 4 scheduling" << std::endl;
                    Core1[2].runCycleQueue(3, 1); Core2[2].runCycleQueue(3, 2); Core3[2].runCycleQueue(3, 3);
                }
             }

        }else{
            if(queue_cycle_switch < 4){
                std::cout << "Queue 1 running" << std::endl;
                Core1[0].runCycleQueue(1, 1); Core2[0].runCycleQueue(1, 2); Core3[0].runCycleQueue(1, 3); Core4[0].runCycleQueue(1, 4);
            }else if(queue_cycle_switch < 6){ 
                std::cout << "Queue 2 running" << std::endl;
                Core1[1].runCycleQueue(2, 1); Core2[1].runCycleQueue(2, 2); Core3[1].runCycleQueue(2, 3); Core4[1].runCycleQueue(2, 4);
            }else{
                std::cout << "Queue 3 running" << std::endl;
                Core1[2].runCycleQueue(3, 1); Core2[2].runCycleQueue(3, 2); Core3[2].runCycleQueue(3, 3); Core4[2].runCycleQueue(3, 4);
            }
        }
        //reset queue cycle
        if(queue_cycle_switch == 7){
            queue_cycle_switch = 0;
        }
        sleep(2);
    }
    return 0;
}