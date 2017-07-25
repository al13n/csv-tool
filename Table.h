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
#include <limits>
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
		int null_values;
		typedef typename std::pair<int, std::shared_ptr<std::string> > col_ele_type;
		std::vector < col_ele_type > col_elements;
		public:
			bool is_numeric;
			double long sum;

			Metadata(int _id): id{_id}, is_numeric{true}, sum{0}, null_values{0} {}
			~Metadata() {
				col_elements.clear();
			}
				
			void setColumn(const std::vector < std::shared_ptr<std::string> > &);
		
			bool containsOnlyNullValues() { return null_values == col_elements.size(); }
				
			long double getMedian() {
				if (containsOnlyNullValues()) return  std::numeric_limits<long double>::quiet_NaN();
				int sz = col_elements.size() - null_values;
				if (sz&1 == 0) {
					long double val1 = stold(col_elements[null_values + sz/2].second->c_str(), NULL);
					long double val2 = stold(col_elements[null_values + sz/2 - 1].second->c_str(), NULL);
					return (val1+val2)/2.0;
				}
				return stold(col_elements[null_values + sz/2].second->c_str(), NULL);
			}
			
			long double getMax() {
				if (containsOnlyNullValues()) return  std::numeric_limits<long double>::quiet_NaN();
				return stold(col_elements[col_elements.size()-1].second->c_str(), NULL);
			}
			
			long double getMin() {
				if (containsOnlyNullValues()) return  std::numeric_limits<long double>::quiet_NaN();
				return stold(col_elements[null_values].second->c_str(), NULL);
			}
			
			long double getAverage() {
				if (containsOnlyNullValues()) return  std::numeric_limits<long double>::quiet_NaN();
				return (sum*1.0)/(col_elements.size() - null_values);
			}
			
	};
	
	unordered_map <int, std::vector< std::shared_ptr<std::string> > > rows;
	unordered_map <int, std::shared_ptr<Metadata> > col_metadata;
	int next_row_id, num_cols;
	bool header;
	int id;
	std::shared_ptr<std::string> filename_ptr;	
	bool createMetadata(int col_id); 
	
public:
	Table() : next_row_id{0}, num_cols{0}, id{-1} {}
	Table(istream &, HasHeader);
	~Table() {
		col_metadata.clear();
		rows.clear();
	}
	
	int addNewRow(istream &);	
	int getNumberColumns() { return num_cols; }
	
	bool hasHeader() { return header; }

	void setId(int &_id) { id = _id; }
	int getId() { return id; }
	
	void setFilenamePtr(std::shared_ptr<std::string> f_ptr) { filename_ptr = f_ptr; }
	std::shared_ptr<std::string> getFilenamePtr() { return filename_ptr; }

	void printHeader(PrintWithId);
	void printrow(std::ostream &, int, const defs::select_cols &, PrintWithId);
	void printrows(std::ostream &, const defs::select_cols &);

	void getStatistic(std::ostream &, int);
/*
	vector<string> performOp(int &col_id_1, int &col_id_2, Operator op) {
	
	}
	*/

};
