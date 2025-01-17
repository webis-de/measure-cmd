import ctypes
import time

# Step 1: Load the shard object (YOUR PATH MAY BE DIFFERENT!)
stdlib = ctypes.CDLL(None)
libmeasureapi = ctypes.cdll.LoadLibrary("build/src/libmeasureapi.so")

# (Optional): Register logger
@ctypes.CFUNCTYPE(None, ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p)
def logCallback(level: ctypes.c_int, component: ctypes.c_char_p, message: ctypes.c_char_p) -> None:
    level_to_name = ["TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL"]
    print(f"[{component.decode('ascii')}] [{level_to_name[level]}] {message.decode('ascii')}")

libmeasureapi.mapiSetLogCallback(logCallback)

# Step 2: Start measuring
providers = [b"git", b"system", b"energy", b"gpu", None]
providers_native = (ctypes.c_char_p*len(providers))(*providers)
handle = libmeasureapi.mapiStartMeasure(providers_native, True)

# Step 3: Run your code
time.sleep(2)

# Step 4: Stop measuring and read out results
#  You can parse the result e.g., via Python's json library
result = ctypes.c_char_p()
libmeasureapi.mapiStopMeasure(handle, ctypes.pointer(result))
print(result.value.decode("ascii"))

# Step 5: Cleanup, delete the result from memory
stdlib.free(result)