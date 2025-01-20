#if __linux__
#include "systemstats.hpp"

#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <cinttypes>

using std::chrono::steady_clock;

using am::Stats;
using am::SystemStats;

const char* SystemStats::version = nullptr;

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
	monitored.emplace_back(Entry{.timestamp = reltime, .usedRAM = utilization.ramUsedKB});
}

Stats SystemStats::getStats() {
	auto info = getSysInfo();
	rusage resUsage;
	/** \todo: handle errors (return value of getrusage) **/
	getrusage(RUSAGE_CHILDREN, &resUsage);

	return {
			{{"system",
			  {{"num cores", {std::to_string(info.numCores)}}, {"ram (MB)", {std::to_string(info.totalRamMB)}}}},
			 {"elapsed time",
			  {{"wallclock (ms)",
				{std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(stoptime - starttime).count())}},
			   {"system (ms)", {std::to_string(resUsage.ru_stime.tv_sec * 1000 + resUsage.ru_stime.tv_usec / 1000)}},
			   {"user (ms)", {std::to_string(resUsage.ru_utime.tv_sec * 1000 + resUsage.ru_utime.tv_usec / 1000)}}}},
			 {"resources", {{"Max RAM used (KB)", {std::to_string(static_cast<unsigned>(resUsage.ru_maxrss))}}}}}
	};
}

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
	getrusage(RUSAGE_CHILDREN, &usageProc);
	return {.ramUsedKB = static_cast<unsigned>(usageProc.ru_maxrss)};
}
#endif