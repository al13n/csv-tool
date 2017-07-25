#include "Table.h"

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
			if (bool(id)) std::cout << "[id:" << j+1 << "]: ";
			os << *rows[row_id][j] << "\t";
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

bool Table::createMetadata(int col_id) {
	if (col_id > num_cols) return false;
	col_id--;
	if (col_metadata.find(col_id) != col_metadata.end()) return true;

	auto meta = make_shared<Metadata>(col_id);
	std::vector < std::shared_ptr<std::string> > ptrs;
	for (int i = 1; i < next_row_id; i++) {
		ptrs.push_back(rows[i][col_id]);
	}
	meta->setColumn(ptrs);
	col_metadata.insert(make_pair(col_id, meta));
	return true;
}

void Table::Metadata::setColumn(const std::vector < std::shared_ptr<std::string> > &ptrs) {
	int row_id = 1;
	for(auto ptr: ptrs) {
		col_elements.push_back(make_pair(row_id, ptr));
		row_id++;
	}
	
	if (col_elements.size() != 0) {
		for (auto col: col_elements) {
			if (col.second->size() != 0) {
				is_numeric &= is_string_numeric(col.second);
			} else {
				null_values++;
			}
		}
	}

	if (containsOnlyNullValues()) is_numeric = false;
	
	if (is_numeric) {
		merge_sort(col_elements.begin(), col_elements.end(), comp_for_metadata<col_ele_type>);
		sum = 0;
		for(auto col: col_elements) {
			if (col.second->size() != 0) {
				sum += strtold(col.second->c_str(), NULL);
			}
		}
	}
}

void Table::getStatistic(std::ostream& os, int col_id) {
	if (next_row_id == 0 || (!header && next_row_id == 1)) return ;
	
	if (createMetadata(col_id)) {
		os << "-----------------------------------------------------------\n";
		os << "Stats for column: "<< col_id << std::endl;
		os << "-----------------------------------------------------------\n";
		
		col_id--;
		auto meta = col_metadata[col_id];
		if (!meta->is_numeric || meta->containsOnlyNullValues()) {
			os << "Err: Column is not numeric!\n";
			os << "-----------------------------------------------------------\n";
			return ; 
		}
		
		os << "Median: " << meta->getMedian() << "\n";	
		os << "Min: " << meta->getMin() << "\n";	
		os << "Max: " << meta->getMax() << "\n";	
		os << "Average: " << meta->getAverage() << "\n";	
		os << "-----------------------------------------------------------\n";
	}
	return;
}

