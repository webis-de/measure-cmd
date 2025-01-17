#include "config.hpp"
#include "utils.hpp"

#include <measureapi.h>

#include <CLI/CLI.hpp>

#include <cstdlib>
#include <future>
#include <iostream>
#include <ranges>
#include <thread>

static void logCallback(mapiLogLevel level, const char* component, const char* message) {
	static constexpr spdlog::level::level_enum levels[] = {
			[mapiLogLevel::TRACE] = spdlog::level::level_enum::trace,
			[mapiLogLevel::DEBUG] = spdlog::level::level_enum::debug,
			[mapiLogLevel::INFO] = spdlog::level::level_enum::info,
			[mapiLogLevel::WARN] = spdlog::level::level_enum::warn,
			[mapiLogLevel::ERROR] = spdlog::level::level_enum::err,
			[mapiLogLevel::CRITICAL] = spdlog::level::level_enum::critical
	};
	am::getLogger(component)->log(levels[level], message);
}

using am::MeasureCmdArgs;

static void setupLoggerArgs(CLI::App& app, am::LoggerConf& conf) {
	app.add_flag(
			"-v,--verbose", conf.verbosity,
			"Sets the logger's verbosity. Passing it multiple times increases verbosity."
	);
	app.add_flag("-q,--quiet", conf.quiet, "Supresses all outputs");
}

static void runMeasureCmd(const MeasureCmdArgs& args) {
	// Initialization and setup
	am::setVerbosity(args.logConf.getVerbosity());
	auto logger = am::getLogger("measure");
	logger->info("Measuring command: {}", args.command);

	// Start measuring
	std::vector<const char*> providers;
	for (const auto& provider : args.statproviders)
		providers.emplace_back(provider.c_str());
	providers.emplace_back(nullptr);
	auto conf = args.measureConf;
	conf.provider = providers.data();

	auto handle = mapiStartMeasure(conf);

	// Run the command
	auto exitcode = std::system(args.command.c_str());

	// Stop measuring
	char* result;
	mapiStopMeasure(handle, &result);
	std::cout << result << std::endl;
	free(result);

	// Collect statistics and print them
	/*am::Stats stats{{"exit code", {std::to_string(exitcode)}}};
	for (auto& provider : providers)
		provider->getStats(stats);

	std::cout << "\n== RESULTS ==" << std::endl;
	std::cout << *args.constructFormatter(std::move(stats));*/
}

int main(int argc, char* argv[]) {
	mapiSetLogCallback(logCallback);
	CLI::App app("Measures runtime, energy, and many other metrics of a specifed command.");
	app.set_version_flag("-V,--version", buildVersionString());
	app.set_help_flag("-h,--help", "Prints this help message");

	MeasureCmdArgs measureArgs;
	setupLoggerArgs(app, measureArgs.logConf);
	app.add_option("command", measureArgs.command, "The command to measure resources for")->required();
	app.add_option("--format,-f", measureArgs.formatter, "Specified how the output should be formatted")
			->default_val("simple");
	app.add_option("--source,-s", measureArgs.statproviders, "The datasources to poll information from")
			->default_val(std::vector<std::string>{"git", "system", "energy"});
	app.add_flag("--monitor,!--no-monitor", measureArgs.measureConf.monitor)
			->description(
					"If set, monitors resource consumption continuously at the intervall specified in --poll-intervall."
			)
			->default_val(true);
	app.add_option("--poll-intervall", measureArgs.measureConf.pollIntervallMs)
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