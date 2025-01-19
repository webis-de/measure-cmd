#include <measureapi.h>

#include "logging.hpp"
#include <measure/statformatter.hpp>
#include <measure/stats/provider.hpp>

#include <cassert>
#include <cstring>
#include <future>
#include <iostream>
#include <ranges>
#include <sstream>
#include <thread>
#include <vector>

size_t mapiGetDataProviders(mapiDataProvider* buf, size_t bufsize) {
	if (buf != nullptr) {
		for (const auto& [key, value] : am::providers | std::views::take(bufsize)) {
			*buf = {.name = key.c_str(), .description = value.description, .version = value.version};
			++buf;
		}
	}
	return am::providers.size();
}

struct mapiMeasure_st final {
	mapiConfig config;
	std::vector<std::unique_ptr<am::StatsProvider>> providers;
	std::thread monitorthread;
	std::promise<void> signal;

	explicit mapiMeasure_st(const mapiConfig& config) : config(config), providers() {
		for (auto provider = this->config.provider; *provider != nullptr; ++provider)
			providers.emplace_back(am::StatsProvider::constructFromName(*provider));
		this->config.provider = nullptr; // Set provider to nullptr to allow the caller to free that memory

		// Start measuring
		measureapi::log::info("measure", "Start Measuring");
		for (auto& provider : providers)
			provider->start();

		monitorthread = std::thread(mapiMeasure::monitorThread, this);
	}

	am::Stats stop() {
		signal.set_value();
		monitorthread.join();

		// Stop measuring
		for (auto& provider : providers | std::views::reverse)
			provider->stop();

		// Collect statistics and print them
		am::Stats stats{}; /** \todo ranges **/
		for (auto& provider : providers)
			stats = am::Stats::merge(stats, provider->getStats());
		/*std::ostringstream stream;
		stream << SimpleFormatter(std::move(stats));
		return std::move(stream.str());*/
		return stats;
	}

	static void monitorThread(mapiMeasure* self) {
		auto future = self->signal.get_future();
		auto intervall = std::chrono::milliseconds{self->config.pollIntervallMs};
		while (future.wait_for(intervall) != std::future_status::ready) {
			for (auto& provider : self->providers)
				provider->step();
		}
	}
};

mapiMeasure* mapiStartMeasure(mapiConfig config) { return new mapiMeasure{config}; }

mapiResult* mapiStopMeasure(mapiMeasure* measure) {
	/*auto str = measure->stop();
	size_t memsize = str.length() + 1;
	*result = static_cast<char*>(std::malloc(memsize)); // The caller has to free
	assert(result != nullptr);
	std::memcpy(static_cast<void*>(*result), static_cast<const void*>(str.c_str()), memsize);
	delete measure;*/
	auto result = measure->stop();
	delete measure;
	return (mapiResult*)new am::Stats(std::move(result));
}

bool mapiResultGetValue(const mapiResult* result, void const** value) {
	if (((am::Stats*)result)->isLeaf()) {
		*value = reinterpret_cast<const void*>(((am::Stats*)result)->item().c_str());
		return true;
	}
	return false;
}

size_t mapiResultGetChildren(const mapiResult* result, mapiResult const** buf, size_t bufsize) {
	if (((am::Stats*)result)->isLeaf())
		return 0;
	if (buf != nullptr)
		for (const auto& child : ((am::Stats*)result)->children() | std::views::take(bufsize))
			*buf = (mapiResult const*)&child.second;
	return ((am::Stats*)result)->children().size();
}

void mapiResultFree(mapiResult* result) { delete (am::Stats*)result; }