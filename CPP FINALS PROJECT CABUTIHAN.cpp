#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib> //pang randomize ng number at pang clear screen
#include <ctime> //nag iiba yung time kada run, nagagamit to sa loan cd at car rng
#include <thread> //sleep_for
#include <chrono> //pang milliseconds
#include <limits> //nag lilimit sa mga value
using namespace std;

//const para fixed value kelangan to kasi masisira yung code at random values i-iinput neto
const int FINISH_LINE = 30;
const int NUM_CARS = 4;
const string MONEY_FILE = "money.txt";
const string LOAN_FILE = "loan.txt";

//long long data type mas malaki ang pwede netong number kesa sa int (mostly ginamit ko sya for time)
const long long LOAN_LIMIT = 5000;
const long long COOLDOWN_SECONDS = 3600;

const string CARS[NUM_CARS] = {
    "-o=o>",
    "=oOo>",
    "-o-o>",
    "-@=@>"
};

long long loanUsed = 0;
long long cooldownStart = 0;

void wait(int milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

void clearScreen() {
    system("cls");
}

void pauseEnter() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

void showInvalid(const string& msg = "Invalid selection!") {
    cout << "\n[!] " << msg << "\n";
    pauseEnter();
}

//input error checker
int getNumber(const string& message) {
    int number;
    cout << message;

    if (cin >> number) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return number;
    }

    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return -1;
}

//data saver
void saveMoney(int balance) {
    ofstream file(MONEY_FILE.c_str());
    if (file.is_open()) {
        file << balance;
        file.close();
    }
}

int loadMoney() {
    ifstream file(MONEY_FILE.c_str());
    int balance;

    if (file.is_open() && file >> balance && balance >= 0) {
        file.close();
        return balance;
    }
    if (file.is_open()) {
        file.close();
    }
    return 1000;
}

void saveLoan() {
    ofstream file(LOAN_FILE.c_str());
    if (file.is_open()) {
        file << loanUsed << " " << cooldownStart;
        file.close();
    }
}

void loadLoan() {
    ifstream file(LOAN_FILE.c_str());
    if (file.is_open()) {
        if (!(file >> loanUsed >> cooldownStart)) {
            loanUsed = 0;
            cooldownStart = 0;
        }
        file.close();
    }
    if (loanUsed < 0) loanUsed = 0;
}

void printResult(const int order[], int choice, int bet,
    int rank, int prize, int balance) {
    clearScreen();

    cout << "====================================\n";
    cout << "          RACE RESULTS\n";
    cout << "====================================\n\n";

    for (int i = 0; i < NUM_CARS; i++) {
        cout << i + 1 << ". Car " << order[i] + 1
            << "  " << CARS[order[i]] << "\n";
    }

    cout << "\nYour car: Car " << choice
        << " finished in place #" << rank << "\n";

    if (prize > 0) {
        cout << "\nCONGRATULATIONS! YOU WON!\n";
        cout << "Bet: " << bet << " credits\n";
        cout << "Prize: +" << prize << " credits\n";
    }
    else {
        cout << "\nNo prize this time!\n";
        cout << "Lost bet: " << bet << " credits\n";
    }

    cout << "\nYour balance: " << balance << " credits\n";
    cout << "\n====================================\n";
    cout << "1. Try another race\n";
    cout << "2. Leave the track\n";
    cout << "====================================\n";
}

void startRace(int& balance) {
    while (true) {
        //checker pag walang pera pang bet
        if (balance <= 0) {
            clearScreen();
            cout << "You have no credits left!\n";
            cout << "Go to 2. Your Bank Account to loan money.\n";
            pauseEnter();
            return;
        }

        clearScreen();
        cout << "====================================\n";
        cout << "          CHOOSE YOUR CAR\n";
        cout << "====================================\n\n";

        for (int i = 0; i < NUM_CARS; i++) {
            cout << i + 1 << ". " << CARS[i] << "  Car " << i + 1 << "\n";
        }
        cout << "0. Back to Menu\n";
        cout << "\nYour balance: " << balance << " credits\n";

        int choice = getNumber("\nChoose your car: ");

        if (choice == 0) {
            return;
        }
        if (choice < 1 || choice > NUM_CARS) {
            showInvalid();
            continue;
        }

        int bet = getNumber("Enter your bet: ");

        // checker pag sumobra or kulang bet
        if (bet <= 0 || bet > balance) {
            showInvalid("Invalid bet! Bet must be 1 up to your balance.");
            continue;
        }

        int position[NUM_CARS] = { 0, 0, 0, 0 }; //galaw ng tsikot
        int lastMove[NUM_CARS] = { 0, 0, 0, 0 }; //oras na tinitingnan kung kelan yung last galaw ng tsikot para maka move sa next
        int speed[NUM_CARS]; //delay kada galaw
        int order[NUM_CARS]; //finishing order (naka set na agad before pa matapos yung race)
        int finished = 0;
        int elapsed = 0;

        for (int i = 0; i < NUM_CARS; i++) {
            speed[i] = 30 + rand() % 121;
        }

        clearScreen();
        cout << "GET READY!\n";
        wait(1000);
        cout << "3...\n";
        wait(700);
        cout << "2...\n";
        wait(700);
        cout << "1...\n";
        wait(700);
        cout << "GO! GO! GO!\n";
        wait(500);

        //code behind every moving car
        //di titigil yong race hanggang di nag fifinish line
        while (finished < NUM_CARS) {
            elapsed += 10; // main reason kung paano nagalaw mga tsikot

            for (int i = 0; i < NUM_CARS; i++) {
                if (position[i] < FINISH_LINE &&
                    elapsed - lastMove[i] >= speed[i]) {

                    position[i]++;
                    lastMove[i] = elapsed;

                    if (position[i] >= FINISH_LINE) {
                        order[finished] = i;
                        finished++;
                    }
                }
            }

            clearScreen();
            cout << "===== DRAG RACE =====\n\n";

            for (int i = 0; i < NUM_CARS; i++) {
                cout << "Car " << i + 1 << " |";

                for (int j = 0; j < position[i]; j++) {
                    cout << " ";
                }

                cout << CARS[i];

                if (position[i] >= FINISH_LINE) {
                    cout << "  FINISH!";
                }

                cout << endl;
            }

            cout << endl;
            for (int j = 0; j < 7 + FINISH_LINE; j++) {
                cout << " ";
            }
            cout << "| FINISH LINE\n";

            wait(10);
        }

        wait(800);


        int rank = 0;
        for (int i = 0; i < NUM_CARS; i++) {
            if (order[i] == choice - 1) {
                rank = i + 1;
            }
        }


        int prize = 0;
        if (rank == 1) {
            prize = bet * 2;
        }
        else if (rank == 2) {
            prize = bet;
        }
        else if (rank == 3) {
            prize = (bet + 1) / 2;
        }

        if (rank == 4) {
            balance -= bet;
        }
        else {
            balance += prize;
        }

        saveMoney(balance);

        while (true) {
            printResult(order, choice, bet, rank, prize, balance);

            int next = getNumber("\nEnter choice: ");

            if (next == 1) {
                break;
            }
            if (next == 2) {
                return;
            }
            showInvalid();
        }
    }
}

//bank system
bool loanPage(int& balance) {
    while (true) {
        clearScreen();
        long long now = time(0);

        //loan cd
        if (loanUsed >= LOAN_LIMIT) {
            if (cooldownStart == 0) {
                cooldownStart = now;
                saveLoan();
            }
            if (now - cooldownStart >= COOLDOWN_SECONDS) {
                loanUsed = 0;
                cooldownStart = 0;
                saveLoan();
            }
        }

        bool onCooldown = (loanUsed >= LOAN_LIMIT);
        long long available = LOAN_LIMIT - loanUsed;

        cout << "====================================\n";
        cout << "            LOAN MONEY\n";
        cout << "====================================\n\n";
        cout << "Available loan: " << available << " credits\n";
        cout << "\n0. Go back to track\n";

        int amount = getNumber("\nEnter loan amount: ");

        if (amount == 0) {
            return true;
        }
        if (amount < 0) {
            showInvalid();
            continue;
        }

        //view loan cd
        if (onCooldown) {
            long long remaining = COOLDOWN_SECONDS - (time(0) - cooldownStart);
            if (remaining < 0) remaining = 0;

            cout << "\n!!! Loan is on cooldown.\n";
            cout << "You have used all of your available loan.\n";
            cout << "Try again in " << remaining / 60 << " min "
                << remaining % 60 << " sec.\n";
            pauseEnter();
            continue;
        }

        if (amount > available) {
            cout << "\nSYSTEM ERROR!!! You cannot exceed your available loan of "
                << available << " credits.\n";
            pauseEnter();
            continue;
        }

        balance += amount;
        loanUsed += amount;
        saveMoney(balance);

        cout << "\nLoan approved!\n";
        cout << "Borrowed: " << amount << " credits\n";
        cout << "New balance: " << balance << " credits\n";

        if (loanUsed >= LOAN_LIMIT) {
            cooldownStart = time(0);
            cout << "\nYou used all of your loan. Loan cooldown: 1 hour.\n";
            saveLoan();
            pauseEnter();
            return false;
        }

        saveLoan();
        pauseEnter();
    }
}

//return true if player choose go back to the track
bool balanceScreen(int& balance) {
    while (true) {
        clearScreen();
        cout << "====================================\n";
        cout << "          CURRENT BALANCE\n";
        cout << "====================================\n";
        cout << endl;
        cout << "Your balance: " << balance << " credits\n";
        cout << "\n1. Go back to track\n";
        cout << "2. Need more money? Loan money here\n";

        int sel = getNumber("\nEnter choice: ");

        if (sel == 1) {
            return true;
        }
        if (sel == 2) {
            return loanPage(balance);
        }
        showInvalid();
    }
}

void bankMenu(int& balance) {
    while (true) {
        clearScreen();
        cout << "====================================\n";
        cout << "         YOUR BANK ACCOUNT\n";
        cout << "====================================\n";
        cout << endl;
        cout << "1. Your current balance\n";
        cout << "2. Loan money\n";
        cout << "3. Go back to track\n";
        cout << "====================================\n";

        int sel = getNumber("\nEnter choice: ");

        if (sel == 1) {
            if (balanceScreen(balance)) return;
        }
        else if (sel == 2) {
            if (loanPage(balance)) return;
        }
        else if (sel == 3) {
            return;
        }
        else {
            showInvalid();
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    int balance = loadMoney();
    loadLoan();

    while (true) {
        clearScreen();

        cout << "====================================\n";
        cout << "       DRAG RACE BETTING GAME  \n";
        cout << "====================================\n";
        cout << endl;
        cout << "1. Start Race\n";
        cout << "2. Your Bank Account\n";
        cout << "3. Leave Track\n";
        cout << endl;
        cout << "====================================\n";

        int menuChoice = getNumber("\nEnter choice: ");

        if (menuChoice == 1) {
            startRace(balance);
        }
        else if (menuChoice == 2) {
            bankMenu(balance);
        }
        else if (menuChoice == 3) {
            saveMoney(balance);
            saveLoan();
            cout << "\nThanks for playing!\n";
            cout << "Your progress has been saved.\n";
            break;
        }
        else {
            showInvalid();
        }
    }

    return 0;
}
