#include "gpustats.hpp"

#include "../../logging.hpp"
#include "../utils/sharedlib.hpp"

#include <nvml/nvml.h>

using namespace std::literals;

using am::GPUStats;
using am::Stats;

const char* GPUStats::version = "nvml v." NVML_API_VERSION_STR;

struct NVMLLib final : am::utils::SharedLib {
public:
	using INIT_V2 = nvmlReturn_t (*)(void);
	INIT_V2 init = load<INIT_V2>({"nvmlInit_v2"});

	using ERROR_STRING = const char* (*)(nvmlReturn_t result);
	ERROR_STRING errorString = load<ERROR_STRING>({"nvmlErrorString"});

	/** SYSTEM */
	using SYSTEM_GET_DRIVER_VERSION = nvmlReturn_t (*)(char* version, unsigned int length);
	SYSTEM_GET_DRIVER_VERSION systemGetDriverVersion = load<SYSTEM_GET_DRIVER_VERSION>({"nvmlSystemGetDriverVersion"});

	/** DEVICE **/
	using DEVICE_GET_COUNT = nvmlReturn_t (*)(unsigned int* deviceCount);
	DEVICE_GET_COUNT deviceGetCount = load<DEVICE_GET_COUNT>({"nvmlDeviceGetCount_v2"});
	using DEVICE_GET_HANDLE_BY_INDEX = nvmlReturn_t (*)(unsigned int index, nvmlDevice_t* device);
	DEVICE_GET_HANDLE_BY_INDEX deviceGetHandleByIndex =
			load<DEVICE_GET_HANDLE_BY_INDEX>({"nvmlDeviceGetHandleByIndex_v2"});
	using DEVICE_GET_ARCHITECTURE = nvmlReturn_t (*)(nvmlDevice_t device, nvmlDeviceArchitecture_t* arch);
	DEVICE_GET_ARCHITECTURE deviceGetArchitecture = load<DEVICE_GET_ARCHITECTURE>({"nvmlDeviceGetArchitecture"});
	using DEVICE_GET_NAME = nvmlReturn_t (*)(nvmlDevice_t device, char* name, unsigned int length);
	DEVICE_GET_NAME deviceGetName = load<DEVICE_GET_NAME>({"nvmlDeviceGetName"});
	using DEVICE_GET_MEMORY_INFO = nvmlReturn_t (*)(nvmlDevice_t device, nvmlMemory_t* memory);
	DEVICE_GET_MEMORY_INFO deviceGetMemoryInfo = load<DEVICE_GET_MEMORY_INFO>({"nvmlDeviceGetMemoryInfo"});

#if defined(__linux__)
	NVMLLib() : am::utils::SharedLib("libnvidia-ml.so.1") {}
#elif defined(__APPLE__)
/** \todo This should not be an error, the SharedLib instance should just not be "good" **/
#error "MacOS is not supported to fetch NVIDIA GPU information"
#elif defined(_WIN64)
/** \todo add support **/
#error "Support for windows needs to be added here"
#else
#error "Unsupported OS"
#endif
} nvml;

static const char* nvmlArchToStr(nvmlDeviceArchitecture_t arch) {
	switch (arch) {
	case NVML_DEVICE_ARCH_KEPLER:
		return "Kepler";
	case NVML_DEVICE_ARCH_MAXWELL:
		return "Maxwell";
	case NVML_DEVICE_ARCH_PASCAL:
		return "Pascal";
	case NVML_DEVICE_ARCH_VOLTA:
		return "Volta";
	case NVML_DEVICE_ARCH_TURING:
		return "Turing";
	case NVML_DEVICE_ARCH_AMPERE:
		return "Ampere";
	case NVML_DEVICE_ARCH_ADA:
		return "Ada";
	case NVML_DEVICE_ARCH_HOPPER:
		return "Hopper";
	}
	return "Unknown";
}

static bool initNVML() {
	if (!nvml.good())
		return false;
	switch (nvml.init()) {
	case NVML_SUCCESS:
		char buf[80];
		nvml.systemGetDriverVersion(buf, sizeof(buf) - 1);
		measureapi::log::info("gpustats", "NVML was loaded successfully with driver version {}", buf);
		return true;
	case NVML_ERROR_DRIVER_NOT_LOADED:
		measureapi::log::warn("gpustats", "No NVIDIA driver is running");
		break;
	case NVML_ERROR_NO_PERMISSION:
		measureapi::log::error("gpustats", "I don't have permission to talk to the driver");
		break;
	}
	return false;
}

GPUStats::GPUStats() : nvml({.supported = initNVML(), .devices = {}}) {
	if (!nvml.supported)
		return;
	unsigned int count;
	switch (::nvml.deviceGetCount(&count)) {
	case NVML_SUCCESS:
		measureapi::log::info("gpustats", "Found {} device(s):", count);
		for (unsigned i = 0u; i < count; ++i) {
			nvmlDevice_t device;
			switch (nvmlReturn_t ret; ret = ::nvml.deviceGetHandleByIndex(i, &device)) {
			case NVML_SUCCESS:
				nvmlDeviceArchitecture_t arch;
				::nvml.deviceGetArchitecture(device, &arch);
				char name[96];
				::nvml.deviceGetName(device, name, sizeof(name) - 1);
				measureapi::log::info("gpustats", "\t[{}] {} ({} Architecture)", i, name, nvmlArchToStr(arch));
				nvml.devices.emplace_back(device);
				break;
			default:
				measureapi::log::error(
						"gpustats", "\t[{}] fetching handle failed with error {}", i, ::nvml.errorString(ret)
				);
				break;
			}
		}
	}
}

void GPUStats::start() {
	if (!nvml.supported)
		return;
}
void GPUStats::stop() {
	if (!nvml.supported)
		return;
	/** \todo implement **/
}

#include <iostream>

void GPUStats::step() {
	if (!nvml.supported)
		return;
	nvmlMemory_t memory;
	/** \todo support multi-gpu **/
	for (auto device : nvml.devices) {
		if (nvmlReturn_t ret; (ret = ::nvml.deviceGetMemoryInfo(device, &memory)) == NVML_SUCCESS) {
			std::cout << "\r used/total (MB):\t" << memory.used / 1000 / 1000 << " / " << memory.total / 1000 / 1000
					  << std::flush;
			nvml.vramUsageTotal.addValue(memory.used / 1000 / 1000);
		} else {
			measureapi::log::critical("gpustats", "Could not fetch memory information: {}", ::nvml.errorString(ret));
			abort(); /** \todo how to handle? **/
		}
	}
}

Stats GPUStats::getStats() {
	if (nvml.supported) {
		std::string vramTotal;
		for (auto& device : nvml.devices) {
			nvmlMemory_t memory;
			if (nvmlReturn_t ret; (ret = ::nvml.deviceGetMemoryInfo(device, &memory)) == NVML_SUCCESS) {
				vramTotal += std::to_string(memory.total / 1000 / 1000) + ",";
			} else {
				vramTotal += "None,";
			}
		}

		return {{"gpu", {{"supported", "1"s}, {"VRAM (MB)", vramTotal}}},
				{"system", {{"Max VRAM Used (MB)", std::to_string(nvml.vramUsageTotal.maxValue())}}}};
		/** \todo implement **/
	} else {
		return {{"gpu", {{"supported", "0"s}}}};
	}
}