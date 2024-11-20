#ifndef MONITOR_H
#define MONITOR_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <string>
#include <vector>

class Monitor {
public:
    void enter();                             // Enter the Monitor
    void leave();                             // Leave or close the Monitor
    void displayQueue();                      // Display queue status
    void addToQueue(const std::string& process);  // Add process to queue
    void removeFromQueue();                   // Remove process from queue

    std::vector<std::string> processInput(const std::string& filename);

private:
    std::mutex mutX;
    std::condition_variable cv;
    bool isLocked = false;                    // Monitor lock status
    std::queue<std::string> monitorQueue;     // Queue for processes
};

#endif
