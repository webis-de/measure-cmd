#ifndef STATS_PROVIDER_HPP
#define STATS_PROVIDER_HPP

#include <measureapi.h>

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>

namespace am {
	struct Stats {
	private:
		std::variant<std::string, std::map<std::string, Stats>> value;

	public:
		Stats(const std::string& value) noexcept : value(value) {}
		Stats(const std::initializer_list<std::pair<const std::string, Stats>>& value) noexcept : value(value) {}
		Stats(std::string&& value) noexcept : value(value) {}
		Stats(std::initializer_list<std::pair<const std::string, Stats>>&& value) noexcept : value(value) {}

		bool isLeaf() const noexcept { return !std::holds_alternative<std::map<std::string, Stats>>(value); }
		const std::map<std::string, Stats>& children() const noexcept {
			assert(!isLeaf());
			return std::get<std::map<std::string, Stats>>(value);
		}
		const std::string& item() const noexcept {
			assert(isLeaf());
			return std::get<std::string>(value);
		}

		void insertChild(const std::string& name, Stats&& stats) {
			assert(!isLeaf());
			auto& map = std::get<std::map<std::string, Stats>>(value);
			map.insert_or_assign(name, stats);
		}

		static Stats merge(const Stats& left, const Stats& right) {
			assert(!left.isLeaf() && !right.isLeaf());
			Stats merged{};
			const auto& rc = right.children();
			const auto& lc = left.children();
			std::map<std::string, Stats>::const_iterator it;
			for (const auto& [key, value] : lc) {
				if ((it = rc.find(key)) != rc.end()) // Present in left and right
					merged.insertChild(key, merge(value, it->second));
				else // Present only in left
					merged.insertChild(key, Stats{value});
			}
			for (const auto& [key, value] : rc) {
				if (lc.find(key) == lc.end()) // Present only in right
					merged.insertChild(key, Stats{value});
			}
			return merged;
		}
	};

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
		 * @returns the statistics that were measured
		 */
		virtual Stats getStats() = 0;

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