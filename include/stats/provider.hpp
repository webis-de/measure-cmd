#ifndef STATS_PROVIDER_HPP
#define STATS_PROVIDER_HPP

#include <map>
#include <string>
#include <variant>

namespace am {
	struct Stat;
	using Stats = std::map<std::string, Stat>;
	struct Stat : public std::variant<std::string, Stats> {};

	class StatsProvider {
	public:
		virtual void start() = 0;
		virtual void stop() = 0;
		virtual void getStats(Stats& stats) = 0;
	};
} // namespace am

#endif