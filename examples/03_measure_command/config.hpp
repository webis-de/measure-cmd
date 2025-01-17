#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "logging.hpp"

#include <measureapi.h>

#include <memory>
#include <string>
#include <vector>

namespace am {
	struct LoggerConf final {
		bool quiet = false;
		/**
         * @brief Sets the verbosity of the logger. Per default (verbosity=0) only critical, error and warnings are printed.
         * If verbosity is...
         *  - 1: info and above is printed
         *  - 2: debug and above is printed
         *  - 3+: trace and above is printed
         */
		int verbosity = 0;

		am::Verbosity getVerbosity() const noexcept {
			if (quiet)
				return am::Verbosity::Off;
			auto verb = static_cast<int>(am::Verbosity::Warning) + verbosity;
			verb = std::min(verb, static_cast<int>(am::Verbosity::Trace));
			return static_cast<am::Verbosity>(verb);
		}
	};

	struct MeasureCmdArgs final {
		LoggerConf logConf;
		std::string command;   /**< The command that should be measured **/
		std::string formatter; /**< The identifier specifying the formatter to use for the output **/
		std::vector<std::string> statproviders;
		mapiConfig measureConf;
		bool pedantic;
	};
} // namespace am

#endif