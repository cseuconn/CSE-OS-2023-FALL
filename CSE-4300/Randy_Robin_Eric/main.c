#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

struct process {
    int pid;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    int remaining_time;
    int is_completed;
};

double calc_avg_waiting_time (struct process *processes, int n) {
    double total_waiting_time = 0;
    for (int i = 0; i < n; i++) {
        total_waiting_time += processes[i].waiting_time;
    }
    return total_waiting_time / n;

}

double calc_avg_turnaround_time (struct process *processes, int n) {
    double total_turnaround_time = 0;
    for (int i = 0; i < n; i++) {
        total_turnaround_time += processes[i].turnaround_time;
    }
    return total_turnaround_time / n;
}

void display_process_table(struct process *processes, int n, int type) {
    if (type == 0){
    printf("PID\tArrival Time\tBurst Time\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time);
    }}
    else if (type == 1){
    printf("PID\tArrival Time\tBurst Time\tCompletion Time\tTurnaround Time\tWaiting Time\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].completion_time, processes[i].turnaround_time, processes[i].waiting_time);
    }}
}

void fcfsScheduling(struct process *processes, int n){
    // Sort processes by arrival time
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - (i + 1); j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                struct process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    int current_time = 0;

    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        current_time += processes[i].burst_time;
        processes[i].completion_time = current_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
    }

    printf("After scheduling:\n");
    display_process_table(processes, n, 1);

    double avg_waiting_time = calc_avg_waiting_time(processes, n);
    double avg_turnaround_time = calc_avg_turnaround_time(processes, n);

    printf("average waiting time: %f\n", avg_waiting_time);
    printf("average turnaround time: %f\n", avg_turnaround_time);

}

void rrScheduling(struct process *processes, int n, int quantum){
    //Sort the processes by arrival time
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - (i + 1); j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                struct process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    int current_time = processes[0].arrival_time;
    int remaining_processes = n;

    while (remaining_processes > 0){
        for (int i = 0; i < n; i++){
            if (processes[i].is_completed == 0){
                if (processes[i].remaining_time <= quantum){
                    current_time += processes[i].remaining_time;
                    processes[i].remaining_time = 0;
                    processes[i].is_completed = 1;
                    remaining_processes--;
                    processes[i].completion_time = current_time;
                    processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                }
                else{
                    current_time += quantum;
                    processes[i].remaining_time -= quantum;
                }
            }
        }
    }

    printf("After scheduling:\n");
    display_process_table(processes, n, 1);

    double avg_waiting_time = calc_avg_waiting_time(processes, n);
    double avg_turnaround_time = calc_avg_turnaround_time(processes, n);

    printf("average waiting time: %f\n", avg_waiting_time);
    printf("average turnaround time: %f\n", avg_turnaround_time);

}

int find_shortest_index(struct process *processes, int n, int current_time){
    int shortest_index = -1;
    int shortest_time = INT_MAX;
    
    for (int i = 0; i < n; i++){
        if (processes[i].is_completed == 0 && processes[i].arrival_time <= current_time){
            if (processes[i].remaining_time < shortest_time){
                shortest_time = processes[i].remaining_time;
                shortest_index = i;
            }
        }
    }
    return shortest_index;
}

void srtfScheduling(struct process *processes, int n){
    //  Sort the processes by arrival time
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - (i + 1); j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                struct process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    int current_time = processes[0].arrival_time;
    int remaining_processes = n;

    while (remaining_processes > 0){
        int shortest_index = find_shortest_index(processes, n, current_time);
        
        if (shortest_index == -1){
            current_time++;
        }

        else{
            processes[shortest_index].remaining_time--;
            current_time++;
            if (processes[shortest_index].remaining_time == 0){
                processes[shortest_index].is_completed = 1;
                remaining_processes--;
                processes[shortest_index].completion_time = current_time;
                processes[shortest_index].turnaround_time = processes[shortest_index].completion_time - processes[shortest_index].arrival_time;
                processes[shortest_index].waiting_time = processes[shortest_index].turnaround_time - processes[shortest_index].burst_time;
            }
        }
    }

    printf("After scheduling:\n");
    display_process_table(processes, n, 1);

    double avg_waiting_time = calc_avg_waiting_time(processes, n);
    double avg_turnaround_time = calc_avg_turnaround_time(processes, n);

    printf("average waiting time: %f\n", avg_waiting_time);
    printf("average turnaround time: %f\n", avg_turnaround_time);
}

int main(int argc, char *argv[]) {
    int n; // num of processes
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    struct process *processes = malloc(n * sizeof(struct process));

    for (int i = 0; i < n; i++) {
        printf("Enter the arrival time of process %d: ", i + 1);
        scanf("%d", &processes[i].arrival_time);
        printf("Enter the burst time of process %d: ", i + 1);
        scanf("%d", &processes[i].burst_time);
        processes[i].pid = i + 1;
        processes[i].is_completed = 0;
        processes[i].remaining_time = processes[i].burst_time;
    }

    display_process_table(processes, n, 0);
    int choice;
    while(1){
        printf("\nSelect scheduling algorithm:\n1. FCFS\n2. RR\n3. SRTF\n4. Quit\nChoice: ");
        scanf("%d", &choice);
        printf("\n");
    switch(choice) {
        case 1:
            fcfsScheduling(processes, n);
            break;
        case 2:
            int quantum;
            printf("Enter quantum time: ");
            scanf("%d", &quantum);
            rrScheduling(processes, n, quantum);
            break;
        case 3:
            srtfScheduling(processes, n);
            break;
        case 4:
            exit(0);
        default:
            printf("Invalid choice\n");
            break;
        }
    }
}