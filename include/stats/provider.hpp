#ifndef STATS_PROVIDER_HPP
#define STATS_PROVIDER_HPP

#include <map>
#include <string>

namespace am {
	class StatsProvider {
	public:
		virtual void start() = 0;
		virtual void stop() = 0;
		virtual void getStats(std::map<std::string, std::string>& stats) = 0;
	};
} // namespace am

#endif