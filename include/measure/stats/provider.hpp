#ifndef STATS_PROVIDER_HPP
#define STATS_PROVIDER_HPP

#include <measureapi.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>

namespace am {
	struct Stat;
	using Stats = std::map<std::string, Stat>;
	struct Stat : public std::variant<std::string, Stats> {};

	class StatsProvider {
	public:
		/**
		 * @brief Start is called once at the very beginning of collecting statistics and shortly before the command is
		 * run.
		 */
		virtual void start() = 0;
		/**
		 * @brief Stop is called once at the end of collecting statistics and shortly after the command is run.
		 */
		virtual void stop() = 0;
		/**
		 * @brief Step may be called multiple times during the execution of the command at some configured intervall (or
		 * even not at all).
		 */
		virtual void step() {}
		/**
		 * @brief Append the statistics from this provider to the given stats. This is called once after
		 * StatsProvider::stop().
		 * 
		 * @param stats The stats to append to.
		 */
		virtual void getStats(Stats& stats) = 0;

		static std::unique_ptr<StatsProvider> constructFromName(const std::string& name);
	};

	using ProviderConstructor = std::function<std::unique_ptr<StatsProvider>(void)>;
	struct ProviderEntry final {
		ProviderConstructor constructor;
		const char* version;
		const char* description;
	};
	extern const std::map<std::string, ProviderEntry> providers;
} // namespace am

#endif