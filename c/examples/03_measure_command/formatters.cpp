#include "formatters.hpp"

/* SIMPLE FORMATTER */
static void simpleRecurseFormat(std::ostream& stream, const mapiResult* stats, const std::string& prefix) {
	const char* value;
	if (mapiResultGetValue(stats, reinterpret_cast<const void**>(&value))) {
		stream << '[' << prefix << "] " << value << std::endl;
	} else {
		size_t num = mapiResultGetEntries(stats, NULL, 0);
		mapiResultEntry buf[num];
		mapiResultGetEntries(stats, buf, num);
		for (const auto& [key, value] : buf)
			simpleRecurseFormat(stream, value, prefix + (prefix.empty() ? "" : "/") + key);
	}
}

void am::simpleFormatter(std::ostream& stream, const mapiResult* result) noexcept {
	simpleRecurseFormat(stream, result, "");
}

/* JSON FORMATTER */
static void jsonRecurseFormat(std::ostream& stream, const mapiResult* stats, const std::string& prefix) {
	const char* value;
	if (mapiResultGetValue(stats, reinterpret_cast<const void**>(&value))) {
		stream << '"' << value << '"';
	} else {
		stream << '{';
		std::string sep = "\n";
		size_t num = mapiResultGetEntries(stats, NULL, 0);
		mapiResultEntry buf[num];
		mapiResultGetEntries(stats, buf, num);
		for (const auto& [key, value] : buf) {
			stream << sep << prefix << "  " << '"' << key << "\": ";
			jsonRecurseFormat(stream, value, prefix + "  ");
			sep = ",\n";
		}
		stream << '\n' << prefix << '}';
	}
}

void am::jsonFormatter(std::ostream& stream, const mapiResult* result) noexcept {
	jsonRecurseFormat(stream, result, "");
}
