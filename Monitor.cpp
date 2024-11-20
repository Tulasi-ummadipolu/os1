#include "Monitor.h"
#include <fstream>
#include <thread>
#include <ctime>

// Lock the monitor
void Monitor::enter() {
    std::unique_lock<std::mutex> lock(mutX);
    while (isLocked) {
        cv.wait(lock);
    }
    isLocked = true;
}

// Unlock the monitor
void Monitor::leave() {
    std::lock_guard<std::mutex> lock(mutX);
    isLocked = false;
    cv.notify_one();
}

// Display queue status
void Monitor::displayQueue() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::lock_guard<std::mutex> lock(mutX);

    std::cout << "[" << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << "] ";
    if (isLocked) {
        std::cout << "New process has entered the queue: ";
        if (!monitorQueue.empty()) {
            std::cout << monitorQueue.front() << "\n";
        } else {
            std::cout << "Queue is empty.\n";
        }
    } else {
        std::cout << "Process has left the queue: ";
        if (!monitorQueue.empty()) {
            std::cout << monitorQueue.front() << "\n";
            monitorQueue.pop();  // Remove the process from the queue
        } else {
            std::cout << "Queue is empty.\n";
        }
    }
}



// Add a process to the queue
void Monitor::addToQueue(const std::string& process) {
    std::lock_guard<std::mutex> lock(mutX);
    monitorQueue.push(process);
}

// Remove a process from the queue
void Monitor::removeFromQueue() {
    std::lock_guard<std::mutex> lock(mutX);
    if (!monitorQueue.empty()) {
        monitorQueue.pop();
    }
}

// Read input from a file
std::vector<std::string> Monitor::processInput(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            lines.push_back(line);
        }
        inFile.close();
    } else {
        std::cerr << "Unable to open file: " << filename << "\n";
    }
    return lines;
}
