#include <measure/statformatter.hpp>

#include "./utils/overloaded.hpp"

static void recurseFormat(std::ostream& stream, const am::Stats& stats, std::string prefix) {
	if (stats.isLeaf()) {
		stream << '"' << stats.item() << '"';
	} else {
		stream << '{';
		std::string sep = "\n";
		for (const auto& [key, value] : stats.children()) {
			stream << sep << prefix << "  " << '"' << key << "\": ";
			recurseFormat(stream, value, prefix + "  ");
			sep = ",\n";
		}
		stream << '\n' << prefix << '}';
	}
}

JsonFormatter::JsonFormatter(const am::Stats& stats) : stats(stats) {}

void JsonFormatter::formatInto(std::ostream& stream) const noexcept { recurseFormat(stream, stats, ""); }