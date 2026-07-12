#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

struct Process {
    int id;
    int arrival_time;
    int burst_time;
    int completion_time = 0;
    int turnaround_time = 0;
    int sizeof_waiting_time = 0; // standard waiting_time
    int waiting_time = 0;
};

// Comparator to sort processes by arrival time. 
// If arrival times are equal, break the tie using the Process ID.
bool compareArrival(const Process& a, const Process& b) {
    if (a.arrival_time == b.arrival_time) {
        return a.id < b.id;
    }
    return a.arrival_time < b.arrival_time;
}

// Comparator to restore the original ID order for final printing
bool compareID(const Process& a, const Process& b) {
    return a.id < b.id;
}

void calculateFCFS(std::vector<Process>& processes) {
    // 1. Sort processes based on their arrival times
    std::sort(processes.begin(), processes.end(), compareArrival);

    int current_time = 0;
    double total_waiting_time = 0;
    double total_turnaround_time = 0;

    // 2. Run the FCFS scheduling simulation loop
    for (size_t i = 0; i < processes.size(); ++i) {
        // If the CPU is idle waiting for the next process to arrive
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }

        // Process executes
        processes[i].completion_time = current_time + processes[i].burst_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;

        // Move the timeline forward
        current_time = processes[i].completion_time;

        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    // 3. Sort back by ID just to display the output cleanly
    std::sort(processes.begin(), processes.end(), compareID);

    // 4. Print results in a structured table
    std::cout << "\n" << std::setw(5) << "PID" 
              << std::setw(15) << "Arrival Time" 
              << std::setw(12) << "Burst Time" 
              << std::setw(18) << "Completion Time" 
              << std::setw(18) << "Turnaround Time" 
              << std::setw(15) << "Waiting Time" << "\n";
    
    std::cout << std::string(83, '-') << "\n";

    for (const auto& p : processes) {
        std::cout << std::setw(5) << p.id 
                  << std::setw(15) << p.arrival_time 
                  << std::setw(12) << p.burst_time 
                  << std::setw(18) << p.completion_time 
                  << std::setw(18) << p.turnaround_time 
                  << std::setw(15) << p.waiting_time << "\n";
    }

    std::cout << std::string(83, '-') << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Waiting Time: " << (total_waiting_time / processes.size()) << "\n";
    std::cout << "Average Turnaround Time: " << (total_turnaround_time / processes.size()) << "\n";
}

int main() {
    // Sample dataset: Processes arriving out of sequence to test sorting logic
    std::vector<Process> processes = {
        {1, 2, 6},  // PID 1: Arrives at 2, needs 6 units
        {2, 5, 2},  // PID 2: Arrives at 5, needs 2 units
        {3, 0, 8},  // PID 3: Arrives at 0, needs 8 units
        {4, 0, 3},  // PID 4: Arrives at 0, needs 3 units (tests tie-breaker with PID 3)
        {5, 20, 4}  // PID 5: Arrives at 20, needs 4 units (tests CPU idle time handling)
    };

    calculateFCFS(processes);

    return 0;
}