#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cctype>

using namespace std;

/// Files name used to store all clients and users records.
const string ClientFileName = "ClientDataFile.txt";
const string UserFileName = "Users.txt";

/// Enum for main menu options
enum enMainMenuOption { enShowClientList = 1, enAddNewClient = 2, enDeleteClient = 3, enUpdateClient = 4, enFindClient = 5, enTransactions = 6, enManageUsers = 7, Logout = 8 };

/// Enum for transactions menu options
enum enTransactionsMenuOptions { enDeposit = 1, enWithdraw = 2, enTotalBalances = 3, enMainMenuTransactions = 4 };

/// Enum for Manage user menu options
enum enManageUserMenuOptions { enShowUsersList = 1, enAddNewUser = 2, enDeleteUser = 3, enUpdateUser = 4, enFindUser = 5, enMainMenuUsers = 6 };

/// Enum for Main Menu permissions
enum enMainMenuPermissions { eAll = -1, pListClients = 1, pAddNewClients = 2, pDeleteClient = 4, pUpdateClient = 8, pFindClient = 16, pTransactions = 32, pManageUsers = 64 };

/// Represents a single client’s data.
struct stClient {
	string AccountNumber, PinCode, FullName, PhoneNumber;
	double AccountBalance;
	bool MarkForDelete = false;
};

/// Represents a single user’s data.
struct stUser {
	string UserName;
	string Password;
	int Permissions;
	bool MarkForDelete = false;
};


stUser CurrentUser;

void ShowMainMenuScreen();
void ShowTransactionsMenuScreen();
void Login();
void ShowManageUsersMenuScreen();
void ShowAccesDeniedMessage();
void GoBackToMainMenu();

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
stClient ConvertClientsLineDataToRecord(string Line, string Seperator = "#//#") {

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
 * @brief Converts a line from the file into a stUser record.
 * @param Line Raw line from file.
 * @param Seperator Delimiter between fields.
 * @return Parsed stUser record.
 */
stUser ConvertUsersLineDataToRecord(string Line, string Seperator = "#//#") {

	stUser User;
	vector <string> vUser;

	vUser = SplitString(Line, Seperator);

	User.UserName = vUser[0];
	User.Password = vUser[1];
	User.Permissions = stoi(vUser[2]);

	return User;
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
			Client = ConvertClientsLineDataToRecord(Line);
			vFileContent.push_back(Client);
		}

		MyFile.close();
	}

	return vFileContent;
}

/**
 * @brief Loads all Users from file.
 * @param FileName The file to read from.
 * @return Vector of users.
 */
vector <stUser> LoadUsersDataFromFile(string FileName) {

	vector <stUser> vFileContent;

	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open()) {
		string Line;
		stUser User;

		while (getline(MyFile, Line)) {
			User = ConvertUsersLineDataToRecord(Line);
			vFileContent.push_back(User);
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

/**
 * @brief Converts user record into a file line.
 * @param User user data.
 * @param Seprator Field separator.
 * @return String line for file storage.
 */
string ConvertRecordToLine(stUser& User, string Seprator) {

	string stUserRecord = "";

	stUserRecord += User.UserName + Seprator;
	stUserRecord += User.Password + Seprator;
	stUserRecord += to_string(User.Permissions);

	return stUserRecord;
}

/**
 * @brief Checks if the currently logged-in user has access to a given permission.
 *
 * @param Permission The required permission to check (from enMainMenuPermissions).
 * @return true  If the current user has the required permission.
 * @return false If the current user does not have the required permission.
 */
bool CheckAccessPermission(enMainMenuPermissions Permission) {

	if (CurrentUser.Permissions == eAll)
		return true;

	if ((Permission & CurrentUser.Permissions) == Permission)
		return true;
	else
		return false;
}

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
 * @brief Prints a single user’s data.
 * @param user user record.
 */
void PrintUsersData(stUser User) {

	cout << "| " << left << setw(15) << User.UserName;
	cout << "| " << left << setw(10) << User.Password;
	cout << "| " << left << setw(40) << User.Permissions;
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
 * @brief Displays a formatted list of all clients in the system.
 *
 * This function:
 * - First checks if the current user has the permission to view clients
 *   using CheckAccessPermission(pListClients).
 * - If access is denied, it shows an "Access Denied" message and
 *   redirects back to the main menu.
 * - If access is granted, it loads all clients from the file
 *   specified by ClientFileName.
 * - Prints a formatted table with columns for:
 *   - Account Number
 *   - Pin Code
 *   - Client Name
 *   - Phone
 *   - Balance
 */
void PrintAllClientsData() {

	if (!CheckAccessPermission(pListClients)) {
		ShowAccesDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	vector <stClient> vClients;
	vClients = LoadClientsDataFromFile(ClientFileName);

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
 * @brief Displays a formatted list of all users in the system.
 */
void PrintAllUsersData() {

	vector <stUser> vUsers;
	vUsers = LoadUsersDataFromFile(UserFileName);

	cout << "\n\t\t\t\t\tUsers List (" << vUsers.size() << ") User(s).";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;
	cout << "| " << left << setw(15) << "User Name";
	cout << "| " << left << setw(10) << "Password";
	cout << "| " << left << setw(40) << "Permissions";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;

	if (vUsers.size() == 0)
		cout << "\t\t\t\tNo users Availabla in the System!";
	else {
		for (stUser& User : vUsers) {
			PrintUsersData(User);
			cout << endl;
		}
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

/**
 * @brief Prints detailed information for a single user.
 * @param User user record.
 */
void PrintUserData(stUser User) {

	cout << "\n\nThe Following are the User details: \n\n";
	cout << "Username    : " << User.UserName << endl;
	cout << "Password    : " << User.Password << endl;
	cout << "Permissions : " << User.Permissions << endl;
}

/**
 * @brief Appends a line of text to the end of a file.
 *
 * This function opens a file in append mode and writes the given line
 * followed by a newline character. If the file cannot be opened, the
 * function does nothing.
 *
 * @param Line The text line to append to the file.
 * @param FileName The name (or path) of the file to which the line will be written.
 */
void AddDataLineToFile(string Line, string FileName) {

	fstream DataFile;
	DataFile.open(FileName, ios::out | ios::app);

	if (DataFile.is_open()) {
		DataFile << Line << endl;
		DataFile.close();
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

/**
 * @brief Saves user data into file.
 * @param FileName Target file.
 * @param vUsers Vector of users.
 * @return Vector of saved users.
 */
vector <stUser> SaveUserDataToFile(string FileName, vector <stUser> vUsers) {
	fstream MyFile;

	MyFile.open(FileName, ios::out);
	string DataLine;
	if (MyFile.is_open()) {

		for (stUser& U : vUsers) {
			if (U.MarkForDelete != true) {
				DataLine = ConvertRecordToLine(U, "#//#");
				MyFile << DataLine << endl;
			}
		}

		MyFile.close();
	}

	return vUsers;
}

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
 * @brief Checks if a given username already exists in the users database.
 *
 * This function loads all users from the file, then iterates through the list
 * to determine if the provided username is already in use.
 *
 * @param UserName The username to check for existence.
 * @return True if the username already exists, otherwise false.
 */
bool CheckUserNameExist(string UserName) {
	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);

	for (stUser& U : vUsers) {
		if (U.UserName == UserName) {
			cout << "User With [" << UserName << "] already exists, Enter another UserName? ";
			return true;
		}
	}
	return false;
}

/**
 * @brief Searches for a user by username in the users database.
 *
 * This function loads all users from the file and iterates through them to find
 * a match with the provided username. If a match is found, the corresponding
 * user data is copied into the output parameter.
 *
 * @param UserName The username to search for.
 * @param User Reference to a stUser object where the found user data will be stored.
 * @return True if the user was found, otherwise false.
 */
bool FindUserByUserName(string UserName, stUser& User) {
	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);

	for (stUser& U : vUsers) {
		if (U.UserName == UserName) {
			User = U;
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
 * @brief Reads and assigns access permissions for a user.
 *
 * This function interacts with the console to configure a user's permissions.
 * It first checks if the user should have full access (all permissions).
 * If not, it prompts the administrator to grant specific permissions one by one.
 *
 * Permissions are represented as integer flags (bitmask).
 * - If full access is granted, the function returns -1 (special flag for all permissions).
 * - Otherwise, the function accumulates the selected permissions and returns the sum.
 *`
 *
 * @return An integer representing the user's granted permissions.
 */
int ReadPermissions() {
	char Answer = 'n';
	int Permissions = 0;

	do {
		cout << "\nDo you want to give full access? Y/N? ";
		cin >> Answer;
	} while (toupper(Answer) != 'Y' && toupper(Answer) != 'N');

	if (toupper(Answer) == 'Y')
		return -1;

	cout << "\nDo you want to give to? \n";

	cout << "\nShow Client List? y/n? ";
	cin >> Answer;
	if (toupper(Answer) == 'Y')
		Permissions += pListClients;

	cout << "\nAdd New Client? y/n? ";
	cin >> Answer;
	if (toupper(Answer) == 'Y')
		Permissions += pAddNewClients;

	cout << "\nDelete Client? y/n? ";
	cin >> Answer;
	if (toupper(Answer) == 'Y')
		Permissions += pDeleteClient;

	cout << "\nUpdate Client? y/n? ";
	cin >> Answer;
	if (toupper(Answer) == 'Y')
		Permissions += pUpdateClient;

	cout << "\nFind Client? y/n? ";
	cin >> Answer;
	if (toupper(Answer) == 'Y')
		Permissions += pFindClient;

	cout << "\nTransactions? y/n? ";
	cin >> Answer;
	if (toupper(Answer) == 'Y')
		Permissions += pTransactions;

	cout << "\nmanage Users? y/n? ";
	cin >> Answer;
	if (toupper(Answer) == 'Y')
		Permissions += pManageUsers;

	return Permissions;
}

/**
 * @brief Reads user data from user input.
 * @param User Reference to stUser.
 * @return Filled user record.
 */
stUser ReadUserData(stUser& User) {

	cout << "Enter UserName? ";
	do {
		getline(cin >> ws, User.UserName);
	} while (CheckUserNameExist(User.UserName));

	cout << "Enter Password? ";
	getline(cin, User.Password);

	User.Permissions = ReadPermissions();

	return User;
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
 * @brief Finds a user by username.
 * @param UserName username.
 * @param User Output user.
 * @param Password 
 * @return True if found, false otherwise.
 */
bool FindUserByUserNameAndPassword(string UserName, string Password, stUser& User) {
	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);

	for (stUser& U : vUsers) {
		if (U.UserName == UserName && U.Password == Password) {
			User = U;
			return true;
		}
	}

	return false;
}

/**
 * @brief Loads user information based on username and password.
 *
 * This function attempts to find a user in the system using the provided
 * username and password. If a matching user is found, the user's details
 * are loaded into the global variable `CurrentUser`.
 *
 * @param UserName The username to search for.
 * @param Password The password to validate.
 * @return True if the user was found and authenticated successfully,
 *         otherwise false.
 */
bool LoadUserInfo(string UserName, string Password) {

	if (FindUserByUserNameAndPassword(UserName, Password, CurrentUser))
		return true;
	else
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
 * @brief Marks a user for deletion by username.
 * @param Username.
 * @param vUsers Vector of users.
 * @return True if marked, false otherwise.
 */
bool MarkUserForDeleteByUsername(string Username, vector <stUser>& vUsers) {
	for (stUser& U : vUsers) {
		if (U.UserName == Username) {
			U.MarkForDelete = true;
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
			SaveClientDataToFile(ClientFileName, vClients);

			vClients = LoadClientsDataFromFile(ClientFileName);

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
 * @brief Deletes a user by username.
 * @param UserName.
 * @param vUsers Vector of users.
 * @return True if deleted, false otherwise.
 */
bool DeleteUserByUsername(string UserName, vector <stUser>& vUsers) {

	if (UserName == "Admin") {
		cout << "\n\nYou cannot Delete This User.";
		return false;
	}

	stUser User;
	char Answer = 'N';

	if (FindUserByUserName(UserName, User)) {
		PrintUserData(User);

		cout << "\nAre you sure you want to delete this client? Y/N? ";
		cin >> Answer;
		if (toupper(Answer) == 'Y') {
			MarkUserForDeleteByUsername(UserName, vUsers);
			SaveUserDataToFile(UserFileName, vUsers);

			vUsers = LoadUsersDataFromFile(UserFileName);

			cout << "\nUser deleted Successfully" << endl;
			return true;
		}
	}
	else {
		cout << "\nClient with Username (" << UserName << ") is Not Found!\n";
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
 * @brief Updates a user’s record by re-entering data.
 * @param Username.
 * @return Updated user record.
 */
stUser UpdateUserRecord(string Username) {

	stUser User;

	User.UserName = Username;

	cout << "Enter Password? ";
	getline(cin >> ws, User.Password);

	User.Permissions = ReadPermissions();

	return User;
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
			SaveClientDataToFile(ClientFileName, vClients);

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
 * @brief Updates a user in file by username.
 * @param Username.
 * @param vUsers Vector of users.
 * @return True if updated, false otherwise.
 */
bool UpdateUserByUsername(string Username, vector <stUser>& vUsers) {

	stUser User;
	char Answer = 'N';

	if (FindUserByUserName(Username, User)) {
		PrintUserData(User);

		cout << "\nAre you sure you want to Update this User? Y/N? ";
		cin >> Answer;
		if (toupper(Answer) == 'Y') {
			for (stUser& U : vUsers) {
				if (U.UserName == Username) {
					U = UpdateUserRecord(Username);
					break;
				}
			}
			SaveUserDataToFile(UserFileName, vUsers);

			cout << "\n\nUser Updated Successfully" << endl;
			return true;
		}
	}
	else {
		cout << "\nUser with User Name (" << Username << ") is Not Found!\n";
		return false;
	}
}

/**
 * @brief Adds a single client to the file.
 */
void AddNewClients() {
	stClient ClientData;
	ReadClientData(ClientData);

	AddDataLineToFile(ConvertRecordToLine(ClientData, "#//#"), "ClientDataFile.txt");
}

/**
 * @brief Adds a single user to the file.
 */
void AddNewUsers() {
	stUser User;
	ReadUserData(User);

	AddDataLineToFile(ConvertRecordToLine(User, "#//#"), "Users.txt");
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

/**
 * @brief Adds multiple users interactively.
 */
void AddUser() {
	char AddMore = 'Y';

	do {
		cout << "Adding New User:\n\n";
		AddNewUsers();
		cout << "\nUser Added Successfully, do you want to add more Users? Y/N? ";
		cin >> AddMore;
	} while (toupper(AddMore) == 'Y');
}

/**
 * @brief Performs deposit operation for a client.
 * @return True if successful.
 */
bool DepositAmountByClientNumber() {

	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
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
		SaveClientDataToFile(ClientFileName, vClients);

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

	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
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
		SaveClientDataToFile(ClientFileName, vClients);

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
	vClients = LoadClientsDataFromFile(ClientFileName);
	double TotalBalances = 0;

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

/**
 * @brief Displays the "Delete Client" screen and handles client removal.
 *
 * This function is responsible for the client deletion workflow. It first
 * checks whether the current user has the required permission to delete
 * clients (`pDeleteClient`). If access is denied, an error message is
 * displayed, and the user is redirected back to the main menu.
 *
 * If access is granted:
 *  - A header for the delete client screen is displayed.
 *  - The user is prompted to enter a client account number.
 *  - All client records are loaded from the file (`ClientFileName`).
 *  - The function `DeleteClientByAccountNumber()` is called to remove the
 *    client with the given account number.
 */
void ShowDeleteClientScreen() {

	if (!CheckAccessPermission(pDeleteClient)) {
		ShowAccesDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tDelete Client Screen\n";
	cout << "---------------------------------------------------------------\n";

	string AccountNumber = ReadClientAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);

	DeleteClientByAccountNumber(AccountNumber, vClients);
}

/**
 * @brief Displays the "Delete User" screen and removes a user by username.
 *
 * This function handles the process of deleting a user from the system.
 * It provides a simple console interface to:
 *  - Display a header for the "Delete User" screen.
 *  - Prompt the administrator to enter the username of the account to be deleted.
 *  - Load the list of existing users from the file (`UserFileName`).
 *  - Call `DeleteUserByUsername()` to remove the specified user from the system.
 */
void ShowDeleteUserScreen() {

	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tDelete User Screen\n";
	cout << "---------------------------------------------------------------\n";

	string UserName = "";

	cout << "\nEnter Username? ";
	cin >> UserName;

	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);

	DeleteUserByUsername(UserName, vUsers);
}

/**
 * @brief Displays the "Add New Clients" screen and handles client creation.
 *
 * This function provides a console interface for adding new clients to the system.
 * It performs the following steps:
 *  - Checks if the current user has the `pAddNewClients` permission.
 *    - If not, shows an "Access Denied" message and returns to the main menu.
 *  - Displays a formatted header for the "Add New Clients" screen.
 *  - Calls `AddClient()` to handle the actual client input and storage.
 */
void ShowAddNewClientScreen() {

	if (!CheckAccessPermission(pAddNewClients)) {
		ShowAccesDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tAdd New Clients Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	AddClient();
}

void ShowAddNewUserScreen() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tAdd New Users Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	AddUser();
}

void ShowUpdateClientInfoScreen() {

	if (!CheckAccessPermission(pFindClient)) {
		ShowAccesDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tUpdate Client Info Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	string AccountNumber = ReadClientAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);

	UpdateClientByAccountNumber(AccountNumber, vClients);
}

void ShowUpdateUserInfoScreen() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tUpdate User Info Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	string UserName = "";

	cout << "Enter Username? ";
	cin >> UserName;

	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);

	UpdateUserByUsername(UserName, vUsers);
}

void ShowFindClientScreen() {

	if (!CheckAccessPermission(pFindClient)) {
		ShowAccesDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tFind Client Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	stClient Client;
	string AccountNumber = ReadClientAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);

	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
		PrintClientData(Client);
	else
		cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!\n";

}

void ShowFindUserScreen() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "\t\tFind User Screen\n";
	cout << "---------------------------------------------------------------\n\n";

	stUser User;
	string UserName = "";

	cout << "Enter Username? ";
	cin >> UserName;

	if (FindUserByUserName(UserName, User))
		PrintUserData(User);
	else
		cout << "\nUser with User Name (" << UserName << ") is Not Found!\n";

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

void ShowAccesDeniedMessage() {
	cout << "\n---------------------------------------------------------------\n";
	cout << "Access Denied\n";
	cout << "You Don't have permissions to do this,\n";
	cout << "Please Contact your Admin\n";
	cout << "---------------------------------------------------------------\n\n";
}

/**
 * @brief Pauses and returns to main menu.
 */
void GoBackToMainMenu() {
	cout << "\n\nPress any key to back to Main Menu..." << endl;
	system("pause>0");
	ShowMainMenuScreen();
}

void GoBackToManageUsersMenu() {
	cout << "\n\nPress any key to back to Manage Users Menu..." << endl;
	system("pause>0");
	ShowManageUsersMenuScreen();
}

/**
 * @brief Pauses and returns to transactions menu.
 */
void GoBackToTransactionsMenu() {
	cout << "\n\nPress any key to back to Transactions Menu..." << endl;
	system("pause>0");
	ShowTransactionsMenuScreen();
}

/**
 * @brief Reads main menu option from user.
 * @return Selected option.
 */
enMainMenuOption ReadMainMenuOption() {
	short MainMenuOption = 0;

	cout << "Choose What do you want to do? [1 to 8]? ";
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

enManageUserMenuOptions ReadManageUsersMenuOption() {
	short ManageUsersMenuOption = 0;

	cout << "Choose What do you want to do? [1 to 6]? ";
	cin >> ManageUsersMenuOption;

	return (enManageUserMenuOptions)ManageUsersMenuOption;

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
	case enMainMenuTransactions: {
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
	}
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
	case enManageUsers: {
		system("cls");
		ShowManageUsersMenuScreen();
		break;
	}
	case Logout: {
		Login();
		break;
	}
	}
}

void PerformManageUsersMenuOptions(enManageUserMenuOptions ManageUsersMenuOptions) {
	switch (ManageUsersMenuOptions) {
	case enShowUsersList: {
		system("cls");
		PrintAllUsersData();
		GoBackToManageUsersMenu();
		break;
	}
	case enAddNewUser: {
		system("cls");
		ShowAddNewUserScreen();
		GoBackToManageUsersMenu();
		break;
	}
	case enDeleteUser: {
		system("cls");
		ShowDeleteUserScreen();
		GoBackToManageUsersMenu();
		break;
	}
	case enUpdateUser: {
		system("cls");
		ShowUpdateUserInfoScreen();
		GoBackToManageUsersMenu();
		break;
	}
	case enFindUser: {
		system("cls");
		ShowFindUserScreen();
		GoBackToManageUsersMenu();
		break;
	}
	case enMainMenuUsers: {
		system("cls");
		ShowMainMenuScreen();
		break;
	}
	}
}

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
	cout << "\t[7] Manage Users.\n";
	cout << "\t[8] Logout.\n";
	cout << "========================================\n" << endl;

	PerformMainMenuOption(ReadMainMenuOption());
}

void ShowTransactionsMenuScreen() {

	if (!CheckAccessPermission(pTransactions)) {
		ShowAccesDeniedMessage();
		GoBackToMainMenu();
		return;
	}

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

void ShowManageUsersMenuScreen() {

	if (!CheckAccessPermission(pManageUsers)) {
		ShowAccesDeniedMessage();
		GoBackToMainMenu();
		return;
	}

	system("cls");
	cout << "========================================\n";
	cout << "\t\Manage Users Menu Screen\n";
	cout << "========================================\n";
	cout << "\t[1] List Users.\n";
	cout << "\t[2] Add New User.\n";
	cout << "\t[3] Delete User.\n";
	cout << "\t[4] Update User.\n";
	cout << "\t[5] Find User.\n";
	cout << "\t[6] Main Menu.\n";
	cout << "========================================\n" << endl;

	PerformManageUsersMenuOptions(ReadManageUsersMenuOption());
}

/**
 * @brief Displays the login screen and authenticates the user.
 *
 * This function provides a simple login mechanism that prompts the user
 * for a username and password, validates the credentials, and grants
 * access to the main menu upon successful authentication.
 */
void Login() {

	bool LoginFaild = false;
	string UserName, Password;

	do {
		system("cls");
		cout << "========================================\n";
		cout << "\t\Login Screen\n";
		cout << "========================================\n";

		if (LoginFaild)
			cout << "Invalid UserName/Password!\n";

		cout << "Enter UserName?: ";
		cin >> UserName;

		cout << "Enter Password?: ";
		cin >> Password;

		LoginFaild = !LoadUserInfo(UserName, Password);

	} while (LoginFaild);

	ShowMainMenuScreen();
}

int main()
{

	Login();
	return 0;
}