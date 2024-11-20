#ifndef BANK_FUNCTIONS_H
#define BANK_FUNCTIONS_H

#include <string>
#include <vector>

// Declare existing functions
std::vector<std::string> processInput(const std::string& filename);
void createAccount(const std::string& id, int startingAmount);
int balanceInquiry(const std::string& filename);
void deposit(const std::string& id, int amount);
bool withdraw(const std::string& id, int amount);
void transfer(const std::string& from, const std::string& to, int amount);
void closeAccount(const std::string& id);

// Declare printTimestamp for shared use
void printTimestamp();

#endif // BANK_FUNCTIONS_H
