#include <measure/stats/gpustats.hpp>

#include "../../logging.hpp"

#include <nvml/nvml.h>

using am::GPUStats;
using am::Stats;

const char* GPUStats::version = "nvml v." NVML_API_VERSION_STR;

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
	switch (nvmlInit_v2()) {
	case NVML_SUCCESS:
		char buf[80];
		nvmlSystemGetDriverVersion(buf, sizeof(buf) - 1);
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
	switch (nvmlDeviceGetCount_v2(&count)) {
	case NVML_SUCCESS:
		measureapi::log::info("gpustats", "Found {} device(s):", count);
		for (unsigned i = 0u; i < count; ++i) {
			nvmlDevice_t device;
			switch (nvmlReturn_t ret; ret = nvmlDeviceGetHandleByIndex_v2(i, &device)) {
			case NVML_SUCCESS:
				nvmlDeviceArchitecture_t arch;
				nvmlDeviceGetArchitecture(device, &arch);
				char name[96];
				nvmlDeviceGetName(device, name, sizeof(name) - 1);
				measureapi::log::info("gpustats", "\t[{}] {} ({} Architecture)", i, name, nvmlArchToStr(arch));
				nvml.devices.emplace_back(device);
				break;
			default:
				measureapi::log::error(
						"gpustats", "\t[{}] fetching handle failed with error {}", i, nvmlErrorString(ret)
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
	for (auto device : nvml.devices) {
		if (nvmlReturn_t ret; (ret = nvmlDeviceGetMemoryInfo(device, &memory)) == NVML_SUCCESS) {
			std::cout << "\r used/total (MB):\t" << memory.used / 1000 / 1000 << " / " << memory.total / 1000 / 1000
					  << std::flush;
			nvml.vramUsageTotal.addValue(memory.used / 1000 / 1000);
		} else {
			measureapi::log::critical("gpustats", "Could not fetch memory information: {}", nvmlErrorString(ret));
			abort(); /** \todo how to handle? **/
		}
	}
}

Stats GPUStats::getStats() {
	if (nvml.supported) {
		return {{"gpu", {{"supported", {"1"}}}},
				{"system", {{"Max VRAM Used (MB)", {std::to_string(nvml.vramUsageTotal.maxValue())}}}}};
		/** \todo implement **/
	} else {
		return {{"gpu", {{"supported", {"0"}}}}};
	}
}