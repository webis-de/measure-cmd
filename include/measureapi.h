/**
 * @brief Contains measure's external C API
 */

#ifndef MEASUREAPI_H
#define MEASUREAPI_H

#include <stdbool.h>
#include <stddef.h>

#if defined _WINDOWS
#if !defined MEASUREAPI_STATIC_IMPORT
#if defined MEASUREAPI_LIB_EXPORT
#define MA_EXPORT __declspec(dllexport)
#else
#define MA_EXPORT __declspec(dllimport)
#endif
#else
#define MA_EXPORT
#endif
#else
#define MA_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Contains information about a data provider.
 * @details Data providers are modules tasked with collecting associated metrics about the program.
 */
typedef struct mapiDataProvider_st {
	const char* name;		 /**< The human legible name of the data provider **/
	const char* description; /**< The humand legible description of the data provider **/
	const char* version; /**< A textual representation of the provider's version. May contain linefeeds. May be NULL **/
} mapiDataProvider;

/**
 * @brief Populates the given buffer with all available providers (or at most \p bufsize if \p buf is not large enugh)
 * and returns the total number of available providers.
 * @details If \p buf is \c NULL , \p bufsize is ignored and only the number of available providers are returned. A
 * typical call may look as follows:
 * ```c
 * size_t num = mapiGetDataProviders(NULL, 0);
 * struct mapiDataProvider* buf = (struct mapiDataProvider*)calloc(num, sizeof(struct mapiDataProvider));
 * mapiGetDataProviders(buf, num);
 * // ...
 * free(buf);
 * ```
 * 
 * @param providers 
 * @param bufsize 
 * @return The number of available data providers.
 */
MA_EXPORT size_t mapiGetDataProviders(mapiDataProvider* buf, size_t bufsize);

/**
 * @brief Represents different levels of verbosity.
 * @details The enum values are sorted by verbosity. That is, for example, if logs should only be logged \c level
 * \c INFO or higher, then `level >= INFO` can be used to check what should be logged.
 */
typedef enum mapiLogLevel_enum { TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL } mapiLogLevel;

/**
 * @brief Points to a function that takes in a log level, component, and message which should be logged.
 * @details The log callback can handle the call as it likes (including ignoring it for example because logging is set
 * to a lower verbosity or turned of). The \p component may be used to identify, where the log is comming from.
 */
typedef void (*mapiLogCallback)(mapiLogLevel level, const char* component, const char* message);

/**
 * @brief Set a callback that should be used for all future logs.
 * @details The default logger does not do anything.
 * 
 * @param callback Points to a function that takes in a log level, component, and message which should be logged. 
 */
MA_EXPORT void mapiSetLogCallback(mapiLogCallback callback);

/**
 * @brief Configures 
 */
typedef struct mapiConfig_st {
	/**
	 * @brief A NULL-terminated list of provider names that should be used to collect metrics.
	 * 
	 * @see mapiDataProvider::name
	 * @see mapiGetDataProviders(mapiDataProvider, size_t)
	 */
	const char** provider;
	/**
	 * @brief Set to \c true to enable a monitoring thread used to collect data during the measurement. Most metrics
	 * require this to be turned on.
	 */
	bool monitor;
	/**
	 * @brief This sets the intervall at which the monitoring thread polls data if monitoring is turned on.
	 * 
	 * @see mapiConfig_st::monitor
	 */
	size_t pollIntervallMs;
} mapiConfig;

/**
 * @brief Holds a handle to an ongoing measurement task.
 */
typedef struct mapiMeasure mapiMeasure;

/**
 * @brief Initializes the providers set in the configuration and starts measuring.
 * 
 * @param config the configuration to use to measure
 * @return a handle to the running measurement.
 * @see mapiStopMeasure(mapiMeasure*, char**)
 */
MA_EXPORT mapiMeasure* mapiStartMeasure(mapiConfig config);

/**
 * @brief Stops the measurement and deinitializes the data providers.
 * @details This function **must** be called **exactly once** for each measurement job.
 * 
 * @param measure The handle of the running measurement that should be stopped.
 * @param result A pointer to write the result into. Must be freed by the caller. 
 */
MA_EXPORT void mapiStopMeasure(mapiMeasure* measure, char** result);
#ifdef __cplusplus
}
#endif

#endif