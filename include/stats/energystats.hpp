#ifndef STATS_ENERGYSTATS_HPP
#define STATS_ENERGYSTATS_HPP

#include "provider.hpp"

#include <cppJoules.h>

namespace am {
	class EnergyStats final : public StatsProvider {
	private:
		EnergyTracker tracker;

	public:
		EnergyStats();

		void start() override;
		void stop() override;
		void getStats(Stats& stats) override;
	};
} // namespace am

#endif