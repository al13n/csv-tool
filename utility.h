#pragma once
#include <vector>
#include <iterator>
#include <climits>
#include <string>
#include <cerrno>
#include <cstdlib>
#include <memory>

namespace {
	template <typename T>
	bool comp(const T &a, const T &b) {
		return a <= b;
	}

	template <class It, typename F>
	void merge_sort (It first, It second, F&& comp) {
		typedef typename std::iterator_traits<It>::value_type v_type;
		// Runs log n time, where n is the number of elements
		for (long int depth = 2; depth < ((second - first) << 1); depth <<= 1) {
			It _traverse = first;
			// Merge the half portions traverse->[1....depth/2] nexthalf->[depth/2+1 ... depth]
			while ((second - _traverse) > (depth >> 1)) {
				It _start = _traverse;
				It _nexthalf = _traverse;
				for (long int j = 0; j < (depth >> 1) && _nexthalf != second; j++) _nexthalf++;
				std::vector<v_type> merged;
				long int cnt1 = 0;
				long int cnt2 = 0;
				/* merge the two sorted portions based on comparator*/
				while (cnt1 < (depth >> 1) && cnt2 < (depth >> 1) && _nexthalf != second) {
					if (comp(*_traverse, *_nexthalf)) {
						merged.push_back(*_traverse);
						cnt1++;
						_traverse++;
					} else {
						merged.push_back(*_nexthalf);
						cnt2++;
						_nexthalf++;
					}
				}
				while (cnt1 < (depth >> 1) && _traverse != second) {
					merged.push_back(*_traverse);
					cnt1++;
					_traverse++;
				}
				while (cnt2 < (depth >> 1) && _nexthalf != second) {
					merged.push_back(*_nexthalf);
					cnt2++;
					_nexthalf++;
				}
				// Copy the sorted elements to original place
				for (auto cpy: merged) {
					*_start = cpy;
					_start++;
				}
				
				_traverse = _nexthalf;
			}
		}
		return ;
	}


	bool isPositiveInteger(std::string num) {
		if (num.size() == 0) return false;

		for (int i = 0; i < num.size(); i++) {
			if (!(i == 0 && num[i] == '+') && (num[i] < '0' && num[i] > '9')) return false;
		}

		unsigned int limit_check = 0;
		for (int i = 0; i < num.size(); i++) {
			if (limit_check > INT_MAX) return false;
			limit_check = limit_check*10 + (num[i] - '0');
		}
		return true;
	}

	int getPositiveInteger(std::string num) {
		int res = 0;
		for (int i = 0; i < num.size(); i++) {
			res = res*10 + (num[i] - '0');
		}
		return res;
	}
	
	bool is_string_numeric(std::shared_ptr<std::string> num) {
		// Checks if the given string is numeric
		if (num->size() == 0) return false;
		char *endptr;
		const char* str = num->c_str();
		long double val = std::strtold(str, &endptr);
		if (errno != 0 || *endptr != '\0' || endptr == str) return false;
		return true;
	}
}
