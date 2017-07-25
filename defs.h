#pragma once

namespace defs {
	const int MAX_DISPLAY_COLUMNS = 15;

	const int F_MEDIAN_ = (1<<1);
	const int F_MAX_ = (1<<2);
	const int F_MIN_ = (1<<3);
	const int F_AVERAGE_ = (1<<4);

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
