#include "logging.hpp"

static void nulllogger(mapiLogLevel, const char*, const char*) { return; }

mapiLogCallback measureapi::logCallback = nulllogger;

void mapiSetLogCallback(mapiLogCallback callback) { measureapi::logCallback = callback; }