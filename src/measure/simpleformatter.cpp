#include <measure/statformatter.hpp>

#include "./utils/overloaded.hpp"

static void recurseFormat(std::ostream& stream, const am::Stats& stats, std::string prefix) {
	if (stats.isLeaf())
		stream << '[' << prefix << "] " << stats.item() << std::endl;
	else
		for (const auto& [key, value] : stats.children())
			recurseFormat(stream, value, prefix + (prefix.empty() ? "" : "/") + key);
}

SimpleFormatter::SimpleFormatter(const am::Stats& stats) : stats(stats) {}

void SimpleFormatter::formatInto(std::ostream& stream) const noexcept { recurseFormat(stream, stats, ""); }