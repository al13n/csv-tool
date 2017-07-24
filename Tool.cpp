#include <iostream>
#include <unordered_map>
#include <fstream>
#include <memory>
#include "Table.h"
#include "Options.h"
using namespace std;

unordered_map <string, int> files_id;
unordered_map <int, shared_ptr<Table>> tables;
int last_file_id = 0;

void outputFileId(string filename) {
	if (files_id.find(filename) != files_id.end()) {
		cout << "Filename: " << filename << "\tTable ID: " << files_id[filename] << endl; 
	}
}

void listallTables() {
	cout << "-----------------------------------------------------------\n";
	cout << "List of all existing tables:\n";
	for(auto f: files_id) {
		outputFileId(f.first);
	}
	cout << "-----------------------------------------------------------\n";
}

void getNewTableFromUser() {
	string filename;
	std::cout << "Enter filename:\t";
	getline(cin, filename);
		
	if (files_id.find(filename) != files_id.end()) {
		cout << "Table already exists [ID: " << files_id[filename] << "]\n";
		return;
	}
	
	ifstream csvfile(filename);
	
	if(!csvfile.is_open()) {
		cout << "Error: Failed to open " << filename << " ! Enter valid file" << endl;
		return;
	} else {
		++last_file_id;
		files_id.insert(make_pair(filename, last_file_id));
		
		std::cout << "Does this table contain a header?(y/n) ";
		std::string choice;
		std::vector<std::string> avail_choices {"y", "Y", "n", "N"};
		getChoice(avail_choices, choice);
		HasHeader h = HasHeader::False;
		if ((choice[0] | (1<<5)) == 'y') {
			h = HasHeader::True;
		}
		
		auto tbl = make_shared<Table>(csvfile, h);
		tables.insert(make_pair(last_file_id, tbl));
		outputFileId(filename);				
		csvfile.close();
	}
	
	return;
}

void viewTable() {
	listallTables();	
	std::cout << "Enter table id: ";
	std::string choice;
	std::vector<std::string> avail_choices;
	for (auto id: tables) {
		char c = id.first + '0';
		string s = ""; s = s + c;
		avail_choices.push_back(s);
	}
	getChoice(avail_choices, choice); 
	auto tbl = tables[stoi(choice)];
	std::cout << "Select columns: " << std::endl;
	if (tbl->hasHeader()) {
		tbl->printHeader();
		cout << endl;	
	} else {
		cout << "Number of columns in table: " << tbl->getNumberColumns() << endl;
	}
}

int main() {
	
	ifstream csvfile;
	MainMenuOptions choice;

	do {
		choice = showMainMenu();
		switch (choice) {
			case MainMenuOptions::ADD_NEW_TABLE:
				getNewTableFromUser();
				break;
			case MainMenuOptions::VIEW_TABLE:
				viewTable();
				break;
			case MainMenuOptions::GET_STATISTIC:
				break;
			case MainMenuOptions::PERFORM_ARITHMETIC_OP:
				break;
			case MainMenuOptions::PERFORM_JOIN:
				break;
			case MainMenuOptions::QUIT:
				break;
			default:
				break;
		}
	} while(choice != MainMenuOptions::QUIT);
		
	return 0;
}
