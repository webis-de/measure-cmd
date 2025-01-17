#ifndef STATS_ENERGYSTATS_HPP
#define STATS_ENERGYSTATS_HPP

#include "provider.hpp"

#include <cppJoules.hpp>

namespace am {
	class EnergyStats final : public StatsProvider {
	private:
		EnergyTracker tracker;

	public:
		EnergyStats();

		void start() override;
		void stop() override;
		void getStats(Stats& stats) override;

		static constexpr const char* description = "Collects the energy consumption of various components.";
		static const char* version;
	};
} // namespace am

#endif