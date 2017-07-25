#pragma once
#include <vector>
#include <iterator>
#include <climits>
#include <string>
#include <cerrno>
#include <cstdlib>

namespace {
	template <typename T>
	bool comp_for_metadata(const T &p1, const T &p2) {
		double long val1 = strtold(p1.second->c_str(), NULL);
		double long val2 = strtold(p2.second->c_str(), NULL);
		return (val2 - val1) > LDBL_EPSILON;
	}
	
	template <typename T>
	bool comp(const T &a, const T &b) {
		return a < b;
	}

	template <class It, typename F>
	void merge_sort (It first, It second, F&& comp) {
		typedef typename std::iterator_traits<It>::value_type v_type;
		for (long int depth = 2; depth < ((second - first) << 1); depth <<= 1) {
			It _traverse = first;
			while ((second - _traverse) > (depth >> 1)) {
				It _start = _traverse;
				It _nexthalf = _traverse;
				for (long int j = 0; j < (depth >> 1) && _nexthalf != second; j++) _nexthalf++;
				std::vector<v_type> merged;
				long int cnt1 = 0;
				long int cnt2 = 0;
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
			if (num[i] < '0' && num[i] > '9') return false;
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
	
	bool is_string_numeric(std::string num) {
		char *end;
		long double val = std::strtold(num.c_str(), &end);
		if (errno != 0 || *end != '\0') return false;
		return true;
	}
}
