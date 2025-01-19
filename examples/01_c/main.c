#include <measureapi.h>

#include <stdio.h>
#include <stdlib.h>

void logcallback(mapiLogLevel level, const char* component, const char* message) {
	static const char* levlToStr[] = {[TRACE] = "TRACE", [DEBUG] = "DEBUG", [INFO] = "INFO",
									  [WARN] = "WARN",	 [ERROR] = "ERROR", [CRITICAL] = "CRITICAL"};
	printf("[%s] [%s] %s\n", levlToStr[level], component, message);
}

void printResult(const mapiResult* result) {
	const char* value;
	if (mapiResultGetValue(result, (const void**)&value)) {
		printf("%s", value);
		fflush(stdout);
	} else {
		size_t num = mapiResultGetChildren(result, NULL, 0);
		const mapiResult** buf = calloc(sizeof(mapiResult*), num);
		mapiResultGetChildren(result, buf, num);
		for (size_t i = 0; i < num; ++i)
			printResult(buf[i]);
	}
}

int main(int argc, char* argv[]) {
	mapiSetLogCallback(logcallback);

	const char* provider[] = {"git", "system", "gpu", "energy", NULL};
	mapiConfig config = {.provider = provider, .monitor = true, .pollIntervallMs = 1000};

	mapiMeasure* measure = mapiStartMeasure(config);

	{
		char* data = calloc(24 * 1000 * 1000, 1); // allocate 24 MB
		data[0] = 1;							  // Access the data so it is not optimized away
		free(data);
	}

	//char* result;
	//mapiStopMeasure(measure, &result);

	//printf("%s\n", result);

	//free(result);

	mapiResult* result = mapiStopMeasure(measure);

	printResult(result);

	mapiResultFree(result);

	return 0;
}