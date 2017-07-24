#include "Table.h"
#include <iostream>
#include <sstream>

Table::Table(std::istream &file_in, HasHeader h): num_rows{0}, header{true}, num_cols{0} {
	std::string line;

	setHeader(bool(h));
	
	while(file_in && !file_in.eof()) {
		addNewRow(file_in);
	}

	num_cols = (num_rows != 0 ? rows[num_rows-1].size() : 0); 
}

void Table::printHeader() {
	if (hasHeader()) printrow(0);
}

int Table::addNewRow(std::istream &file_in) {
	std::string col_data = "";
	std::vector<string> row;
	bool end_row = false;
	bool quoted = false;
	while (file_in && !file_in.eof()) {
		char c = file_in.get();
		if (quoted && c == '"') {
			col_data += c;
			if (file_in.peek() == '"') col_data += file_in.get();
			else quoted = false;
		}
		else if (!quoted && (c == ',' || c == '\n')) {
			row.push_back(col_data);
			if (c == '\n') break;
			col_data = "";
			quoted = false;
		}
		else {
			col_data += c;
			quoted = quoted | (c == '"');
		}
	}
	rows.insert(make_pair(num_rows, row));
	if (file_in.peek() == '\n') file_in.get();
	return num_rows++;
}

void Table::printrow(int row_id) {
	for(int j = 0; j < rows[row_id].size(); j++) {
		std::cout << rows[row_id][j] << " ";
	}
}
void Table::printrows() {
	for (int i = (hasHeader() ? 0 : 1); i < num_rows; i++) {
		printrow(i);
		std::cout << std::endl;
	}
}
