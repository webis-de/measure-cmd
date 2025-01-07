#include <config.hpp>

#include <cstdlib>
#include <future>
#include <iostream>
#include <ranges>
#include <thread>

using am::MeasureCmdArgs;
using am::setupLoggerArgs;

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

int main(int argc, char* argv[]) {
	/** \todo add app description **/
	CLI::App app("TODO");
	auto versionString = std::format("TODO");
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
			->description("If set, monitors resource consumption continuously at the intervall "
						  "specified in --poll-intervall.")
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