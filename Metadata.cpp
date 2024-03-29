#include "Metadata.h"

void Metadata::setColumn(const std::vector < std::shared_ptr<std::string> > &ptrs) {
	int row_id = 1;
	for(auto ptr: ptrs) {
		col_elements.push_back(meta_ele_type(row_id, ptr));
		row_id++;
	}
	
	/*
		Finds if the values are numeric -> so that we can perform arithmetic ops  
	*/
	if (col_elements.size() != 0) {
		for (auto col: col_elements) {
			if (col.str_ptr->size() != 0) {
				if (is_numeric) is_numeric &= is_string_numeric(col.str_ptr);
			} else {
				null_values++;
			}
		}
	}

	merge_sort(col_elements.begin(), col_elements.end(), comp<meta_ele_type>);

	if (is_numeric) {
		sum = 0;
		for(auto col: col_elements) {
			if (col.str_ptr->size() != 0) {
				sum += std::strtold(col.str_ptr->c_str(), NULL);
			}
		}
	}
}

long double Metadata::getMedian() {
	if (!is_numeric || containsOnlyNullValues()) return  std::numeric_limits<long double>::quiet_NaN();
	int sz = col_elements.size() - null_values;
	if ((sz%2) == 0) {
		long double val1 = std::stold(col_elements[null_values + sz/2].str_ptr->c_str(), NULL);
		long double val2 = std::stold(col_elements[null_values + sz/2 - 1].str_ptr->c_str(), NULL);
		return (val1+val2)/2.0;
	}
	return std::stold(col_elements[null_values + sz/2].str_ptr->c_str(), NULL);
}
long double Metadata::getMax() {
	if (!is_numeric || containsOnlyNullValues()) return  std::numeric_limits<long double>::quiet_NaN();
	return std::stold(col_elements[col_elements.size()-1].str_ptr->c_str(), NULL);
}

long double Metadata::getMin() {
	if (!is_numeric || containsOnlyNullValues()) return  std::numeric_limits<long double>::quiet_NaN();
	return std::stold(col_elements[null_values].str_ptr->c_str(), NULL);
}

long double Metadata::getAverage() {
	if (!is_numeric || containsOnlyNullValues()) return  std::numeric_limits<long double>::quiet_NaN();
	return (sum*1.0)/(col_elements.size() - null_values);
}

bool Metadata::meta_ele_type::operator < (const meta_ele_type &rhs) const {
	/* 
		Compares two rows of the same column
		If it's numeric, it compares their numeric value otherwise compares them lexicographically
	*/
	if (rhs.str_ptr->size() == 0) return false;
	if (str_ptr->size() == 0) return true;
	if (is_string_numeric(str_ptr) && is_string_numeric(rhs.str_ptr)) {
		double long val1 = strtold(str_ptr->c_str(), NULL);
		double long val2 = strtold(rhs.str_ptr->c_str(), NULL);
		return (val2 - val1) > LDBL_EPSILON;
	}
	int sz = std::min(str_ptr->size(), rhs.str_ptr->size());
	for (int i = 0; i < sz; i++) {
		if ((*str_ptr)[i] == (*(rhs.str_ptr))[i]) continue;
		return (*str_ptr)[i] < (*(rhs.str_ptr))[i]; 
	}
	
	return str_ptr->size() < rhs.str_ptr->size(); 
}

bool Metadata::meta_ele_type::operator > (const meta_ele_type& rhs) const { return rhs < *this; }
bool Metadata::meta_ele_type::operator <= (const meta_ele_type& rhs) const { return !(*this > rhs); }
bool Metadata::meta_ele_type::operator == (const meta_ele_type& rhs) const { return !(*this < rhs) && !(*this > rhs); }
