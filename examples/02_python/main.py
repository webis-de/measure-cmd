import ctypes
import time

# Step 1: Load the shard object (YOUR PATH MAY BE DIFFERENT!)
stdlib = ctypes.CDLL(None)
libmeasureapi = ctypes.cdll.LoadLibrary("build/src/libmeasureapi.so")

# Step 2: Start measuring
providers = [b"git", b"system", b"energy", b"gpu", None]
providers_native = (ctypes.c_char_p*len(providers))(*providers)
handle = libmeasureapi.start_measure(providers_native, True)

# Step 3: Run your code
time.sleep(2)

# Step 4: Stop measuring and read out results
#  You can parse the result e.g., via Python's json library
result = ctypes.c_char_p()
libmeasureapi.stop_measure(handle, ctypes.pointer(result))
print(result.value.decode("ascii"))

# Step 5: Cleanup, delete the result from memory
stdlib.free(result)