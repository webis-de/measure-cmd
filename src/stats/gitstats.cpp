#include <stats/gitstats.hpp>

#include <git2.h>

#include <iostream>

using am::GitStats;

GitStats::GitStats() : logger(getLogger("gitstats")), repo(nullptr) {
	git_libgit2_init();
	int error = git_repository_open(&repo, "./");
	if (error < 0) {
		const git_error* e = git_error_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		return;
	}
}
GitStats::~GitStats() { git_libgit2_shutdown(); }

bool GitStats::isRepository() const noexcept { return repo != nullptr; }

void GitStats::start() { logger->info("Is a Git Repository: {}", (isRepository() ? "Yes" : "No")); }
void GitStats::stop() { /* nothing to do */ }
void GitStats::getStats(std::map<std::string, std::string>& stats) {
	stats["git/isrepo"] = std::to_string(isRepository());
	if (isRepository()) {
		/** \todo more stats **/
	}
}