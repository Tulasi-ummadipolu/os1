#include "BankFunctions.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include "Monitor.h"

// Global Monitor instance
extern Monitor monitor;

struct TransactionLog {
    char accountId[20];
    char transactionType[20];
    int amount;
    char status[10];
    char timestamp[30];
};

// Logs transactions to shared memory
void logTransaction(const std::string& id, const std::string& type, int amount, const std::string& status) {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(TransactionLog) * 100, 0666 | IPC_CREAT);
    TransactionLog* log = (TransactionLog*)shmat(shmid, (void*)0, 0);

    for (int i = 0; i < 100; ++i) {
        if (log[i].accountId[0] == '\0') {  // Empty slot
            strncpy(log[i].accountId, id.c_str(), sizeof(log[i].accountId));
            strncpy(log[i].transactionType, type.c_str(), sizeof(log[i].transactionType));
            log[i].amount = amount;
            strncpy(log[i].status, status.c_str(), sizeof(log[i].status));

            auto now = std::chrono::system_clock::now();
            std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
            std::strftime(log[i].timestamp, sizeof(log[i].timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
            break;
        }
    }
    shmdt(log);  // Detach shared memory
}

void printTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << "] ";
}

std::vector<std::string> processInput(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return lines;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        lines.push_back(line);
    }

    inFile.close();
    return lines;
}

void createAccount(const std::string& id, int startingAmount) {
    monitor.enter();  // Lock the critical section
    std::string filename = id + ".txt";
    std::ofstream outFile(filename, std::ios::trunc);
    if (outFile.is_open()) {
        outFile << startingAmount;
        outFile.close();
        printTimestamp();
        std::cout << "Account " << id << " created with balance: " << startingAmount << "\n";
        logTransaction(id, "create", startingAmount, "success");
    } else {
        printTimestamp();
        std::cerr << "Error creating account for ID: " << id << "\n";
        logTransaction(id, "create", startingAmount, "failed");
    }
    monitor.leave();  // Unlock the critical section
}


void deposit(const std::string& id, int amount) {
    std::string filename = id + ".txt";
    int currentBalance = balanceInquiry(filename);
    
    if (currentBalance == -1) {  // Account does not exist
        printTimestamp();
        std::cerr << "Error: Cannot deposit to nonexistent account " << id << "\n";
        logTransaction(id, "deposit", amount, "failed");
        return;
    }

    currentBalance += amount;
    std::ofstream outFile(filename, std::ios::trunc);
    if (outFile.is_open()) {
        outFile << currentBalance;
        outFile.close();

        printTimestamp();
        std::cout << "Deposited " << amount << " to account " << id << ". New balance: " << currentBalance << "\n";
        logTransaction(id, "deposit", amount, "success");
    } else {
        printTimestamp();
        std::cerr << "Error: Could not update account " << id << "\n";
        logTransaction(id, "deposit", amount, "failed");
    }
}



bool withdraw(const std::string& id, int amount) {
    std::string filename = id + ".txt";
    int currentBalance = balanceInquiry(filename);
    
    if (currentBalance == -1) {  // Account does not exist
        printTimestamp();
        std::cerr << "Error: Cannot withdraw from nonexistent account " << id << "\n";
        logTransaction(id, "withdraw", amount, "failed");
        return false;
    }

    if (currentBalance >= amount) {
        currentBalance -= amount;
        std::ofstream outFile(filename, std::ios::trunc);
        if (outFile.is_open()) {
            outFile << currentBalance;
            outFile.close();

            printTimestamp();
            std::cout << "Withdrew " << amount << " from account " << id
                      << ". New balance: " << currentBalance << "\n";
            logTransaction(id, "withdraw", amount, "success");
            return true;
        } else {
            printTimestamp();
            std::cerr << "Error: Could not update account " << id << "\n";
            logTransaction(id, "withdraw", amount, "failed");
        }
    } else {
        printTimestamp();
        std::cerr << "Insufficient funds in account " << id << "\n";
        logTransaction(id, "withdraw", amount, "failed");
    }

    return false;
}

int balanceInquiry(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        printTimestamp();
        std::cerr << "Error: Account file " << filename << " not found.\n";
        return -1;
    }

    int balance;
    inFile >> balance;
    inFile.close();

    printTimestamp();
    std::cout << "Balance: " << balance << "\n";
    return balance;
}


void transfer(const std::string& from, const std::string& to, int amount) {
    monitor.enter();
    if (withdraw(from, amount)) {
        deposit(to, amount);
        logTransaction(from, "transfer", amount, "success");
    } else {
        logTransaction(from, "transfer", amount, "failed");
    }
    monitor.leave();
}

void closeAccount(const std::string& id) {
    std::string filename = id + ".txt";
    if (std::remove(filename.c_str()) == 0) {
        printTimestamp();
        std::cout << "Account " << id << " closed successfully\n";
        logTransaction(id, "close", 0, "success");
    } else {
        printTimestamp();
        std::cerr << "Error closing account " << id << "\n";
        logTransaction(id, "close", 0, "failed");
    }
}
