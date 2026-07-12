#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>

struct Process {
    int id;
    int arrival_time;
    int burst_time;
    int completion_time = 0;
    int turnaround_time = 0;
    int waiting_time = 0;
};

// Comparator for the timeline sorting (Chronological order)
bool compareArrival(const Process& a, const Process& b) {
    if (a.arrival_time == b.arrival_time) {
        return a.id < b.id;
    }
    return a.arrival_time < b.arrival_time;
}

// Custom Comparator for the Min-Heap Ready Queue
// In C++ priority_queue, returning 'true' means 'a' has LOWER priority than 'b' (elements float down).
struct CompareSJF {
    bool operator()(const Process& a, const Process& b) {
        if (a.burst_time == b.burst_time) {
            if (a.arrival_time == b.arrival_time) {
                return a.id > b.id; // Smaller ID gets higher priority
            }
            return a.arrival_time > b.arrival_time; // Earlier arrival gets higher priority
        }
        return a.burst_time > b.burst_time; // Smaller burst time gets higher priority
    }
};

void calculateSJF(std::vector<Process>& processes) {
    // 1. Sort the main pool chronologically by arrival time
    std::sort(processes.begin(), processes.end(), compareArrival);

    std::priority_queue<Process, std::vector<Process>, CompareSJF> ready_queue;
    std::vector<Process> completed_processes;
    
    int current_time = 0;
    size_t system_idx = 0; // Tracks which processes from the main pool have landed
    
    double total_waiting_time = 0;
    double total_turnaround_time = 0;

    // 2. Loop until all processes are processed
    while (system_idx < processes.size() || !ready_queue.empty()) {
        
        // Push all processes that have arrived by the current timestamp into the min-heap
        while (system_idx < processes.size() && processes[system_idx].arrival_time <= current_time) {
            ready_queue.push(processes[system_idx]);
            system_idx++;
        }

        // If no processes have arrived yet, the CPU is idle. 
        // Advance time directly to the next arriving job.
        if (ready_queue.empty()) {
            current_time = processes[system_idx].arrival_time;
            continue; 
        }

        // Pop the absolute shortest job currently available
        Process current_job = ready_queue.top();
        ready_queue.pop();

        // Execute the job non-preemptively
        current_job.completion_time = current_time + current_job.burst_time;
        current_job.turnaround_time = current_job.completion_time - current_job.arrival_time;
        current_job.waiting_time = current_job.turnaround_time - current_job.burst_time;

        // Advance system clock
        current_time = current_job.completion_time;

        total_waiting_time += current_job.waiting_time;
        total_turnaround_time += current_job.turnaround_time;
        
        completed_processes.push_back(current_job);
    }

    // 3. Sort by ID for clean tabular output
    std::sort(completed_processes.begin(), completed_processes.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    // 4. Output results
    std::cout << "\n" << std::setw(5) << "PID" 
              << std::setw(15) << "Arrival Time" 
              << std::setw(12) << "Burst Time" 
              << std::setw(18) << "Completion Time" 
              << std::setw(18) << "Turnaround Time" 
              << std::setw(15) << "Waiting Time" << "\n";
    
    std::cout << std::string(83, '-') << "\n";

    for (const auto& p : completed_processes) {
        std::cout << std::setw(5) << p.id 
                  << std::setw(15) << p.arrival_time 
                  << std::setw(12) << p.burst_time 
                  << std::setw(18) << p.completion_time 
                  << std::setw(18) << p.turnaround_time 
                  << std::setw(15) << p.waiting_time << "\n";
    }

    std::cout << std::string(83, '-') << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Waiting Time: " << (total_waiting_time / completed_processes.size()) << "\n";
    std::cout << "Average Turnaround Time: " << (total_turnaround_time / completed_processes.size()) << "\n";
}

int main() {
    // Dataset chosen to highlight SJF vs FCFS behavior
    std::vector<Process> processes = {
        {1, 1, 6},  // PID 1: Arrives at 1, long job (6 units)
        {2, 2, 2},  // PID 2: Arrives at 2, short job (2 units)
        {3, 0, 8},  // PID 3: Arrives at 0, long job starts first because queue is empty at t=0
        {4, 3, 1}   // PID 4: Arrives at 3, shortest job (1 unit)
    };

    calculateSJF(processes);

    return 0;
}