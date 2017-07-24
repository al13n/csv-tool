#pragma once

namespace defs {
	const int MAX_DISPLAY_COLUMNS = 15;

	struct select_cols {
		bool ALL = false;
		std::vector<int> cols;
		
		void reset() {
			ALL = false;
			cols.clear();
		}
		
		select_cols() {
			reset();
		}
	};
}
