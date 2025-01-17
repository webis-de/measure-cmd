#include <measure/stats/provider.hpp>

#include "../../logging.hpp"

#include <measure/stats/energystats.hpp>
#include <measure/stats/gitstats.hpp>
#include <measure/stats/gpustats.hpp>
#include <measure/stats/systemstats.hpp>

using am::StatsProvider;

const std::map<std::string, am::ProviderEntry> am::providers{
		{"system", {std::make_unique<am::SystemStats>, am::SystemStats::version, am::SystemStats::description}},
		{"energy", {std::make_unique<am::EnergyStats>, am::EnergyStats::version, am::EnergyStats::description}},
		{"git", {std::make_unique<am::GitStats>, am::GitStats::version, am::GitStats::description}},
		{"gpu", {std::make_unique<am::GPUStats>, am::GPUStats::version, am::GPUStats::description}}
};

std::unique_ptr<StatsProvider> StatsProvider::constructFromName(const std::string& name) {
	auto it = providers.find(name);
	if (it == providers.end()) {
		measureapi::log::error("measure", "The stat provider '{}' could not be found and will be ignored", name);
		return nullptr;
	}
	return it->second.constructor();
}