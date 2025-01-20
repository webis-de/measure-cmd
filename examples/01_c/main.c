#include <measureapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void logcallback(mapiLogLevel level, const char* component, const char* message) {
	static const char* levlToStr[] = {[TRACE] = "TRACE", [DEBUG] = "DEBUG", [INFO] = "INFO",
									  [WARN] = "WARN",	 [ERROR] = "ERROR", [CRITICAL] = "CRITICAL"};
	printf("[%s] [%s] %s\n", levlToStr[level], component, message);
}

void printResult(const mapiResult* result, const char* prefix) {
	const char* value;
	if (mapiResultGetValue(result, (const void**)&value)) {
		printf("[%s] %s\n", prefix ? prefix : "", value);
		fflush(stdout);
	} else {
		size_t num = mapiResultGetEntries(result, NULL, 0);
		mapiResultEntry* buf = calloc(sizeof(mapiResultEntry), num);
		mapiResultGetEntries(result, buf, num);
		for (size_t i = 0; i < num; ++i) {
			if (prefix) {
				size_t len = strlen(prefix) + strlen(buf[i].name) + 1 + 1;
				char* pre = malloc(len);
				snprintf(pre, len, "%s/%s", prefix, buf[i].name);
				printResult(buf[i].value, pre);
				free(pre);
			} else {
				printResult(buf[i].value, buf[i].name);
			}
		}
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

	mapiResult* result = mapiStopMeasure(measure);

	printResult(result, NULL);

	mapiResultFree(result);

	return 0;
}