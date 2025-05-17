#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Account {
private:
    int accountNumber;
    string holderName;
    double balance;
    char accountType;
public:
    Account(int accNumber, string holderName, double initialBalance, char accType) 
        : accountNumber(accNumber), holderName(holderName), 
          balance(initialBalance), accountType(accType) {}

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Deposit successful. New balance: " << balance << endl;
        } else {
            cout << "Invalid deposit amount." << endl;
        }
    }

    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            cout << "Withdrawal successful. New balance: " << balance << endl;
        } else {
            cout << "Invalid withdrawal amount or insufficient balance." << endl;
        }
    }

    void transfer(double amount, Account &destinationAccount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            destinationAccount.deposit(amount);
            cout << "Transfer successful." << endl;
        } else {
            cout << "Invalid transfer amount or insufficient balance." << endl;
        }
    }

    double getBalance() const {
        return balance;
    }

    void displayBalance() const {
        cout << "Account Balance: " << balance << endl;
    }

    int getAccountNumber() const {
        return accountNumber;
    }

    string getHolderName() const {
        return holderName;
    }

    char getAccountType() const {
        return accountType;
    }
};

class SavingsAccount : public Account {
private:
    double interestRate;
public:
    SavingsAccount(int accNumber, string holderName, double initialBalance, double rate) 
        : Account(accNumber, holderName, initialBalance, 'S'), interestRate(rate) {}

    void addInterest() {
        double interest = getBalance() * interestRate / 100;
        deposit(interest);
        cout << "Interest added: " << interest << endl;
    }

    double getInterestRate() const {
        return interestRate;
    }
};

class CheckingAccount : public Account {
private:
    double fee;
public:
    CheckingAccount(int accNumber, string holderName, double initialBalance, double rfee) 
        : Account(accNumber, holderName, initialBalance, 'C'), fee(rfee) {}

    void deductFee() {
        withdraw(fee);
        cout << "Fee deducted: " << fee << endl;
    }

    double getFee() const {
        return fee;
    }
};

class Transaction {
private:
    Account &sourceAccount;
    Account &destinationAccount;
    double amount;
public:
    Transaction(Account &source, Account &destination, double Tamount) 
        : sourceAccount(source), destinationAccount(destination), amount(Tamount) {}

    void execute() {
        cout << "Attempting to transfer " << amount << " from account " 
             << sourceAccount.getAccountNumber() << " to account " 
             << destinationAccount.getAccountNumber() << endl;
        sourceAccount.transfer(amount, destinationAccount);
    }
};

class User {
private:
    string username;
    int pin;
    vector<Account*> accounts;

public:
    User(string uname, int p) : username(uname), pin(p) {}

    string getUsername() const {
        return username;
    }

    int getPin() const {
        return pin;
    }

    void addAccount(Account* account) {
        accounts.push_back(account);
    }

    vector<Account*>& getAccounts() {
        return accounts;
    }
};

class ATM {
private:
    User* currentUser;
    vector<User> users;
    vector<Account*> allAccounts;  // To maintain account lifetime

    void loadUsers() {
        ifstream file("users.txt");
        if (file.is_open()) {
            string username;
            int pin;
            while (file >> username >> pin) {
                users.push_back(User(username, pin));
            }
            file.close();
        } else {
            cout << "Creating new users file..." << endl;
            ofstream newFile("users.txt");
            newFile << "john 5678\n";
            newFile.close();
            users.push_back(User("john", 5678));
        }
    }

    void saveUsers() {
        ofstream file("users.txt");
        for (const auto& user : users) {
            file << user.getUsername() << " " << user.getPin() << "\n";
        }
        file.close();
    }

    void initializeSampleData() {
        // Create sample accounts
        SavingsAccount* savings = new SavingsAccount(1001, "John Doe", 1000.0, 2.5);
        CheckingAccount* checking = new CheckingAccount(1002, "John Doe", 500.0, 10.0);
        
        // Store them to maintain lifetime
        allAccounts.push_back(savings);
        allAccounts.push_back(checking);

        // Add accounts to our default user
        for (auto& user : users) {
            if (user.getUsername() == "john") {
                user.addAccount(savings);
                user.addAccount(checking);
                break;
            }
        }
    }

public:
    ATM() : currentUser(nullptr) {
        loadUsers();
        initializeSampleData();
    }

    ~ATM() {
        saveUsers();
        // Clean up allocated accounts
        for (auto account : allAccounts) {
            delete account;
        }
    }

    bool authenticateUser() {
        string uname;
        int pin;
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter PIN: ";
        cin >> pin;

        for (auto& user : users) {
            if (uname == user.getUsername() && pin == user.getPin()) {
                currentUser = &user;
                cout << "Authentication successful!" << endl;
                return true;
            }
        }

        cout << "Invalid username or PIN." << endl;
        return false;
    }

    void run() {
        while (true) {
            if (!currentUser && !authenticateUser()) {
                continue;
            }

            displayMenu();
            int choice;
            cin >> choice;

            if (choice == 4) {
                cout << "Thank you for using the ATM. Goodbye!" << endl;
                currentUser = nullptr;
                return;
            }

            if (currentUser->getAccounts().empty()) {
                cout << "No accounts found. Please contact bank to open an account." << endl;
                continue;
            }

            // Display available accounts
            cout << "\nYour Accounts:" << endl;
            for (size_t i = 0; i < currentUser->getAccounts().size(); i++) {
                cout << i+1 << ". Account #" << currentUser->getAccounts()[i]->getAccountNumber() 
                     << " (" << (currentUser->getAccounts()[i]->getAccountType() == 'S' ? "Savings" : "Checking") << ")"
                     << " - Balance: $" << currentUser->getAccounts()[i]->getBalance() << endl;
            }
            cout << "Select account (1-" << currentUser->getAccounts().size() << "): ";
            int accountChoice;
            cin >> accountChoice;
            accountChoice--;  // Convert to 0-based index

            if (accountChoice < 0 || accountChoice >= currentUser->getAccounts().size()) {
                cout << "Invalid account selection." << endl;
                continue;
            }

            Account* currentAccount = currentUser->getAccounts()[accountChoice];

            switch (choice) {
                case 1: {
                    double amount;
                    cout << "Enter deposit amount: $";
                    cin >> amount;
                    currentAccount->deposit(amount);
                    break;
                }
                case 2: {
                    double amount;
                    cout << "Enter withdrawal amount: $";
                    cin >> amount;
                    currentAccount->withdraw(amount);
                    break;
                }
                case 3:
                    currentAccount->displayBalance();
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        }
    }

private:
    void displayMenu() {
        cout << "\nWelcome to the ATM, " << currentUser->getUsername() << "!" << endl;
        cout << "1. Deposit" << endl;
        cout << "2. Withdraw" << endl;
        cout << "3. Check Balance" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";
    }
};

int main() {
    ATM atm;
    atm.run();
    return 0;
}