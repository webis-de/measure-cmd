#include <config.hpp>

#include <CLI/CLI.hpp>

#include <cstdlib>
#include <future>
#include <iostream>
#include <ranges>
#include <thread>

#include <git2/version.h> /** \todo move into stats provider **/

using am::MeasureCmdArgs;

static void setupLoggerArgs(CLI::App& app, am::LoggerConf& conf) {
	app.add_flag(
			"-v,--verbose", conf.verbosity,
			"Sets the logger's verbosity. Passing it multiple times increases verbosity."
	);
	app.add_flag("-q,--quiet", conf.quiet, "Supresses all outputs");
}

static void runCommand(std::promise<int> exitcode, std::string command) {
	exitcode.set_value(std::system(command.c_str()));
}

static void runMeasureCmd(const MeasureCmdArgs& args) {
	// Initialization and setup
	am::setVerbosity(args.logConf.getVerbosity());
	auto logger = am::getLogger("measure");
	logger->info("Measuring command: {}", args.command);

	// Start measuring
	auto providers = args.constructProviders();
	for (auto& provider : providers)
		provider->start();

	// Run the command
	std::promise<int> promise;
	auto exitcodeFuture = promise.get_future();
	std::thread cmdthread(runCommand, std::move(promise), args.command);

	// Wait for the result (and continue monitoring if configured)
	if (args.monitor) {
		logger->debug("Monitoring is enabled, I will monitor once every {} ms", args.pollIntervallMs);
		auto intervall = std::chrono::milliseconds{args.pollIntervallMs};
		while (exitcodeFuture.wait_for(intervall) != std::future_status::ready) {
			for (auto& provider : providers)
				provider->step();
		}
	} else {
		exitcodeFuture.wait();
	}
	cmdthread.join();
	auto exitcode = exitcodeFuture.get();

	// Stop measuring
	for (auto& provider : providers | std::views::reverse)
		provider->stop();

	// Collect statistics and print them
	am::Stats stats{{"exit code", {std::to_string(exitcode)}}};
	for (auto& provider : providers)
		provider->getStats(stats);

	std::cout << "\n== RESULTS ==" << std::endl;
	std::cout << *args.constructFormatter(std::move(stats));
}

#if defined(__GNUC__)
#if defined(__clang__)
static constexpr const char* compiler = "clang " __VERSION__;
#else
static constexpr const char* compiler = "gcc " __VERSION__;
#endif
#elif defined(_MSC_VER)
static constexpr const char* compiler = "msvc " _MSC_FULL_VER;
#else
static constexpr const char* compiler = "unknown compiler";
#endif

int main(int argc, char* argv[]) {
	/** \todo add app description **/
	CLI::App app("TODO");
	auto versionString = std::format(
			"libgit v.{}\n{}\nBuilt with {} for C++ {}", LIBGIT2_VERSION, am::getVersionStr(), compiler, __cplusplus
	);
	app.set_version_flag("-V,--version", versionString);
	app.set_help_flag("-h,--help", "Prints this help message");

	MeasureCmdArgs measureArgs;
	setupLoggerArgs(app, measureArgs.logConf);
	app.add_option("command", measureArgs.command, "The command to measure resources for")->required();
	app.add_option("--format,-f", measureArgs.formatter, "Specified how the output should be formatted")
			->default_val("simple");
	app.add_option("--source,-s", measureArgs.statproviders, "The datasources to poll information from")
			->default_val(std::vector<std::string>{"git", "system", "energy"});
	app.add_flag("--monitor,!--no-monitor", measureArgs.monitor)
			->description(
					"If set, monitors resource consumption continuously at the intervall specified in --poll-intervall."
			)
			->default_val(true);
	app.add_option("--poll-intervall", measureArgs.pollIntervallMs)
			->description(
					"The intervall in milliseconds in which to poll for updated stats like energy consumption and RAM "
					"usage. Smaller intervalls allow for higher accuracy."
			)
			->default_val(100);
	app.add_flag("--pedantic", measureArgs.pedantic, "If set, measure will stop execution on errors")
			->default_val(false); /** \todo support pedantic **/

	app.callback([&measureArgs]() { runMeasureCmd(measureArgs); });

	CLI11_PARSE(app, argc, argv);
	return 0;
}