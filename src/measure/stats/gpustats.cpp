#include <measure/stats/gpustats.hpp>

#include <nvml/nvml.h>

using am::GPUStats;

const char* GPUStats::version = "nvml v." NVML_API_VERSION_STR;

GPUStats::GPUStats() { /** \todo implement **/ }

void GPUStats::start() { /** \todo implement **/ }
void GPUStats::stop() { /** \todo implement **/ }
void GPUStats::getStats(Stats& stats) { /** \todo implement **/ }