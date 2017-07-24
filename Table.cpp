#include "Table.h"
#include <iostream>
#include <sstream>

Table::Table(std::istream &file_in, HasHeader h): num_rows{0}, header{true}, num_cols{0}, id{-1} {
	std::string line;

	setHeader(bool(h));
	
	while(file_in && !file_in.eof()) {
		addNewRow(file_in);
	}

	num_cols = (num_rows != 0 ? rows[num_rows-1].size() : 0); 
}

void Table::printHeader(PrintWithId id) {
	defs::select_cols sel;
	sel.ALL = true;
	if (hasHeader()) printrow(std::cout, 0, sel, id);
}

int Table::addNewRow(std::istream &file_in) {
	std::string col_data = "";
	std::vector<string> row;
	bool quoted = false;
	while (file_in && !file_in.eof()) {
		char c = file_in.get();
		if (quoted && c == '"') {
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
			col_data += (c != '"' ? c : '\0');
			quoted = quoted | (c == '"');
		}
	}
	rows.insert(make_pair(num_rows, row));
	if (file_in.peek() == '\n') file_in.get();
	return num_rows++;
}

void Table::printrow(std::ostream &os, const int row_id, const defs::select_cols &sel, PrintWithId id) {
	auto sel_cols = sel.cols;
	if (!sel.ALL) merge_sort(sel_cols.begin(), sel_cols.end(), comparator_for_int);
	int idx = 0;
	for(int j = 0; j < rows[row_id].size(); j++) {
		if (!sel.ALL && idx >= sel_cols.size()) break;
		if (sel.ALL || (j+1 == sel_cols[idx])) {
			if (bool(id)) std::cout << j+1 << ": ";
			os << rows[row_id][j] << " ";
			idx++;
		}
	}
}
void Table::printrows(std::ostream &os, const defs::select_cols &sel) {
	for (int i = (hasHeader() ? 0 : 1); i < num_rows; i++) {
		printrow(os, i, sel, PrintWithId::False);
		os << std::endl;
	}
}
