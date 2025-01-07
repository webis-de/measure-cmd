#include <config.hpp>

#include <stats/energystats.hpp>
#include <stats/gitstats.hpp>
#include <stats/gpustats.hpp>
#include <stats/systemstats.hpp>

using am::MeasureCmdArgs;
using am::StatsProvider;

using FormatterConstructor = std::function<std::unique_ptr<StatFormatter>(const am::Stats&)>;
static const std::map<std::string, FormatterConstructor> formatters{
		{"simple", std::make_unique<SimpleFormatter, const am::Stats&>},
		{"json", std::make_unique<JsonFormatter, const am::Stats&>}
};

using ProviderConstructor = std::function<std::unique_ptr<am::StatsProvider>(void)>;
static const std::map<std::string, ProviderConstructor> providers{
		{"system", std::make_unique<am::SystemStats>},
		{"energy", std::make_unique<am::EnergyStats>},
		{"git", std::make_unique<am::GitStats>},
		{"gpu", std::make_unique<am::GPUStats>}
};

std::unique_ptr<StatFormatter> MeasureCmdArgs::constructFormatter(const am::Stats& stats) const {
	auto it = formatters.find(formatter);
	if (it == formatters.end()) {
		/** \todo handle gracefully **/
		abort();
	}
	return (it->second)(stats);
}

std::vector<std::unique_ptr<am::StatsProvider>> MeasureCmdArgs::constructProviders() const {
	auto logger = am::getLogger("measure");
	std::vector<std::unique_ptr<am::StatsProvider>> ret;
	for (const auto& p : statproviders) {
		auto it = providers.find(p);
		if (it == providers.end())
			logger->error("The stat provider '{}' could not be found and will be ignored", p);
		else
			ret.emplace_back((it->second)());
	}
	return ret;
}