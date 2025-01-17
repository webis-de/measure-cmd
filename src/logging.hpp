#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <measureapi.h>

#include <format>
#include <string>

namespace measureapi {
	extern mapiLogCallback logCallback;

	namespace log {

		template <mapiLogLevel level, class... Args>
		void log(std::string component, std::format_string<Args...> fmt, Args&&... args) {
			const auto msg = std::format(fmt, std::forward<Args>(args)...);
			logCallback(level, component.c_str(), msg.c_str());
		}

		template <typename... Args>
		void trace(std::string component, std::format_string<Args...> fmt, Args&&... args) {
			log<mapiLogLevel::TRACE>(component, fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		void debug(std::string component, std::format_string<Args...> fmt, Args&&... args) {
			log<mapiLogLevel::DEBUG>(component, fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		void info(std::string component, std::format_string<Args...> fmt, Args&&... args) {
			log<mapiLogLevel::INFO>(component, fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		void warn(std::string component, std::format_string<Args...> fmt, Args&&... args) {
			log<mapiLogLevel::WARN>(component, fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		void error(std::string component, std::format_string<Args...> fmt, Args&&... args) {
			log<mapiLogLevel::ERROR>(component, fmt, std::forward<Args>(args)...);
		}
		template <typename... Args>
		void critical(std::string component, std::format_string<Args...> fmt, Args&&... args) {
			log<mapiLogLevel::CRITICAL>(component, fmt, std::forward<Args>(args)...);
		}
	} // namespace log
} // namespace measureapi

#endif