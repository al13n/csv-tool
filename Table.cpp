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
			if (c == '"') quoted = true;
			else col_data += c;
		}
	}
	rows.insert(make_pair(next_row_id, row));
	while (file_in.peek() == '\n' || file_in.peek() == '\r') file_in.get();
	return next_row_id++;
}

void Table::printrow(std::ostream &os, const int row_id, const defs::select_cols &sel, PrintWithId id) {
	auto sel_cols = sel.cols;
	if (!sel.ALL) {
		merge_sort(sel_cols.begin(), sel_cols.end(), comp<int>);
		for(int j = 0; j < sel_cols.size(); j++) {
			if (!sel.print_null && bool(id)) std::cout << "[id:" << sel_cols[j] << "] ";
			if (!sel.print_null && rows[row_id][sel_cols[j]-1]->size() != 0) os << *rows[row_id][sel_cols[j]-1] << "\t";
			else os << defs::null << "\t";
		}	
		return ;
	}

	for(int j = 0; j < rows[row_id].size(); j++) {
		if (!sel.print_null && bool(id)) std::cout << "[id:" << j+1 << "] ";
		if (!sel.print_null && rows[row_id][j]->size() != 0) os << *rows[row_id][j] << "\t";
		else os << defs::null << "\t";
	}
}
void Table::printrows(std::ostream &os, const defs::select_cols &sel) {
	for (int i = (hasHeader() ? 0 : 1); i < next_row_id; i++) {
		printrow(os, i, sel, PrintWithId::False);
		os << std::endl;
	}
}

bool Table::createMetadata(int col_id) {
	if (col_id == 0 && col_id > num_cols) return false;
	col_id--;
	if (col_metadata.find(col_id) != col_metadata.end()) return true;

	std::shared_ptr<Metadata> meta = make_shared<Metadata>(col_id);
	std::vector < std::shared_ptr<std::string> > ptrs;
	for (int i = 1; i < next_row_id; i++) {
		ptrs.push_back(rows[i][col_id]);
	}
	meta->setColumn(ptrs);
	col_metadata.insert(make_pair(col_id, meta));
	return true;
}

std::shared_ptr<Metadata> Table::getColumnMetadata(int col_id) {
	if (createMetadata(col_id)) {
		return col_metadata[col_id-1];
	}
	return nullptr;
}

void Table::getStatistic(std::ostream& os, int col_id) {
	if (createMetadata(col_id)) {
		os << "-----------------------------------------------------------\n";
		os << "Stats for column: "<< col_id << std::endl;
		os << "-----------------------------------------------------------\n";
		
		col_id--;
		auto meta = col_metadata[col_id];
		if (!meta->is_numeric || meta->containsOnlyNullValues()) {
			os << "==>\tErr: Column is not numeric!\n";
			os << "-----------------------------------------------------------\n";
			return ; 
		}
		
		os << "Median: " << meta->getMedian() << "\n";	
		os << "Min: " << meta->getMin() << "\n";	
		os << "Max: " << meta->getMax() << "\n";	
		os << "Average: " << meta->getAverage() << "\n";	
		os << "-----------------------------------------------------------\n";
	} else {
		os << "==>\tErr: Invalid input!\n";
		os << "-----------------------------------------------------------\n";
	}
	return;
}

void Table::performSortAsc(std::ostream &os, defs::select_cols &sel, defs::select_cols &pred) {
	int col_id = pred.cols[0];
	auto meta = getColumnMetadata(col_id);
	
	for(int i = 0; i < meta->col_elements.size(); i++) {
		printrow(os, meta->col_elements[i].row_id, sel, PrintWithId::False);
		os << std::endl;
	}
}

void Table::performArithmeticOp(std::ostream &os, defs::select_cols &sel1, defs::select_cols &sel2, defs::ARITHMETIC_OP op) {
	int col_id1 = sel1.cols[0];
	int col_id2 = sel2.cols[0];
	if (createMetadata(col_id1) && createMetadata(col_id2)) {
		col_id1--;
		col_id2--;
		auto meta1 = col_metadata[col_id1];
		auto meta2 = col_metadata[col_id2];
		if (!meta1->is_numeric || !meta2->is_numeric) {
			os << "==>\tErr: Column is not numeric!\n";
			os << "-----------------------------------------------------------\n";
			return ; 
		}
		
		for(int i = 1; i < next_row_id; i++) {
			auto c1 = rows[i][col_id1];
			auto c2 = rows[i][col_id2];
			
			if (!is_string_numeric(c1) || !is_string_numeric(c2)) {
				std::cout << defs::null << std::endl;
				continue;
			}
			
			double long val1 = strtold(c1->c_str(), NULL);
			double long val2 = strtold(c2->c_str(), NULL);
			switch(op) {
				case defs::ARITHMETIC_OP::PLUS :
					os << (val1 + val2) << std::endl;
					break;
				case defs::ARITHMETIC_OP::MINUS :
					os << (val1 - val2) << std::endl;
					break;
				case defs::ARITHMETIC_OP::MULTIPLY :
					os << (val1*val2) << std::endl;
					break;
				case defs::ARITHMETIC_OP::DIVIDE :
					if (val2 == 0) {
						os << std::numeric_limits<long double>::quiet_NaN() << std::endl;
					} else {
						os << (val1/val2) << std::endl;
					}
					break;
				default:
					os << "Err: Invalid op" << std::endl;
					return ;
			}	
		}
	}
	return ;
}

void Table::performJoin(std::ostream &os, std::shared_ptr<Table> &tbl, defs::select_cols &sel1, defs::select_cols &sel2, 
	defs::select_cols &pred_col1, defs::select_cols &pred_col2, defs::JOIN join_type) {
	auto meta1 = getColumnMetadata(pred_col1.cols[0]);
	auto meta2 = tbl->getColumnMetadata(pred_col2.cols[0]);
	
	int idx1 = meta1->null_values;
	int idx2 = meta2->null_values;
	std::vector<int> rows1;
	std::vector<int> rows2;

	while(idx1 < meta1->col_elements.size() && idx2 < meta2->col_elements.size()) {
		
		auto key1 = meta1->col_elements[idx1];
		auto key2 = meta2->col_elements[idx2];
		
		if (key1 == key2) {
			auto curr = key1;
			do {
				rows1.push_back(meta1->col_elements[idx1].row_id);
				idx1++;
				if (idx1 == meta1->col_elements.size()) break;
				curr = meta1->col_elements[idx1];
			} while(curr == key1);
			
			curr = key2;
			do {
				rows2.push_back(meta2->col_elements[idx2].row_id);
				idx2++;
				if (idx2 == meta2->col_elements.size()) break;
				curr = meta2->col_elements[idx2];
			} while(curr == key2);
			
			for(int i = 0; i < rows1.size(); i++) {
				for (int j = 0; j < rows2.size(); j++) {
					printrow(os, rows1[i], sel1, PrintWithId::False);
					tbl->printrow(os, rows2[j], sel2, PrintWithId::False);
					os << std::endl;
				}
			}
			
			rows1.clear();
			rows2.clear();
		}
		else if(key1 < key2) {
			if (join_type == defs::JOIN::OUTER_LEFT || join_type == defs::JOIN::OUTER_FULL) {
				printrow(os, meta1->col_elements[idx1].row_id, sel1, PrintWithId::False);
				int dummy_r = tbl->hasHeader() ? 0 : 1;
				sel2.print_null = true;
				tbl->printrow(os, dummy_r, sel2, PrintWithId::False);
				sel2.print_null = false;
				os << std::endl;
			}
			
			idx1++;
		} 
		else {
			if (join_type == defs::JOIN::OUTER_RIGHT || join_type == defs::JOIN::OUTER_FULL) {
				int dummy_r = hasHeader() ? 0 : 1;
				sel1.print_null = true;
				printrow(os, dummy_r, sel1, PrintWithId::False);
				sel1.print_null = false;
				tbl->printrow(os, meta2->col_elements[idx2].row_id, sel2, PrintWithId::False);
				os << std::endl;
			}
			idx2++;
		}	
	}

	if (join_type == defs::JOIN::OUTER_LEFT || join_type == defs::JOIN::OUTER_FULL) {
		for (int i = idx1; i < meta1->col_elements.size(); i++) {
			printrow(os, meta1->col_elements[i].row_id, sel1, PrintWithId::False);
			int dummy_r = tbl->hasHeader() ? 0 : 1;
			sel2.print_null = true;
			tbl->printrow(os, dummy_r, sel2, PrintWithId::False);
			sel2.print_null = false;
			os << std::endl;
		}
		
		for (int i = 0; i < meta1->null_values; i++) {
			printrow(os, meta1->col_elements[i].row_id, sel1, PrintWithId::False);
			int dummy_r = tbl->hasHeader() ? 0 : 1;
			sel2.print_null = true;
			tbl->printrow(os, dummy_r, sel2, PrintWithId::False);
			sel2.print_null = false;
			os << std::endl;
		}
	}

	if (join_type == defs::JOIN::OUTER_RIGHT || join_type == defs::JOIN::OUTER_FULL) {
		for (int i = idx2; i < meta2->col_elements.size(); i++) {
			int dummy_r = hasHeader() ? 0 : 1;
			sel1.print_null = true;
			printrow(os, dummy_r, sel1, PrintWithId::False);
			sel1.print_null = false;
			tbl->printrow(os, meta2->col_elements[i].row_id, sel2, PrintWithId::False);
			os << std::endl;
		}
		for (int i = 0; i < meta2->null_values; i++) {
			int dummy_r = hasHeader() ? 0 : 1;
			sel1.print_null = true;
			printrow(os, dummy_r, sel1, PrintWithId::False);
			sel1.print_null = false;
			tbl->printrow(os, meta2->col_elements[i].row_id, sel2, PrintWithId::False);
			os << std::endl;
		}
	}
	return ;
}

