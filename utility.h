#pragma once
#include <vector>
#include <iterator>

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
