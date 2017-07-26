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

class Metadata {
	public:	
		Metadata(int _id): id{_id}, is_numeric{true}, sum{0}, null_values{0} {}
		~Metadata() {
			col_elements.clear();
		}
		
		void setColumn(const std::vector < std::shared_ptr<std::string> > &);
	
		bool containsOnlyNullValues() { return null_values == col_elements.size(); }
			
		long double getMedian();
		long double getMax();
		long double getMin();
		long double getAverage();
	private:
		int id;
		
		class meta_ele_type {
		public:
			meta_ele_type(): row_id{-1} {}
			meta_ele_type(int _id, std::shared_ptr<std::string> _ptr) : row_id {_id}, str_ptr {_ptr} {}
			
			bool operator < (const meta_ele_type&) const;
			bool operator > (const meta_ele_type&) const;
			bool operator <= (const meta_ele_type&) const;
			bool operator == (const meta_ele_type&) const;
			
			// which row is this element from?
			int row_id;
			// which resource does this element point to?
			std::shared_ptr<std::string> str_ptr;
		};
	public:
		bool is_numeric;
		double long sum;
		// Holds the sorted metadata for the column
		std::vector < meta_ele_type > col_elements;
		// Number of null elements in this column
		int null_values;
	
};
