#include <measure/statformatter.hpp>

#include "./utils/overloaded.hpp"

static void recurseFormat(std::ostream& stream, const am::Stats& stats, std::string prefix) {
	for (auto&& [key, val] : stats) {
		std::visit(
				am::utils::overloaded{
						[&stream, key, prefix](const std::string& val) {
							stream << '[' << prefix << key << "] " << val << std::endl;
						},
						[&stream, key, prefix](const am::Stats& stats) {
							recurseFormat(stream, stats, prefix + key + "/");
						},
				},
				val
		);
	}
}

SimpleFormatter::SimpleFormatter(const am::Stats& stats) : stats(stats) {}

void SimpleFormatter::formatInto(std::ostream& stream) const noexcept { recurseFormat(stream, stats, ""); }