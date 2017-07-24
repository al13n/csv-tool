#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "utility.h"
#include "defs.h"

using namespace std;

enum class HasHeader: bool {
	False = false,
	True = true
};

enum class PrintWithId: bool {
	False = false,
	True = true
};

class Table {
private:
	// Map row id to Row
	unordered_map <int, vector<string>> rows;
	int num_rows, num_cols;
	bool header;
	int id;
public:
	Table() : num_rows{0}, num_cols{0}, id{-1} {}

	Table(istream &, HasHeader);

	int addNewRow(istream &);	
	int getNumberColumns() { return num_cols; }
	
	bool hasHeader() { return header; }
	void setHeader(bool flag) { header = flag; };

	void setId(int &_id) { id = _id; }
	int getId() { return id; } 

	void printHeader(PrintWithId);
	void printrow(std::ostream &, int, const defs::select_cols &, PrintWithId);
	void printrows(std::ostream &, const defs::select_cols &);

	/*
	stat * getStatistic(int &col_id, bool &flags) {
		stat *ret = nullptr;

		if (!num_rows) return ret;

		if (isColNumeric(col_id) && createMetadata(col_id)) {
			Metadata meta = metadata[col_id];
			if (flags & F_MEDIAN_) {
				long double median = stold(rows[meta.sortedOrder[num_rows/2]][col_id]);
				if (num_rows&1 == 0) {
					median = (median + stold(rows[meta.sortedOrder[num_rows/2 - 1]][col_id]))/2.0;
				}
				ret->median = median;
			}
			
			if (flags & F_AVERAGE_) {
				ret->average = (meta.sum*1.0)/num_rows;
			}
			
			if (flags & F_MAX_) {
				ret->max = meta.max;
			}
			
			if (flags & F_MIN_) {
				ret->min = meta.min;
			}
		}
		return ret;
	}


	vector<string> performOp(int &col_id_1, int &col_id_2, Operator op) {
	
	}
	*/

};
