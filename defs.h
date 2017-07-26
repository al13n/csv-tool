#pragma once

namespace defs {
	const int MAX_DISPLAY_COLUMNS = 15;
	const std::string null = "[NULL]";

	struct select_cols {
		bool ALL = false;
		bool print_null = false;
		std::vector<int> cols;
		
		void reset() {
			ALL = false;
			print_null = false;
			cols.clear();
		}
		
		select_cols() {
			reset();
		}
	};

	enum class ARITHMETIC_OP : char {
		PLUS = '+',
		MINUS = '-',
		DIVIDE = '/',
		MULTIPLY = '*'
	};
	
	enum class JOIN : int {
		INNER = 1,
		OUTER_LEFT = 2,
		OUTER_RIGHT = 3,
		OUTER_FULL = 4
	};
}
