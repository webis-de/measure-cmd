#include "energystats.hpp"

using am::EnergyStats;
using am::Stats;

const char* EnergyStats::version = nullptr;

EnergyStats::EnergyStats() : tracker() {}

void EnergyStats::start() { tracker.start(); }
void EnergyStats::stop() { tracker.stop(); }
Stats EnergyStats::getStats() {
	auto results = tracker.calculate_energy().energy;
	Stats stats{};
	for (auto& [device, result] : results)
		stats.insertChild(device, {std::to_string(result)});

	return {{"energy", stats}};
}