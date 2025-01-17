#include <measureapi.h>

#include <stdio.h>
#include <stdlib.h>

void logcallback(mapiLogLevel level, const char* component, const char* message) {
	static const char* levlToStr[] = {[TRACE] = "TRACE", [DEBUG] = "DEBUG", [INFO] = "INFO",
									  [WARN] = "WARN",	 [ERROR] = "ERROR", [CRITICAL] = "CRITICAL"};
	printf("[%s] [%s] %s\n", levlToStr[level], component, message);
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

	char* result;
	mapiStopMeasure(measure, &result);

	printf("%s\n", result);

	free(result);
	return 0;
}