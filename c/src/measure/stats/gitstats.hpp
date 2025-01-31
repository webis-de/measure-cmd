#ifndef STATS_GITSTATS_HPP
#define STATS_GITSTATS_HPP

#include "provider.hpp"

struct git_repository;

namespace am {
	class GitStats final : public StatsProvider {
	private:
		git_repository* repo;

	public:
		GitStats();
		~GitStats();

		bool isRepository() const noexcept;

		void start() override;
		void stop() override;
		Stats getStats() override;

		static constexpr const char* description = "Collects git related metrics.";
		static const char* version;
	};
} // namespace am

#endif