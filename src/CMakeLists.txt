include(FetchContent)
enable_testing()

add_executable(measurecmd)
target_sources(measurecmd PRIVATE
	main.cpp
	config.cpp
	jsonformatter.cpp
	logging.cpp
	simpleformatter.cpp
	stats/energystats.cpp
	stats/gitstats.cpp
	stats/gpustats.cpp
	stats/systemstats.cpp
)
target_compile_features(measurecmd PUBLIC cxx_std_23)
target_include_directories(measurecmd PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)


set(BUILD_SHARED_LIBS OFF)


##########################################################################################
# Libraries
##########################################################################################
# spdlog for logging
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.14.1)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(measurecmd spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# CLI11
FetchContent_Declare(cli11 GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git GIT_TAG v2.4.2)
FetchContent_MakeAvailable(cli11)
target_link_libraries(measurecmd CLI11::CLI11)

# CPPJoule
FetchContent_Declare(cppjoule GIT_REPOSITORY https://github.com/TheMrSheldon/CPPJoules.git GIT_TAG main)#GIT_TAG v1.0)
FetchContent_MakeAvailable(cppjoule)
target_link_libraries(measurecmd CPP_Joules)

# LibGit
FetchContent_Declare(libgit GIT_REPOSITORY https://github.com/libgit2/libgit2.git GIT_TAG v1.8.4)
FetchContent_MakeAvailable(libgit)
target_link_libraries(measurecmd libgit2 libgit2package)

# NVML (We load the go-nvml bindings since there does not seem to be a repo with just the header)
FetchContent_Declare(nvml GIT_REPOSITORY https://github.com/NVIDIA/go-nvml.git GIT_TAG v0.12.4-0)
FetchContent_MakeAvailable(nvml)
target_include_directories(measurecmd PUBLIC ${nvml_SOURCE_DIR}/gen/)
