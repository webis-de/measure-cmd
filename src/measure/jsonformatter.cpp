#include <measure/statformatter.hpp>

#include "./utils/overloaded.hpp"

static void recurseFormat(std::ostream& stream, const am::Stats& stats, std::string prefix) {
	std::string sep = "\n";
	for (auto&& [key, val] : stats) {
		std::visit(
				am::utils::overloaded{
						[&](const std::string& val) {
							stream << sep << prefix << '"' << key << "\": \"" << val << "\"";
						},
						[&](const am::Stats& stats) {
							stream << sep << prefix << '"' << key << "\": {";
							recurseFormat(stream, stats, prefix + "  ");
							stream << '\n' << prefix << "}";
						},
				},
				val
		);
		sep = ",\n";
	}
}

JsonFormatter::JsonFormatter(const am::Stats& stats) : stats(stats) {}

void JsonFormatter::formatInto(std::ostream& stream) const noexcept {
	stream << "{";
	recurseFormat(stream, stats, "  ");
	stream << "\n}" << std::endl;
}