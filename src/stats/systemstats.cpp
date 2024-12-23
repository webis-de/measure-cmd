#include <stats/systemstats.hpp>

#include <cinttypes>
#include <format>

using am::SystemStats;

/**
 * @brief Returns the total system RAM in Megabytes
 * 
 * @return The total system RAM in Megabytes.
 */
unsigned getSystemRAM();

void SystemStats::start() {}
void SystemStats::stop() {}
void SystemStats::getStats(std::map<std::string, std::string>& stats) {
	stats["system/ram"] = std::format("{} MB", getSystemRAM());
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#error "Unsupported System" /** \todo add support **/
#elif __APPLE__
#error "Unsupported System" /** \todo add support **/
#elif __linux__
#include <sys/sysinfo.h>

unsigned getSystemRAM() {
	struct sysinfo info;
	sysinfo(&info);
	return ((std::uint64_t)info.totalram * info.mem_unit) / 1000 / 1000;
}
#else
#error "Unsupported System"
#endif