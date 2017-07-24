#include <iostream>
#include <unordered_map>
#include <fstream>
#include <memory>
#include "Table.h"
#include "Options.h"
#include "utility.h"
#include "defs.h"
using namespace std;

unordered_map <string, int> files_id;
unordered_map <int, shared_ptr<Table>> tables;
int last_file_id = 0;
defs::select_cols current_select;
std::ofstream out;

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

std::ostream& getOutputStream() {
	std::cout << "Output result to file? (y/n) ";
	std::string choice = "";
	getYesNoChoice(choice);
	if (choice == "y") {
		if (out.is_open()) out.close();
		string filename;
		std::cout << "Enter filename:\t";
		getline(cin, filename);
		out.open(filename, std::ios::app);
	}
	return (choice == "y" ? out : std::cout);
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
		getYesNoChoice(choice);
		HasHeader h = HasHeader::False;
		if (choice == "y") {
			h = HasHeader::True;
		}
		
		auto tbl = make_shared<Table>(csvfile, h);
		tables.insert(make_pair(last_file_id, tbl));
		tbl->setId(last_file_id);
		outputFileId(filename);				
		csvfile.close();
	}
	
	return;
}

void selectColumns (shared_ptr<Table> tbl) {
	bool invalid = false;
	do {
		invalid = false;
		current_select.reset();
		std::cout << "==>\t-----------------------------------------------------------\n";
		std::cout << "==>\tSelect columns: " << std::endl;
		if (tbl->hasHeader() && tbl->getNumberColumns() < defs::MAX_DISPLAY_COLUMNS) {
			std::cout << "==>\t";
			tbl->printHeader(PrintWithId::True);
			std::cout << std::endl;	
		} else {
			std::cout << "==>\tNumber of columns in table: " << tbl->getNumberColumns() << std::endl;
		}
		std::cout << "==>\tEnter space-separated column number or * for all: (ex: 1 2 3 or *)\n"; 
		std::cout << "==>\t-----------------------------------------------------------\n";
		std::cout << "==>\t";
		std::string choice;
		getline(std::cin, choice);
		stringstream ss(choice);
		std::string col;
		while(ss >> col) {
			if (col == "*") {
				current_select.ALL = true;
				break;
			} else {
				if (isPositiveInteger(col) && getPositiveInteger(col) <= tbl->getNumberColumns()) {
					current_select.cols.push_back(getPositiveInteger(col));
				} else {
					invalid = true;
					break;
				}
			}
		}
		
		if (invalid) {
			std::cout << "Error ! Invalid columns entered, please check and enter\n";
		}
	} while(invalid);	
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
	selectColumns(tbl);
	std::ostream &os = getOutputStream();
	tbl->printrows(os, current_select);	
}

void init() {
	current_select.reset();
}

int main() {
	
	ifstream csvfile;
	MainMenuOptions choice;

	do {
		init();
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
