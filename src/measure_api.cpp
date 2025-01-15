#include <measure_api.h>

#include <config.hpp>

#include <future>
#include <iostream>
#include <ranges>
#include <thread>

struct measure::Measure final {
	am::MeasureCmdArgs args;
	std::vector<std::unique_ptr<am::StatsProvider>> providers;
	std::thread monitorthread;
	size_t pollIntervallMs;
	std::promise<void> signal;

	explicit Measure(const am::MeasureCmdArgs& _args)
			: args(_args), providers(args.constructProviders()), pollIntervallMs(args.pollIntervallMs) {
		// Initialization and setup
		am::setVerbosity(args.logConf.getVerbosity());
		auto logger = am::getLogger("measure");
		logger->info("Start Measuring");

		// Start measuring
		for (auto& provider : providers)
			provider->start();

		monitorthread = std::thread(Measure::monitorThread, this);
	}

	std::string stop() {
		signal.set_value();
		monitorthread.join();

		// Stop measuring
		for (auto& provider : providers | std::views::reverse)
			provider->stop();

		// Collect statistics and print them
		am::Stats stats;
		for (auto& provider : providers)
			provider->getStats(stats);
		std::ostringstream stream;
		stream << *args.constructFormatter(std::move(stats));
		return std::move(stream.str());
	}

	static void monitorThread(Measure* self) {
		auto future = self->signal.get_future();
		auto intervall = std::chrono::milliseconds{self->pollIntervallMs};
		while (future.wait_for(intervall) != std::future_status::ready) {
			for (auto& provider : self->providers)
				provider->step();
		}
	}
};

measure::Measure* measure::start_measure(const char** sources, measure::_Bool monitor) {
	std::vector<std::string> providers;
	for (auto source = sources; *source != nullptr; ++source)
		providers.emplace_back(*source);
	am::MeasureCmdArgs args{.formatter = "json", .statproviders = providers, .monitor = monitor};
	return new measure::Measure{args};
}

void measure::stop_measure(measure::Measure* measure, char** result) {
	auto str = measure->stop();
	size_t memsize = str.length() + 1;
	*result = static_cast<char*>(std::malloc(memsize)); // The caller has to free
	assert(result != nullptr);
	std::memcpy(static_cast<void*>(*result), static_cast<const void*>(str.c_str()), memsize);
	delete measure;
}