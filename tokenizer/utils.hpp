#pragma once

#include <cctype>

#define IS_FUNC(f) \
	inline bool f(char ch){ \
		return std::f(static_cast<unsigned char>(ch)); \
	} \
	using __let_this_macro_end_with_a_simicolon_##f = int

namespace cc0 {
	IS_FUNC(isprint);
	IS_FUNC(isspace);
	IS_FUNC(isblank);
	IS_FUNC(isalpha);
	IS_FUNC(isupper);
	IS_FUNC(islower);
	IS_FUNC(isdigit);
}