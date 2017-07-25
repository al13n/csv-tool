#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cfloat>
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
	
	class Metadata {
		int id;
		typedef typename std::pair<int, std::shared_ptr<std::string> > col_ele_type;
		std::vector < col_ele_type > col_elements;
		public:
			bool is_numeric;
			double long sum;

			Metadata(int _id): id{_id}, is_numeric{false}, sum{0} {}
			
			void setColumn(const std::vector < std::shared_ptr<std::string> > &ptrs) {
				int row_id = 1;
				for(auto ptr: ptrs) {
					col_elements.push_back(make_pair(row_id, ptr));
					row_id++;
				}
				
				if (col_elements.size() != 0) {
					is_numeric = is_string_numeric(*(col_elements[0].second));
				}
				
				if (is_numeric) {
					merge_sort(col_elements.begin(), col_elements.end(), comp_for_metadata<col_ele_type>);
					for(auto col: col_elements) {
						sum += strtold(col.second->c_str(), NULL);
					}
				}
			}
	};
	
	unordered_map <int, std::vector< std::shared_ptr<std::string> > > rows;
	unordered_map <int, std::shared_ptr<Metadata> > col_metadata;
	int next_row_id, num_cols;
	bool header;
	int id;
	
	bool createMetadata(int col_id) {
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
	}
	
public:
	Table() : next_row_id{0}, num_cols{0}, id{-1} {}

	Table(istream &, HasHeader);

	int addNewRow(istream &);	
	int getNumberColumns() { return num_cols; }
	
	bool hasHeader() { return header; }

	void setId(int &_id) { id = _id; }
	int getId() { return id; } 

	void printHeader(PrintWithId);
	void printrow(std::ostream &, int, const defs::select_cols &, PrintWithId);
	void printrows(std::ostream &, const defs::select_cols &);

	stat getStatistic(int &col_id, bool &flags) {
		stat ret;
		
		if (!next_row_id) return ret;
		
		if (createMetadata(col_id)) {
			col_id--;
			auto meta = col_metadata[col_id];
			if (!meta->is_numeric) {
				
			}
			
			if (flags & F_MEDIAN_) {
				long double median = stold(rows[meta.sortedOrder[next_row_id/2]][col_id]);
				if (next_row_id&1 == 0) {
					median = (median + stold(rows[meta.sortedOrder[next_row_id/2 - 1]][col_id]))/2.0;
				}
				ret.median = median;
			}
			
			if (flags & F_AVERAGE_) {
				ret.average = (meta->sum*1.0)/next_row_id;
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
/*
	vector<string> performOp(int &col_id_1, int &col_id_2, Operator op) {
	
	}
	*/

};
