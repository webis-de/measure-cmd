# The Measure Python API

```py
from measure import Environment

environment = Environment("git.hash", "elapsed_time.wall_clock", ..., polling_ms=1000)  # defaults would be all measures, and some reasonable polling interval.

# Then measure with a context manager...
with environment.measure():
  # do something

# ...or a function decorator...
@environment.measure
def do_something():
  # do something
do_something()

# ...or manually...
environment.start_measuring()
# do something
environment.stop_measuring()

print("start git hash", environment.measurements["git.hash"][0])
print("end git hash", environment.measurements["git.hash"][-1])
print("elapsed time", environment.measurements["elapsed_time.wall_clock"])
```