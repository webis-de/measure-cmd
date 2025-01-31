#ifndef STATS_PROVIDER_HPP
#define STATS_PROVIDER_HPP

#include <measureapi.h>

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <variant>

struct mapiResult_st {
private:
	std::variant<std::string, std::map<std::string, mapiResult_st>> value;

public:
	mapiResult_st(const std::string& value) noexcept : value(value) {}
	mapiResult_st(const std::initializer_list<std::pair<const std::string, mapiResult_st>>& value) noexcept
			: value(value) {}
	mapiResult_st(std::string&& value) noexcept : value(value) {}
	mapiResult_st(std::initializer_list<std::pair<const std::string, mapiResult_st>>&& value) noexcept : value(value) {}

	bool isLeaf() const noexcept { return !std::holds_alternative<std::map<std::string, mapiResult_st>>(value); }
	const std::map<std::string, mapiResult_st>& children() const noexcept {
		assert(!isLeaf());
		return std::get<std::map<std::string, mapiResult_st>>(value);
	}
	const std::string& item() const noexcept {
		assert(isLeaf());
		return std::get<std::string>(value);
	}

	void insertChild(const std::string& name, mapiResult_st&& stats) {
		assert(!isLeaf());
		auto& map = std::get<std::map<std::string, mapiResult_st>>(value);
		map.insert_or_assign(name, stats);
	}

	static mapiResult_st merge(const mapiResult_st& left, const mapiResult_st& right) {
		assert(!left.isLeaf() && !right.isLeaf());
		mapiResult_st merged{};
		const auto& rc = right.children();
		const auto& lc = left.children();
		std::map<std::string, mapiResult_st>::const_iterator it;
		for (const auto& [key, value] : lc) {
			if ((it = rc.find(key)) != rc.end()) // Present in left and right
				merged.insertChild(key, merge(value, it->second));
			else // Present only in left
				merged.insertChild(key, mapiResult_st{value});
		}
		for (const auto& [key, value] : rc) {
			if (lc.find(key) == lc.end()) // Present only in right
				merged.insertChild(key, mapiResult_st{value});
		}
		return merged;
	}
};

namespace am {
	using Stats = mapiResult_st;

	class StatsProvider {
	public:
		virtual ~StatsProvider() = default;

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