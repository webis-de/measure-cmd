#ifndef UTILS_OVERLOADED_HPP
#define UTILS_OVERLOADED_HPP

namespace am::utils {
	/** Taken from https://en.cppreference.com/w/cpp/utility/variant/visit **/
	template <class... Ts>
	struct overloaded : Ts... {
		using Ts::operator()...;
	};
} // namespace am::utils

#endif