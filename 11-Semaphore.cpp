#include <mutex>
#include <condition_variable>

class RealWorldSemaphore {
private:
    int S;                              // The internal counter (same as S in your textbook)
    std::mutex mtx;                     // The lock that fixes the non-atomic data race
    std::condition_variable cv;         // The mechanism that fixes the CPU-burning spinlock

public:
    // Initialize the semaphore counter (e.g., S = 1 for binary, S = 3 for counting)
    explicit RealWorldSemaphore(int initial_count = 1) : S(initial_count) {}

    // The 'wait()' operation (Textbook P operation)
    void wait() {
        // Step 1: Lock the mutex immediately!
        std::unique_lock<std::mutex> lock(mtx);
        
        // Step 2: Replace the textbook "while(S <= 0);" spinlock with a smart sleep
        cv.wait(lock, [this]() { return S > 0; });
        
        // Step 3: Safely decrement S (guaranteed atomic because we hold the lock)
        S--; 
        
        // Mutex automatically unlocks here when 'lock' goes out of scope
    }

    // The 'signal()' operation (Textbook V operation)
    void signal() {
        // Step 1: Lock the mutex to safely modify S
        std::unique_lock<std::mutex> lock(mtx);
        
        // Step 2: Safely increment S
        S++;
        
        // Step 3: Wake up one thread that is sleeping inside the wait() queue
        cv.notify_one(); 
        
        // Mutex automatically unlocks here
    }
};