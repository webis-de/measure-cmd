#include <measure/stats/gitstats.hpp>

#include "../../logging.hpp"

#include <git2.h>

#include <format>
#include <iostream>
#include <string>

using am::GitStats;
using am::Stats;

const char* GitStats::version = "libgit v." LIBGIT2_VERSION;

static std::string getLastCommitHash(git_repository* repo) {
	git_oid id;
	int err;
	if (err = git_reference_name_to_id(&id, repo, "HEAD")) {
		return ""; /** \todo log the error **/
	}
	char buf[GIT_OID_SHA1_HEXSIZE + 1];
	git_oid_tostr(buf, sizeof(buf), &id);
	return std::string{buf};
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

GitStats::GitStats() : repo(nullptr) {
	git_libgit2_init();
	int error = git_repository_open_ext(&repo, "./", 0, nullptr);
	if (error < 0) {
		const git_error* e = git_error_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		return;
	}
}
GitStats::~GitStats() { git_libgit2_shutdown(); }

bool GitStats::isRepository() const noexcept { return repo != nullptr; }

void GitStats::start() {
	measureapi::log::info("gitstats", "Is a Git Repository: {}", (isRepository() ? "Yes" : "No"));
}
void GitStats::stop() { /* nothing to do */ }
Stats GitStats::getStats() {
	if (isRepository()) {
		return {
				{"git",
				 {{"isrepo", {"1"}},
				  {"tag", {getShortname(repo)}},
				  {"last commit", {getLastCommitHash(repo)}},
				  {"remote", {{{"origin", {getRemoteOrigin(repo)}}}}}}}
		};
	} else {
		return {{"git", {{"isrepo", {"0"}}}}};
	}
}