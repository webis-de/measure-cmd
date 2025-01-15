include(FetchContent)
enable_testing()

# Must be shared since it should interface with other languages as well
add_library(measureapi SHARED
	measure_api.cpp
	config.cpp
	jsonformatter.cpp
	logging.cpp
	simpleformatter.cpp
	stats/energystats.cpp
	stats/gitstats.cpp
	stats/gpustats.cpp
	stats/systemstats_linux.cpp
)
target_compile_features(measureapi PUBLIC cxx_std_20)
target_include_directories(measureapi PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)
set_property(TARGET measureapi PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(measurecmd)
target_sources(measurecmd PRIVATE
	main.cpp
)
target_compile_features(measurecmd PUBLIC cxx_std_20)
target_link_libraries(measurecmd measureapi)


set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


##########################################################################################
# Libraries
##########################################################################################
# spdlog for logging
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.14.1)
FetchContent_MakeAvailable(spdlog)
set_property(TARGET spdlog PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(measureapi PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# CLI11
FetchContent_Declare(cli11 GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git GIT_TAG v2.4.2)
FetchContent_MakeAvailable(cli11)
target_link_libraries(measurecmd CLI11::CLI11)

# CPPJoule
FetchContent_Declare(cppjoule GIT_REPOSITORY https://github.com/TheMrSheldon/CPPJoules.git GIT_TAG main)#GIT_TAG v1.0)
FetchContent_MakeAvailable(cppjoule)
set_property(TARGET CPP_Joules PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(measureapi PRIVATE CPP_Joules)

# LibGit
option(USE_HTTPS OFF)
option(BUILD_CLI OFF) # Do not build the Git CLI
FetchContent_Declare(libgit GIT_REPOSITORY https://github.com/libgit2/libgit2.git GIT_TAG v1.8.4)
FetchContent_MakeAvailable(libgit)
set_property(TARGET libgit2 PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET libgit2package PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(measureapi PUBLIC libgit2 libgit2package)

# NVML (We load the go-nvml bindings since there does not seem to be a repo with just the header)
FetchContent_Declare(nvml GIT_REPOSITORY https://github.com/NVIDIA/go-nvml.git GIT_TAG v0.12.4-0)
FetchContent_MakeAvailable(nvml)
target_include_directories(measureapi PRIVATE ${nvml_SOURCE_DIR}/gen/)
