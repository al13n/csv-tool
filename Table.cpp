#include "Table.h"
#include <iostream>
#include <sstream>

Table::Table(std::istream &file_in, HasHeader h): next_row_id{0}, header{true}, num_cols{0}, id{-1} {
	std::string line;

	header = bool(h);
	if (!header) next_row_id++;
	
	while(file_in && !file_in.eof()) {
		addNewRow(file_in);
	}

	num_cols = ((header && next_row_id != 0) || (!header && next_row_id != 1)? rows[next_row_id-1].size() : 0); 
}

void Table::printHeader(PrintWithId id) {
	defs::select_cols sel;
	sel.ALL = true;
	if (hasHeader()) printrow(std::cout, 0, sel, id);
}

int Table::addNewRow(std::istream &file_in) {
	std::string col_data = "";
	std::vector< std::shared_ptr<std::string> >row;
	bool quoted = false;
	while (file_in && !file_in.eof()) {
		char c = file_in.get();
		if (quoted && c == '"') {
			if (file_in.peek() == '"') col_data += file_in.get();
			else quoted = false;
		}
		else if (!quoted && (c == ',' || c == '\n' || c == '\r')) {
			row.push_back(make_shared<std::string>(col_data));
			if (c == '\n' || c == '\r') break;
			col_data = "";
			quoted = false;
		}
		else {
			col_data += (c != '"' ? c : '\0');
			quoted = quoted | (c == '"');
		}
	}
	rows.insert(make_pair(next_row_id, row));
	while (file_in.peek() == '\n' || file_in.peek() == '\r') file_in.get();
	return next_row_id++;
}

void Table::printrow(std::ostream &os, const int row_id, const defs::select_cols &sel, PrintWithId id) {
	auto sel_cols = sel.cols;
	if (!sel.ALL) merge_sort(sel_cols.begin(), sel_cols.end(), comp<int>);
	int idx = 0;
	for(int j = 0; j < rows[row_id].size(); j++) {
		if (!sel.ALL && idx >= sel_cols.size()) break;
		if (sel.ALL || (j+1 == sel_cols[idx])) {
			if (bool(id)) std::cout << j+1 << ": ";
			os << "[" << *rows[row_id][j] << "]\t";
			idx++;
		}
	}
}
void Table::printrows(std::ostream &os, const defs::select_cols &sel) {
	for (int i = (hasHeader() ? 0 : 1); i < next_row_id; i++) {
		printrow(os, i, sel, PrintWithId::False);
		os << std::endl;
	}
}
