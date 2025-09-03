/**
 * @brief Console-based Bank Management System in C++
 *
 * This program allows managing clients of a bank with operations like:
 * - Adding, deleting, updating, and finding clients.
 * - Handling transactions: deposit, withdraw, and balance reports.
 * - Persisting data into a text file.
 *
 * @author Madi Abdelheq
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cctype>

using namespace std;

/// File name used to store all client records.
const string FileName = "ClientDataFile.txt";

/// Enum for main menu options
enum enMainMenuOption { enShowClientList = 1, enAddNewClient = 2, enDeleteClient = 3, enUpdateClient = 4, enFindClient = 5, enTransactions = 6, Exit = 7 };

/// Enum for transactions menu options
enum enTransactionsMenuOptions { enDeposit = 1, enWithdraw = 2, enTotalBalances = 3, enMainMenu = 4};

/// Represents a single client’s data.
struct stClient {
	string AccountNumber, PinCode, FullName, PhoneNumber;
	double AccountBalance;
	bool MarkForDelete = false;
};


void ShowMainMenuScreen();

void ShowTransactionsMenuScreen();

// ---------- Helper & Input Functions ----------

/**
 * @brief Reads deposit amount from user.
 * @return The deposit amount.
 */
int ReadDepositAmount() {
	int DepositAmount = 0;
	cout << "\nPlease enter Deposit Amount? ";
	cin >> DepositAmount;

	return DepositAmount;
}

/**
 * @brief Reads withdraw amount from user.
 * @return The withdraw amount.
 */
double ReadWithdrawAmount() {
	double WithdrawAmount = 0;
	cout << "\nPlease enter Withdraw Amount? ";
	cin >> WithdrawAmount;

	return WithdrawAmount;
}

/**
 * @brief Splits a string into substrings by a given delimiter.
 * @param Text Input string.
 * @param delim Delimiter string.
 * @return Vector of substrings.
 */
vector <string> SplitString(string Text, string delim) {

	vector <string> vString{};
	short pos = 0;
	string sWord;

	while ((pos = Text.find(delim)) != std::string::npos) {
		sWord = Text.substr(0, pos);
		if (sWord != "")
			vString.push_back(sWord);
		Text.erase(0, pos + delim.length());
	}
	if (Text != "")
		vString.push_back(Text);

	return vString;
}

/**
 * @brief Converts a line from the file into a stClient record.
 * @param Line Raw line from file.
 * @param Seperator Delimiter between fields.
 * @return Parsed stClient record.
 */
stClient ConvertLineDataToRecord(string Line, string Seperator = "#//#") {

	stClient ClientData;
	vector <string> vClient;
	vClient = SplitString(Line, Seperator);

	ClientData.AccountNumber = vClient[0];
	ClientData.PinCode = vClient[1];
	ClientData.FullName = vClient[2];
	ClientData.PhoneNumber = vClient[3];
	ClientData.AccountBalance = stod(vClient[4]);

	return ClientData;
}

/**
 * @brief Loads all clients from file.
 * @param FileName The file to read from.
 * @return Vector of clients.
 */
vector <stClient> LoadClientsDataFromFile(string FileName) {

	vector <stClient> vFileContent;
	fstream MyFile;

	MyFile.open(FileName, ios::in);
	if (MyFile.is_open()) {
		string Line;
		stClient Client;

		while (getline(MyFile, Line)) {
			Client = ConvertLineDataToRecord(Line);
			vFileContent.push_back(Client);
		}

		MyFile.close();
	}

	return vFileContent;
}

/**
 * @brief Converts client record into a file line.
 * @param ClientData Client data.
 * @param Seprator Field separator.
 * @return String line for file storage.
 */
string ConvertRecordToLine(stClient& ClientData, string Seprator) {

	string stClientRecord = "";

	stClientRecord += ClientData.AccountNumber + Seprator;
	stClientRecord += ClientData.PinCode + Seprator;
	stClientRecord += ClientData.FullName + Seprator;
	stClientRecord += ClientData.PhoneNumber + Seprator;
	stClientRecord += to_string(ClientData.AccountBalance);

	return stClientRecord;
}

/* ================= Printing Functions ================= */

/**
 * @brief Prints a single client’s data.
 * @param ClientData Client record.
 */
void PrintClientsData(stClient ClientData) {

	cout << "| " << left << setw(15) << ClientData.AccountNumber;
	cout << "| " << left << setw(10) << ClientData.PinCode;
	cout << "| " << left << setw(40) << ClientData.FullName;
	cout << "| " << left << setw(12) << ClientData.PhoneNumber;
	cout << "| " << left << setw(12) << ClientData.AccountBalance;
}

/**
 * @brief Prints client account balance for total balances screen.
 * @param ClientData Client record.
 */
void PrintClientsDataForTotalBalances(stClient ClientData) {

	cout << "| " << left << setw(15) << ClientData.AccountNumber;
	cout << "| " << left << setw(40) << ClientData.FullName;
	cout << "| " << left << setw(12) << ClientData.AccountBalance;
}

/**
 * @brief Prints client list in table form.
 */
void PrintAllClientsData() {

	vector <stClient> vClients;
	vClients = LoadClientsDataFromFile(FileName);

	cout << "\n\t\t\t\t\tClient List (" << vClients.size() << ") Client(s).";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;
	cout << "| " << left << setw(15) << "Accout Number";
	cout << "| " << left << setw(10) << "Pin Code";
	cout << "| " << left << setw(40) << "Client Name";
	cout << "| " << left << setw(12) << "Phone";
	cout << "| " << left << setw(12) << "Balance";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;

	for (stClient& Client : vClients) {
		PrintClientsData(Client);
		cout << endl;
	}

	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;
}

/**
 * @brief Prints detailed information for a single client.
 * @param ClientData Client record.
 */
void PrintClientData(stClient ClientData) {

	cout << "\n\nThe Following are the client details: \n\n";
	cout << "Account Number  : " << ClientData.AccountNumber << endl;
	cout << "PinCode         : " << ClientData.PinCode << endl;
	cout << "Name            : " << ClientData.FullName << endl;
	cout << "Phone           : " << ClientData.PhoneNumber << endl;
	cout << "Account Balance : " << ClientData.AccountBalance << endl;
}

// ---------- File Operations ----------

/**
 * @brief Adds a new line of client data to file.
 * @param Line The formatted client data line.
 * @param FileName The target file.
 */
void AddClientDataLineToFile(string Line, string FileName) {

	fstream ClientDataFile;
	ClientDataFile.open(FileName, ios::out | ios::app);

	if (ClientDataFile.is_open()) {
		ClientDataFile << Line << endl;
		ClientDataFile.close();
	}
}

/**
 * @brief Saves client data into file.
 * @param FileName Target file.
 * @param vClients Vector of clients.
 * @return Vector of saved clients.
 */
vector <stClient> SaveClientDataToFile(string FileName, vector <stClient> vClients) {
	fstream MyFile;

	MyFile.open(FileName, ios::out);
	string DataLine;
	if (MyFile.is_open()) {

		for (stClient& C : vClients) {
			if (C.MarkForDelete != true) {
				DataLine = ConvertRecordToLine(C, "#//#");
				MyFile << DataLine << endl;
			}
		}

		MyFile.close();
	}

	return vClients;
}

/* ================= Client Management ================= */

/**
 * @brief Checks if an account number already exists.
 * @param AccountNumber The account number to check.
 * @return True if exists, false otherwise.
 */
bool CheckAccountNumberExist(string AccountNumber) {
	vector <stClient> vClientDataFile = LoadClientsDataFromFile("ClientDataFile.txt");

	for (stClient& C : vClientDataFile) {
		if (C.AccountNumber == AccountNumber) {
			cout << "Client With [" << AccountNumber << "] already exists, Enter another Account Number? ";
			return true;
		}
	}
	return false;
}

/**
 * @brief Reads client data from user input.
 * @param ClientData Reference to stClient.
 * @return Filled client record.
 */
stClient ReadClientData(stClient& ClientData) {

	cout << "Enter Account Number? ";
	do {
		getline(cin >> ws, ClientData.AccountNumber);
	} while (CheckAccountNumberExist(ClientData.AccountNumber));

	cout << "Enter PinCode? ";
	getline(cin, ClientData.PinCode);

	cout << "Enter Name? ";
	getline(cin, ClientData.FullName);

	cout << "Enter Phone? ";
	getline(cin, ClientData.PhoneNumber);

	cout << "Enter AccountBalance? ";
	cin >> ClientData.AccountBalance;

	return ClientData;
}

/**
 * @brief Reads an account number from user input.
 * @return Account number.
 */
string ReadClientAccountNumber() {
	string AccountNumber;

	cout << "\nPlease Enter Account Number? ";
	cin >> AccountNumber;

	return AccountNumber;
}

/**
 * @brief Finds a client by account number.
 * @param AccountNumber Account number.
 * @param vClients Vector of clients.
 * @param Client Output client.
 * @return True if found, false otherwise.
 */
bool FindClientByAccountNumber(string AccountNumber, vector <stClient> vClients, stClient& Client) {

	for (stClient& C : vClients) {
		if (C.AccountNumber == AccountNumber) {
			Client = C;
			return true;
		}
	}

	return false;
}

/**
 * @brief Marks a client for deletion by account number.
 * @param AccountNumber Account number.
 * @param vClients Vector of clients.
 * @return True if marked, false otherwise.
 */
bool MarkClientForDeleteByAccountNumber(string AccountNumber, vector <stClient>& vClients) {
	for (stClient& C : vClients) {
		if (C.AccountNumber == AccountNumber) {
			C.MarkForDelete = true;
			return true;
		}
	}

	return false;
}

/**
 * @brief Deletes a client by account number.
 * @param AccountNumber Account number.
 * @param vClients Vector of clients.
 * @return True if deleted, false otherwise.
 */
bool DeleteClientByAccountNumber(string AccountNumber, vector <stClient>& vClients) {

	stClient Client;
	char Answer = 'N';

	if (FindClientByAccountNumber(AccountNumber, vClients, Client)) {
		PrintClientData(Client);

		cout << "\nAre you sure you want to delete this client? Y/N? ";
		cin >> Answer;
		if (toupper(Answer) == 'Y') {
			MarkClientForDeleteByAccountNumber(AccountNumber, vClients);
			SaveClientDataToFile(FileName, vClients);

			vClients = LoadClientsDataFromFile(FileName);

			cout << "\n\nClient deleted Successfully" << endl;
			return true;
		}
	}
	else {
		cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!\n";
		return false;
	}
}

/**
 * @brief Updates a client’s record by re-entering data.
 * @param AccountNumber The account number.
 * @return Updated client record.
 */
stClient UpdateClientRecord(string AccountNumber) {

	stClient ClientData;

	ClientData.AccountNumber = AccountNumber;

	cout << "\nEnter PinCode? ";
	getline(cin >> ws, ClientData.PinCode);

	cout << "Enter Name? ";
	getline(cin, ClientData.FullName);

	cout << "Enter Phone? ";
	getline(cin, ClientData.PhoneNumber);

	cout << "Enter AccountBalance? ";
	cin >> ClientData.AccountBalance;

	return ClientData;
}

/**
 * @brief Updates a client in file by account number.
 * @param AccountNumber The account number.
 * @param vClients Vector of clients.
 * @return True if updated, false otherwise.
 */
bool UpdateClientByAccountNumber(string AccountNumber, vector <stClient>& vClients) {

	stClient Client;
	char Answer = 'N';

	if (FindClientByAccountNumber(AccountNumber, vClients, Client)) {
		PrintClientData(Client);

		cout << "\nAre you sure you want to Update this client? Y/N? ";
		cin >> Answer;
		if (toupper(Answer) == 'Y') {
			for (stClient& C : vClients) {
				if (C.AccountNumber == AccountNumber) {
					C = UpdateClientRecord(AccountNumber);
					break;
				}
			}
			SaveClientDataToFile(FileName, vClients);

			cout << "\n\nClient Updated Successfully" << endl;
			return true;
		}
	}
	else {
		cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!\n";
		return false;
	}
}

/**
 * @brief Adds a single client to the file.
 */
void AddNewClients() {
	stClient ClientData;
	ReadClientData(ClientData);

	AddClientDataLineToFile(ConvertRecordToLine(ClientData, "#//#"), "ClientDataFile.txt");
}

/**
 * @brief Adds multiple clients interactively.
 */
void AddClient() {
	char AddMore = 'Y';

	do {
		cout << "Adding New Client:\n\n";
		AddNewClients();
		cout << "\nClient Added Successfully, do you want to add more clients? Y/N? ";
		cin >> AddMore;
	} while (toupper(AddMore) == 'Y');
}

/* ================= Transactions ================= */

/**
 * @brief Performs deposit operation for a client.
 * @return True if successful.
 */
bool DepositAmountByClientNumber() {

	vector <stClient> vClients = LoadClientsDataFromFile(FileName);
	string AccountNumber = ReadClientAccountNumber();
	stClient Client;
	char Answer = 'N';

	while (!FindClientByAccountNumber(AccountNumber, vClients, Client)) {
		cout << "Client with [" << AccountNumber << "] does not Found!\n";
		AccountNumber = ReadClientAccountNumber();
	}

	PrintClientData(Client);
	double DepositAmount = ReadDepositAmount();

	cout << "Are you Sure you want perform this transaction? y/n ? ";
	cin >> Answer;

	if (toupper(Answer) == 'Y') {
		for (stClient& C : vClients) {
			if (C.AccountNumber == AccountNumber) {
				C.AccountBalance += DepositAmount;
				break;
			}
		}
		SaveClientDataToFile(FileName, vClients);

		cout << "\n\nAmount Deposit Successfully" << endl;
		return true;
	}

	return false;
}

/**
 * @brief Performs withdrawal operation for a client.
 * @return True if successful.
 */
bool WithdrawAmountByClientNumber() {

	vector <stClient> vClients = LoadClientsDataFromFile(FileName);
	string AccountNumber = ReadClientAccountNumber();
	stClient Client;
	char Answer = 'N';

	while (!FindClientByAccountNumber(AccountNumber, vClients, Client)) {
		cout << "Client with [" << AccountNumber << "] does not Found!\n";
		AccountNumber = ReadClientAccountNumber();
	}

	PrintClientData(Client);
	double WithdrawAmount = ReadWithdrawAmount();

	while (WithdrawAmount > Client.AccountBalance) {
		cout << "Amoount Exceeds the balance, you can withdraw up to : " << Client.AccountBalance << endl;
		WithdrawAmount = ReadWithdrawAmount();
	}

	cout << "Are you Sure you want perform this transaction? y/n ? ";
	cin >> Answer;

	if (toupper(Answer) == 'Y') {
		for (stClient& C : vClients) {
			if (C.AccountNumber == AccountNumber) {
				C.AccountBalance -= WithdrawAmount;
				break;
			}
		}
		SaveClientDataToFile(FileName, vClients);

		cout << "\n\nAmount Withdraw Successfully" << endl;
		return true;
	}

	return false;
}

/**
 * @brief Prints total balances report.
 */
void ShowTotalBalnces() {
	vector <stClient> vClients;
	vClients = LoadClientsDataFromFile(FileName);
	int TotalBalances = 0;

	cout << "\n\t\t\t\t\tClient List (" << vClients.size() << ") Client(s).";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;
	cout << "| " << left << setw(15) << "Accout Number";
	cout << "| " << left << setw(40) << "Client Name";
	cout << "| " << left << setw(12) << "Balance";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;

	for (stClient& Client : vClients) {
		PrintClientsDataForTotalBalances(Client);
		TotalBalances += Client.AccountBalance;
		cout << endl;
	}

	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;

	cout << "\t\t\t\tTotal Balances = " << TotalBalances;
}

// ---------- UI Screens ----------

/**
 * @brief Shows delete client screen.
 */
void ShowDeleteClientScreen() {

	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tDelete Client Screen\n";
	cout << "---------------------------------------------------------------\n";

	string AccountNumber = ReadClientAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(FileName);

	DeleteClientByAccountNumber(AccountNumber, vClients);
}

/**
 * @brief Shows add new client screen.
 */
void ShowAddNewClientScreen() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tAdd New Clients Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	AddClient();
}

/**
 * @brief Shows update client info screen.
 */
void ShowUpdateClientInfoScreen() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tUpdate Client Info Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	string AccountNumber = ReadClientAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(FileName);

	UpdateClientByAccountNumber(AccountNumber, vClients);
}

/**
 * @brief Shows find client screen.
 */
void ShowFindClientScreen() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tUpdate Client Info Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	stClient Client;
	string AccountNumber = ReadClientAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(FileName);

	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
		PrintClientData(Client);
	else
		cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!\n";

}

/**
 * @brief Displays program end screen.
 */
void ShowEndScreen() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tProgram Ends\n";
	cout << "---------------------------------------------------------------\n\n";
}

/**
 * @brief Shows deposit screen.
 */
void ShowDepositScreen() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tDeposit Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	DepositAmountByClientNumber();
}

/**
 * @brief Shows withdraw screen.
 */
void ShowWithdrawScreen() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tWithdraw Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	WithdrawAmountByClientNumber();
}

// ---------- Navigation ----------

/**
 * @brief Pauses and returns to main menu.
 */
void GoBackToMainMenu() {
	cout << "\n\nPress any key to back to Main Menu..." << endl;
	system("pause>0");
	ShowMainMenuScreen();
}

/**
 * @brief Pauses and returns to transactions menu.
 */
void GoBackToTransactionsMenu() {
	cout << "\n\nPress any key to back to Transactions Menu..." << endl;
	system("pause>0");
	ShowTransactionsMenuScreen();
}

// ---------- Menu Input & Execution ----------

/**
 * @brief Reads main menu option from user.
 * @return Selected option.
 */
enMainMenuOption ReadMainMenuOption() {
	short MainMenuOption = 0;

	cout << "Choose What do you want to do? [1 to 7]? ";
	cin >> MainMenuOption;

	return (enMainMenuOption)MainMenuOption;

}

/**
 * @brief Reads transactions menu option from user.
 * @return Selected option.
 */
enTransactionsMenuOptions ReadTransactionsMenuOption() {
	short TransactionsMenuOption = 0;

	cout << "Choose What do you want to do? [1 to 4]? ";
	cin >> TransactionsMenuOption;

	return (enTransactionsMenuOptions)TransactionsMenuOption;

}

/**
 * @brief Handles selected option from transactions menu.
 * @param TransactionsMenuOptions The selected option.
 */
void PerformTransactionsMenuoption(enTransactionsMenuOptions TransactionsMenuOptions) {
	switch (TransactionsMenuOptions) {
	case enDeposit: {
		system("cls");
		ShowDepositScreen();
		ShowTransactionsMenuScreen();
		break;
	}
	case enWithdraw: {
		system("cls");
		ShowWithdrawScreen();
		ShowTransactionsMenuScreen();
		break;
	}
	case enTotalBalances: {
		system("cls");
		ShowTotalBalnces();
		GoBackToTransactionsMenu();
		break;
	}
	case enMainMenu: {
		system("cls");
		ShowMainMenuScreen();
		break;
	}
	}
}

/**
 * @brief Handles selected option from main menu.
 * @param MainMenuOption The selected option.
 */
void PerformMainMenuOption(enMainMenuOption MainMenuOption) {
	switch (MainMenuOption) {
	case enShowClientList: {
		system("cls");
		PrintAllClientsData();
		GoBackToMainMenu();
		break;
	};
	case enAddNewClient: {
		system("cls");
		ShowAddNewClientScreen();
		GoBackToMainMenu();
		break;
	}
	case enDeleteClient: {
		system("cls");
		ShowDeleteClientScreen();
		GoBackToMainMenu();
		break;
	}
	case enUpdateClient: {
		system("cls");
		ShowUpdateClientInfoScreen();
		GoBackToMainMenu();
		break;
	}
	case enFindClient: {
		system("cls");
		ShowFindClientScreen();
		GoBackToMainMenu();
		break;
	}
	case enTransactions: {
		system("cls");
		ShowTransactionsMenuScreen();
		break;
	}
	case Exit: {
		system("cls");
		ShowEndScreen();
		break;
	}
	}
}

/* ================= Menu Screens ================= */

/**
 * @brief Displays main menu screen.
 */
void ShowMainMenuScreen() {
	system("cls");
	cout << "========================================\n";
	cout << "\t\tMain Menu Screen\n";
	cout << "========================================\n";
	cout << "\t[1] Show Client List.\n";
	cout << "\t[2] Add New Client.\n";
	cout << "\t[3] Delete Client.\n";
	cout << "\t[4] Update Client Info.\n";
	cout << "\t[5] Find Client.\n";
	cout << "\t[6] Transactions.\n";
	cout << "\t[7] Exit.\n";
	cout << "========================================\n" << endl;

	PerformMainMenuOption(ReadMainMenuOption());
}

/**
 * @brief Displays transactions menu screen.
 */
void ShowTransactionsMenuScreen() {
	system("cls");
	cout << "========================================\n";
	cout << "\t\Transactions Menu Screen\n";
	cout << "========================================\n";
	cout << "\t[1] Deposit.\n";
	cout << "\t[2] Withdraw.\n";
	cout << "\t[3] Total Balances.\n";
	cout << "\t[4] Main Menu.\n";
	cout << "========================================\n" << endl;

	PerformTransactionsMenuoption(ReadTransactionsMenuOption());
}

/* ================= Main ================= */

int main()
{
	ShowMainMenuScreen();
    return 0;
}