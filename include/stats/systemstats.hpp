#ifndef STATS_SYSTEMSTATS_HPP
#define STATS_SYSTEMSTATS_HPP

#include "provider.hpp"

#include <chrono>

namespace am {
	class SystemStats final : public StatsProvider {
	private:
		std::chrono::steady_clock::time_point starttime;
		std::chrono::steady_clock::time_point stoptime;

	public:
		void start() override;
		void stop() override;
		void getStats(Stats& stats) override;
	};
} // namespace am

#endif