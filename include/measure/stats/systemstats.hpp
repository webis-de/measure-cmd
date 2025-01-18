#ifndef STATS_SYSTEMSTATS_HPP
#define STATS_SYSTEMSTATS_HPP

#include "provider.hpp"

#include <chrono>
#include <vector>

namespace am {
	class SystemStats final : public StatsProvider {
	private:
		std::chrono::steady_clock::time_point starttime;
		std::chrono::steady_clock::time_point stoptime;

		unsigned maxRAM;

		struct Entry;
		std::vector<Entry> monitored;

	public:
		void start() override;
		void stop() override;
		void step() override;
		Stats getStats() override;

		static constexpr const char* description = "Collects system components and utilization metrics.";
		static const char* version;

	private:
		struct Entry {
			int64_t timestamp;
			unsigned usedRAM;
		};
	};
} // namespace am

#endif