#include <stats/gitstats.hpp>

#include <git2.h>

#include <iostream>
#include <string>

using am::GitStats;

static std::string getLastCommitHash(git_repository* repo) {
	git_revwalk* walk;
	git_oid id;
	git_commit* commit;
	int err;
	if (err = git_reference_name_to_id(&id, repo, "HEAD")) {
		return ""; /** \todo log the error **/
	}
	char buf[GIT_OID_SHA1_HEXSIZE + 1];
	git_oid_tostr(buf, sizeof(buf), &id);
	return std::string{std::begin(buf), std::end(buf)};
}

static std::string getShortname(git_repository* repo) {
	git_reference* head;
	int err;
	if (err = git_repository_head(&head, repo)) {
		return ""; /** \todo log the error **/
	}
	std::string hash = git_reference_shorthand(head);
	git_reference_free(head);
	return hash;
}

static std::string getRemoteOrigin(git_repository* repo) {
	git_remote* remote;
	int err;
	if (err = git_remote_lookup(&remote, repo, "origin")) {
		return ""; /** \todo log the warning **/
	}
	std::string url = git_remote_url(remote);
	git_remote_free(remote);
	return url;
}

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
void GitStats::getStats(Stats& stats) {
	if (isRepository()) {
		stats["git"] = Stat{
				Stats{{"isrepo", {"1"}},
					  {"tag", {getShortname(repo)}},
					  {"last commit", {getLastCommitHash(repo)}},
					  {"remote", {Stats{{"origin", {getRemoteOrigin(repo)}}}}}}
		};
	} else {
		stats["git"] = Stat{Stats{{"isrepo", Stat{"0"}}}};
	}
}