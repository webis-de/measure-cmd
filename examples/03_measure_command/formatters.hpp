#ifndef STATFORMATTER_HPP
#define STATFORMATTER_HPP

#include <measureapi.h>

#include <functional>
#include <ostream>

namespace am {
	using ResultFormatter = std::function<void(std::ostream& stream, const mapiResult* result)>;

	extern void simpleFormatter(std::ostream& stream, const mapiResult* result) noexcept;
	extern void jsonFormatter(std::ostream& stream, const mapiResult* result) noexcept;
} // namespace am

#endif