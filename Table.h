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
#include "Metadata.h"

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

	std::shared_ptr<Metadata> getColumnMetadata(int);
	
	void getStatistic(std::ostream &, int);
	
	void performArithmeticOp(std::ostream &, defs::select_cols &, defs::select_cols &, defs::ARITHMETIC_OP);

	void performSortAsc(std::ostream &, defs::select_cols &, defs::select_cols &);
	
	void performJoin(std::ostream &, std::shared_ptr<Table> &, defs::select_cols &, defs::select_cols &, defs::select_cols &, defs::select_cols &, defs::JOIN); 
	
private:
	unordered_map <int, std::vector< std::shared_ptr<std::string> > > rows;
	unordered_map <int, std::shared_ptr<Metadata> > col_metadata;
	int next_row_id, num_cols;
	bool header;
	int id;
	std::shared_ptr<std::string> filename_ptr;	
	bool createMetadata(int col_id);
};
