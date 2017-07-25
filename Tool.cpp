#include <iostream>
#include <unordered_map>
#include <fstream>
#include <memory>
#include "Table.h"
#include "Options.h"
#include "utility.h"
#include "defs.h"
using namespace std;

unordered_map <std::shared_ptr<std::string>, int> files_id;
unordered_map <int, std::shared_ptr<Table>> tables;
int last_file_id = 0;
std::ofstream out;

void outputFileId(std::shared_ptr<std::string> filename_ptr) {
	if (files_id.find(filename_ptr) != files_id.end()) {
		std::cout << "Table ID: " << files_id[filename_ptr] << "\tFilename: " << *filename_ptr << std::endl; 
	}
}

bool listallTables() {
	if (tables.size() == 0) {
		std::cout << "No tables exist within the tool!\n";
		return false;
	}
	std::cout << "-----------------------------------------------------------\n";
	std::cout << "List of all existing tables:\n";
	std::cout << "Total number of tables: " << tables.size() << std::endl;
	for(auto f: files_id) {
		outputFileId(f.first);
	}
	std::cout << "-----------------------------------------------------------\n";
	return true;
}

std::ostream& getOutputStream() {
	std::cout << "Output result to file? (y/n) ";
	std::string choice = "";
	getYesNoChoice(choice);
	if (choice == "y") {
		string filename;
		std::cout << "Enter filename:\t";
		getline(cin, filename);
		out.open(filename, std::ios::out);
	}
	return (choice == "y" ? out : std::cout);
}

void getNewTableFromUser() {
	string filename;
	std::cout << "Enter filename:\t";
	getline(cin, filename);
		
	ifstream csvfile(filename);
	
	if(!csvfile.is_open()) {
		cout << "==>\tError: Failed to open " << filename << " ! Enter valid file" << endl;
		return;
	} else {
		++last_file_id;
		auto filename_ptr = make_shared<std::string>(filename);
		files_id.insert(make_pair(filename_ptr, last_file_id));
		
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
		tbl->setFilenamePtr(filename_ptr);
		outputFileId(filename_ptr);
		csvfile.close();
	}
	
	return;
}

defs::select_cols selectColumns (shared_ptr<Table> tbl, ChooseOne chooseOne) {
	if (tbl == nullptr) return defs::select_cols{};
	
	defs::select_cols current_select;
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
		if (!bool(chooseOne)) {
			std::cout << "==>\tEnter space-separated column number or * for all: (ex: 1 2 3 or *)\n"; 
		} else {
			std::cout << "==>\tEnter column number: (ex: 1)\n"; 
		}
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
		
		if((current_select.ALL || current_select.cols.size() != 1) && bool(chooseOne)) {
			invalid = true;
		}
		
		if (invalid) {
			std::cout << "==>\tError ! Invalid columns entered, please check and enter\n";
		}
	} while(invalid);

	return current_select;
}

shared_ptr<Table> selectTable() {
	if (!listallTables()) {
		return nullptr;
	}
	
	std::cout << "==>\t-----------------------------------------------------------\n";
	std::cout << "==>\tEnter table id: ";
	std::string choice;
	std::vector<std::string> avail_choices;
	for (auto id: tables) {
		char c = id.first + '0';
		string s = ""; s = s + c;
		avail_choices.push_back(s);
	}
	getChoice(avail_choices, choice); 
	std::cout << "==>\t-----------------------------------------------------------\n";
	return tables[getPositiveInteger(choice)];
}

void viewTable() {
	auto tbl = selectTable();
	if (tbl != nullptr) {
		defs::select_cols current_select = selectColumns(tbl, ChooseOne::False);
		std::ostream &os = getOutputStream();
		tbl->printrows(os, current_select);
	}
	return ;
}

void removeTable() {
	auto tbl = selectTable();
	if (tbl != nullptr) {
		int id = tbl->getId();
		files_id.erase(files_id.find(tbl->getFilenamePtr()));
		tables.erase(tables.find(id));
	}
	return ;
}

void getStatistic() {
	auto tbl = selectTable();
	if (tbl != nullptr) {
		cout << "Enter the column id(s) to get a statistic on: \n";
		defs::select_cols current_select = selectColumns(tbl, ChooseOne::False);
		std::ostream &os = getOutputStream();
		merge_sort(current_select.cols.begin(), current_select.cols.end(), comp<int>);
		int idx = 0;
		for (int i = 1; i <= tbl->getNumberColumns(); i++) {
			if (current_select.ALL || (current_select.cols[idx] == i)) {
				idx++;
				tbl->getStatistic(os, i);
			}
		}
	}
	return;
}

void performArithmeticOp() {
	auto tbl = selectTable();
	if (tbl != nullptr) {
		std::cout << "Select column 1:\n";
		defs::select_cols sel1 = selectColumns(tbl, ChooseOne::True);
		std::cout << "Operator (ex: + or - or / or *):";
		std::string choice;
		std::vector <std::string> avail_choices(4);
		avail_choices[0] += static_cast<char>(defs::ARITHMETIC_OP::PLUS); 
		avail_choices[1] += static_cast<char>(defs::ARITHMETIC_OP::MINUS); 
		avail_choices[2] += static_cast<char>(defs::ARITHMETIC_OP::MULTIPLY); 
		avail_choices[3] += static_cast<char>(defs::ARITHMETIC_OP::DIVIDE); 
		getChoice(avail_choices, choice);
		defs::ARITHMETIC_OP op = static_cast<defs::ARITHMETIC_OP> (choice[0]);	
		std::cout << "Select column 2:\n";
		defs::select_cols sel2 = selectColumns(tbl, ChooseOne::True);
		std::ostream &os = getOutputStream();
		
		//tbl->performArithmeticOp(os, sel1, sel2, op);
	}
}

void performJoin() {
	std::cout << "Enter table 1:\n";
	auto tbl1 = selectTable();
	if (tbl1 == nullptr) return ;
	std::cout << "Enter table 2:\n";
	auto tbl2 = selectTable();
	if (tbl2 == nullptr) return ;
	
	std::cout << "==>\tINFO: ON col1 == col2, where col1 is from table 1 and col2 is from table 2\n";
	std::cout << "Select column 1:\n";
	defs::select_cols sel1 = selectColumns(tbl1, ChooseOne::True);
	std::cout << "Select column 2:\n";
	defs::select_cols sel2 = selectColumns(tbl2, ChooseOne::True);
	
	std::cout << "JOIN TYPE (1: inner or 2: left outer or 3: right outer or 4: full outer): (ex: 1 or 2 or 3 or 4)";
	std::string choice;
	std::vector <std::string> avail_choices(4);
	avail_choices[0] += static_cast<int>(defs::JOIN::INNER);
	avail_choices[1] += static_cast<int>(defs::JOIN::OUTER_LEFT);
	avail_choices[2] += static_cast<int>(defs::JOIN::OUTER_RIGHT);
	avail_choices[3] += static_cast<int>(defs::JOIN::OUTER_FULL);
	getChoice(avail_choices, choice);
	defs::JOIN join_type = static_cast<defs::JOIN> (choice[0]);	
	
	std::ostream &os = getOutputStream();
}


void init() {
	if (out.is_open()) out.close();
}

int main() {
	
	ifstream csvfile;
	MainMenuOptions choice;

	do {
		init();
		std::cout << std::endl << std::endl;
		choice = showMainMenu();
		switch (choice) {
			case MainMenuOptions::ADD_NEW_TABLE:
				getNewTableFromUser();
				break;
			case MainMenuOptions::VIEW_TABLE:
				viewTable();
				break;
			case MainMenuOptions::GET_STATISTIC:
				getStatistic();
				break;
			case MainMenuOptions::PERFORM_ARITHMETIC_OP:
				performArithmeticOp();
				break;
			case MainMenuOptions::PERFORM_JOIN:
				performJoin();
				break;
			case MainMenuOptions::DELETE_TABLE:
				removeTable();
				break;
			case MainMenuOptions::QUIT:
				break;
			default:
				break;
		}
	} while(choice != MainMenuOptions::QUIT);
		
	return 0;
}
