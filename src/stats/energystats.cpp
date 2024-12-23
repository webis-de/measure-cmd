#include <stats/energystats.hpp>

using am::EnergyStats;

EnergyStats::EnergyStats() : tracker() {}

void EnergyStats::start() { tracker.start(); }
void EnergyStats::stop() {
	tracker.stop();
	tracker.calculate_energy();
}
void EnergyStats::getStats(std::map<std::string, std::string>& stats) { /** \todo implement **/ }