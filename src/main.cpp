#include <config.hpp>

#include <cstdlib>
#include <iostream>
#include <ranges>
#include <thread>

using am::MeasureCmdArgs;
using am::setupLoggerArgs;

void runMeasureCmd(const MeasureCmdArgs& args) {
	am::setVerbosity(args.logConf.getVerbosity());
	auto logger = am::getLogger("measure");
	logger->info("Measuring command: {}", args.command);
	if (args.monitor)
		logger->debug("Monitoring is enabled, I will monitor once every {} ms", args.pollIntervallMs);

	auto providers = args.constructProviders();
	for (auto& provider : providers)
		provider->start();
	volatile bool running = true;
	std::thread pollthread{[&providers, &running, args] {
		while (args.monitor && running) {
			std::this_thread::sleep_for(std::chrono::milliseconds{args.pollIntervallMs});
			for (auto& provider : providers)
				provider->step();
		}
	}};
	auto exitcode = std::system(args.command.c_str());
	running = false;
	pollthread.join();
	for (auto& provider : providers | std::views::reverse)
		provider->stop();
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