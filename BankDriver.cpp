#include <iostream>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "Monitor.h"
#include "BankFunctions.h"

// Global monitor instance for synchronization
Monitor monitor;

void executeTransaction(const std::string& transaction) {
    std::istringstream ss(transaction);
    std::string accountId, transactionType, targetAccount;
    int amount = 0;

    ss >> accountId >> transactionType;

    if (transactionType == "Create") {
        if (!(ss >> amount)) {  // Validate that amount is provided
            printTimestamp();
            std::cerr << "Error: Invalid input format for transaction: " << transaction << "\n";
            return;
        }
        createAccount(accountId, amount);
    } else if (transactionType == "Deposit") {
        if (!(ss >> amount)) {  // Validate that amount is provided
            printTimestamp();
            std::cerr << "Error: Invalid input format for transaction: " << transaction << "\n";
            return;
        }
        deposit(accountId, amount);
    } else if (transactionType == "Withdraw") {
        if (!(ss >> amount)) {  // Validate that amount is provided
            printTimestamp();
            std::cerr << "Error: Invalid input format for transaction: " << transaction << "\n";
            return;
        }
        withdraw(accountId, amount);
    } else if (transactionType == "Transfer") {
        if (!(ss >> amount >> targetAccount)) {  // Validate amount and target account
            printTimestamp();
            std::cerr << "Error: Invalid input format for transaction: " << transaction << "\n";
            return;
        }
        transfer(accountId, targetAccount, amount);
    } else if (transactionType == "Inquiry") {
        if (ss.rdbuf()->in_avail() > 0) {  // Ensure no extra fields for Inquiry
            printTimestamp();
            std::cerr << "Error: Invalid input format for transaction: " << transaction << "\n";
            return;
        }
        int balance = balanceInquiry(accountId + ".txt");
        if (balance >= 0) {
            printTimestamp();
            std::cout << "Balance for account " << accountId << ": " << balance << "\n";
        }
    } else if (transactionType == "Close") {
        if (ss.rdbuf()->in_avail() > 0) {  // Ensure no extra fields for Close
            printTimestamp();
            std::cerr << "Error: Invalid input format for transaction: " << transaction << "\n";
            return;
        }
        closeAccount(accountId);
    } else {
        printTimestamp();
        std::cerr << "Error: Unknown transaction type in transaction: " << transaction << "\n";
    }
}

// Function to parse and execute a transaction
// void executeTransaction(const std::string& transaction) {
//     std::istringstream ss(transaction);
//     std::string accountId, transactionType, targetAccount;
//     int amount = 0;

//     ss >> accountId >> transactionType;
//     if (transactionType == "Withdraw") {
//         ss >> amount;
//         withdraw(accountId, amount);
//     } else if (transactionType == "Deposit") {
//         ss >> amount;
//         deposit(accountId, amount);
//     } else if (transactionType == "Create") {
//         ss >> amount;
//         createAccount(accountId, amount);
//     } else if (transactionType == "Inquiry") {
//         int balance = balanceInquiry(accountId + ".txt");
//         if (balance >= 0) {
//             printTimestamp();
//             std::cout << "Balance for account " << accountId << ": " << balance << "\n";
//         }
//     } else if (transactionType == "Close") {
//         closeAccount(accountId);
//     } else if (transactionType == "Transfer") {  // Add Transfer handling
//         ss >> amount >> targetAccount;
//         transfer(accountId, targetAccount, amount);
//     } else {
//         std::cerr << "Unknown transaction type: " << transactionType << "\n";
//     }
// }



// Function to fork processes and execute transactions
void forkProcesses(const std::vector<std::string>& transactions) {
    for (const auto& transaction : transactions) {
        pid_t pid = fork();  // Create a new process

        if (pid == 0) {  // Child process
            executeTransaction(transaction);  // Call executeTransaction
            exit(0);  // Exit the child process
        } else if (pid > 0) {  // Parent process
            wait(nullptr);  // Wait for the child process to complete
        } else {
            std::cerr << "Fork failed for transaction: " << transaction << "\n";
        }
    }
}




// Main function
int main() {
    // Load transactions from input file
    std::vector<std::string> transactions = monitor.processInput("input.txt");

    if (transactions.empty()) {
        std::cerr << "No transactions found in input file.\n";
        return 1;
    }

    // Extract the first line as the number of threads (not a transaction)
    int threadCount = 0;
    try {
        threadCount = std::stoi(transactions[0]);
        transactions.erase(transactions.begin()); // Remove the first line
    } catch (const std::exception& e) {
        std::cerr << "Invalid thread count in input file.\n";
        return 1;
    }

    // Display the transactions
    std::cout << "Transactions to process:\n";
    for (const auto& transaction : transactions) {
        monitor.addToQueue(transaction);
        std::cout << " - " << transaction << "\n";
    }

    // Process the transactions
    forkProcesses(transactions);

    // Display final queue status
    monitor.displayQueue();

    return 0;
}
