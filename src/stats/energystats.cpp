#include <stats/energystats.hpp>

using am::EnergyStats;

EnergyStats::EnergyStats() : tracker() {}

void EnergyStats::start() { tracker.start(); }
void EnergyStats::stop() { tracker.stop(); }
void EnergyStats::getStats(Stats& stats) {
	auto results = tracker.calculate_energy().energy;
	Stats dict;
	for (auto& [device, result] : results)
		dict[device] = Stat{std::to_string(result)};
	stats["energy"] = {std::move(dict)};
}