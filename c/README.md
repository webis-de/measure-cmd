# The Measure C API

```c
#include <measureapi.h>

int main(int argc, char* argv[]) {
    const char* provider[] = {"git", "system", "gpu", "energy", NULL};
    mapiConfig config = {.provider = provider, .monitor = true, .pollIntervallMs = 100};

    mapiMeasure* measure = mapiStartMeasure(config);
    /** Measure your code here **/

    mapiResult* result = mapiStopMeasure(measure);
    /** The collected data can be traversed here **/
    
    mapiResultFree(result);
}
```