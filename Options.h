#pragma once

#include <string>

enum class MainMenuOptions {
	ADD_NEW_TABLE,
	VIEW_TABLE,
	GET_STATISTIC,
	PERFORM_ARITHMETIC_OP,
	PERFORM_JOIN,
	QUIT,
	INVALID
};

std::ostream& operator<<(std::ostream& os, MainMenuOptions opt) {
	switch (opt) {
		case MainMenuOptions::ADD_NEW_TABLE:
			os << "add new table";
			break;
		case MainMenuOptions::VIEW_TABLE:
			os << "view table - (you could optionally select columns)";
			break;
		case MainMenuOptions::GET_STATISTIC:
			os << "get table statistics";
			break;
		case MainMenuOptions::PERFORM_ARITHMETIC_OP:
			os << "perform arithmetic operations [+,-,*,/] on two columns row-wise";
			break;
		case MainMenuOptions::PERFORM_JOIN:
			os << "perform join [inner, outer-left,right,full] on two tables";
			break;
		case MainMenuOptions::QUIT:
			os << "quit application";
			break;
		case MainMenuOptions::INVALID:
			os << "invalid";
			break;
	}
	return os;
}

void getChoice(vector<std::string> &avail_choices, std::string &choice) {
	bool valid_choice = true;
	do {
		if (!valid_choice) {
			cout << "You have the following choices: ";
			for (auto c: avail_choices) cout << c << " ";
			cout << "\nPlease choose one from above: ";
		}
		getline(cin, choice);
		valid_choice = false;
		for (auto c: avail_choices) valid_choice |= (c == choice);
	} while(!valid_choice);
}

void getYesNoChoice(std::string &choice) {
	std::vector<std::string> avail_choices {"y", "Y", "n", "N"};
	getChoice(avail_choices, choice);
	choice[0] = choice[0] | (1<<5);
}

MainMenuOptions showMainMenu() {
	cout << "-----------------------------------------------------------\n";
	cout << "----------------TOOL MAIN MENU-----------------------------\n";
	cout << "\nChoose from the following options:\n";
	cout << "1 " << MainMenuOptions::ADD_NEW_TABLE << endl;	
	cout << "2 " << MainMenuOptions::VIEW_TABLE << endl;
	cout << "3 " << MainMenuOptions::GET_STATISTIC << endl;	
	cout << "4 " << MainMenuOptions::PERFORM_ARITHMETIC_OP << endl;	
	cout << "5 " << MainMenuOptions::PERFORM_JOIN << endl;	
	cout << "6 " << MainMenuOptions::QUIT << endl;
	cout << "-----------------------------------------------------------\n";
	cout << "Enter number (1-6): ";
	std::string choice;
	std::vector<std::string> avail_choices;
	for(int i = 1; i <= 6; i++) {
		char c = i + '0';
		string s = ""; s = s + c;
		avail_choices.push_back(s);
	}
	getChoice(avail_choices, choice);
	switch(stoi(choice)) {
		case 1:
			return MainMenuOptions::ADD_NEW_TABLE;
		case 2:
			return MainMenuOptions::VIEW_TABLE;
		case 3:
			return MainMenuOptions::GET_STATISTIC;
		case 4:
			return MainMenuOptions::PERFORM_ARITHMETIC_OP;
		case 5:
			return MainMenuOptions::PERFORM_JOIN;
		case 6:
			return MainMenuOptions::QUIT;
		default:
			break;
	}
	return MainMenuOptions::INVALID;
}
