Execution command : 

g++ -std=c++11 -o BankSystem BankDriver.cpp Monitor.cpp BankFunctions.cpp -lpthread
./BankSystem


Test cases: 

Test Case 1: Basic Transactions
2
A100001 Create 1000
A100002 Create 500
A100001 Deposit 200
A100002 Withdraw 300
A100001 Inquiry
A100002 Inquiry
A100001 Close
A100002 Close


Test Case 2: Nonexistent Account Transactions
1
A200001 Deposit 100
A200001 Withdraw 50
A200001 Inquiry


Test Case 3: Simultaneous Transfers
2
A300001 Create 1000
A300002 Create 2000
A300001 Transfer 500 A300002
A300002 Transfer 300 A300001
A300001 Inquiry
A300002 Inquiry


Test Case 4: Deadlock Prevention in Transfers
3
A400001 Create 1000
A400002 Create 500
A400001 Transfer 400 A400002
A400002 Transfer 300 A400001

Test Case 5: Invalid Input Formats
1
A500001 Create
A500001 Deposit
A500001 Withdraw
Create 1000
Withdraw 100


Test Case 6: Edge Cases
1
A600001 Create 0
A600001 Deposit 0
A600001 Withdraw 0
A600001 Inquiry
A600001 Close


Test Case 7: Concurrent Transactions
3
A700001 Create 1000
A700002 Create 2000
A700003 Create 3000
A700001 Deposit 500
A700002 Withdraw 1500
A700003 Transfer 1000 A700001
A700001 Inquiry
A700002 Inquiry
A700003 Inquiry
