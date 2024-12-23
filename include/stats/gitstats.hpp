#ifndef STATS_GITSTATS_HPP
#define STATS_GITSTATE_HPP

#include "../logging.hpp"
#include "provider.hpp"

#include <filesystem>

struct git_repository;

namespace am {
	class GitStats final : public StatsProvider {
	private:
		LoggerPtr logger;
		git_repository* repo;

	public:
		GitStats();
		~GitStats();

		bool isRepository() const noexcept;

		void start() override;
		void stop() override;
		void getStats(std::map<std::string, std::string>& stats) override;
	};
} // namespace am

#endif