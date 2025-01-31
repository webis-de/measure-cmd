#include <measure/config.hpp>

#include "../logging.hpp"

#include <measure/stats/energystats.hpp>
#include <measure/stats/gitstats.hpp>
#include <measure/stats/gpustats.hpp>
#include <measure/stats/systemstats.hpp>

using am::MeasureCmdArgs;
using am::StatsProvider;

using FormatterConstructor = std::function<std::unique_ptr<StatFormatter>(const am::Stats&)>;
static const std::map<std::string, FormatterConstructor> formatters{
		{"simple", std::make_unique<SimpleFormatter, const am::Stats&>},
		{"json", std::make_unique<JsonFormatter, const am::Stats&>}
};

std::unique_ptr<StatFormatter> MeasureCmdArgs::constructFormatter(const am::Stats& stats) const {
	auto it = formatters.find(formatter);
	if (it == formatters.end()) {
		/** \todo handle gracefully **/
		abort();
	}
	return (it->second)(stats);
}