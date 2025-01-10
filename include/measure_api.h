/**
 * @brief Contains measure's external C-API
 */

#ifdef __cplusplus
namespace measure {
	extern "C" {
	using _Bool = bool;
#endif

	struct Measure;

	struct Measure* start_measure(const char** sources, _Bool monitor);
	void stop_measure(struct Measure* measure, char** result);

#ifdef __cplusplus
	}
}
#endif