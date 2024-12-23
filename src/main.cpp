#include <logging.hpp>
#include <stats/energystats.hpp>
#include <stats/gitstats.hpp>
#include <stats/systemstats.hpp>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <ranges>
#include <thread>
#include <vector>

struct LoggerConf final {
	bool quiet = false;
	/**
	 * @brief Sets the verbosity of the logger. Per default (verbosity=0) only critical, error and warnings are printed.
	 * If verbosity is...
	 *  - 1: info and above is printed
	 *  - 2: debug and above is printed
	 *  - 3+: trace and above is printed
	 */
	int verbosity = 0;

	am::Verbosity getVerbosity() const noexcept {
		if (quiet)
			return am::Verbosity::Off;
		auto verb = static_cast<int>(am::Verbosity::Warning) + verbosity;
		verb = std::min(verb, static_cast<int>(am::Verbosity::Trace));
		return static_cast<am::Verbosity>(verb);
	}
};

struct MeasureCmdArgs {
	LoggerConf logConf;
	std::string command;
};

void setupLoggerArgs(CLI::App& app, LoggerConf& conf) {
	app.add_flag(
			"-v,--verbose", conf.verbosity,
			"Sets the logger's verbosity. Passing it multiple times increases verbosity."
	);
	app.add_flag("-q,--quiet", conf.quiet, "Supresses all outputs");
}

void runMeasureCmd(const MeasureCmdArgs& args) {
	am::setVerbosity(args.logConf.getVerbosity());
	auto logger = am::getLogger("measure");
	logger->info("Measuring command: {}", args.command);
	std::vector<std::unique_ptr<am::StatsProvider>> providers;
	/* Initializer list does not work here, since it uses the copy-constructor :( */
	providers.push_back(std::make_unique<am::GitStats>());
	providers.push_back(std::make_unique<am::SystemStats>());
	providers.push_back(std::make_unique<am::EnergyStats>());
	for (auto& provider : providers)
		provider->start();
	auto exticode = std::system(args.command.c_str());
	for (auto& provider : providers | std::views::reverse)
		provider->stop();
	std::map<std::string, std::string> stats;
	for (auto& provider : providers)
		provider->getStats(stats);
	for (auto& [key, value] : stats)
		logger->info("[{}] {}", key, value);
}

int main(int argc, char* argv[]) {
	/** \todo add app description **/
	CLI::App app("TODO");
	auto versionString = std::format("TODO");
	app.set_version_flag("-V,--version", versionString);

	MeasureCmdArgs measureArgs;
	setupLoggerArgs(app, measureArgs.logConf);
	app.add_option("command", measureArgs.command, "The command to measure resources for")->required();

	app.callback([&measureArgs]() { runMeasureCmd(measureArgs); });

	CLI11_PARSE(app, argc, argv);
	return 0;
}