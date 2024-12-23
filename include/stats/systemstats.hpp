#ifndef STATS_SYSTEMSTATS_HPP
#define STATS_SYSTEMSTATS_HPP

#include "provider.hpp"

namespace am {
	class SystemStats final : public StatsProvider {
	private:
	public:
		void start() override;
		void stop() override;
		void getStats(std::map<std::string, std::string>& stats) override;
	};
} // namespace am

#endif