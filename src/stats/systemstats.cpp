#include <stats/systemstats.hpp>

#include <cinttypes>
#include <format>

using std::chrono::steady_clock;

using am::SystemStats;

struct SysInfo {
	unsigned numCores;
	uint64_t totalRamMB;
};

struct Utilization {
	unsigned ramUsedKB;
};

SysInfo getSysInfo();

Utilization getUtilization();

void SystemStats::start() { starttime = steady_clock::now(); }
void SystemStats::stop() { stoptime = steady_clock::now(); }
void SystemStats::step() {
	auto reltime = std::chrono::duration_cast<std::chrono::microseconds>(steady_clock::now() - starttime).count();
	auto utilization = getUtilization();
	maxRAM = std::max(utilization.ramUsedKB, maxRAM);
	monitored.emplace_back(Entry{.timestamp = reltime, .usedRAM = utilization.ramUsedKB});
}

void SystemStats::getStats(Stats& stats) {
	auto info = getSysInfo();
	stats["system"] = Stat{Stats{
			{"num cores", Stat{std::to_string(info.numCores)}}, {"ram", Stat{std::format("{} MB", info.totalRamMB)}}
	}};
	stats["elapsed time"] = Stat{
			std::format("{} ms", std::chrono::duration_cast<std::chrono::milliseconds>(stoptime - starttime).count())
	};
	stats["resources"] = Stat{Stats{{"Max RAM used", {std::format("{} KB", maxRAM)}}}};
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#error "Unsupported System" /** \todo add support **/

#include <windows.h>

SysInfo getSysInfo() {
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return { .numCores = info.dwNumberOfProcessors, .totalRamMB = /** \todo **/ }
}

#elif __APPLE__
#error "Unsupported System" /** \todo add support **/
#elif __linux__
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <unistd.h>

SysInfo getSysInfo() {
	struct sysinfo info;
	sysinfo(&info);
	return {.numCores = static_cast<unsigned>(sysconf(_SC_NPROCESSORS_ONLN)),
			.totalRamMB = ((std::uint64_t)info.totalram * info.mem_unit) / 1000 / 1000};
}

Utilization getUtilization() {
	/** \todo this is currently wrong! **/
	struct rusage usageProc;
	/** \todo: handle errors (return value of getrusage) **/
	getrusage(RUSAGE_SELF, &usageProc);
	return {.ramUsedKB = static_cast<unsigned>(usageProc.ru_maxrss)};
}
#else
#error "Unsupported System"
#endif