## Simulates the environment process: sends observations and receives actions

from multiprocessing import shared_memory
import numpy as np
import time


data = np.arange(128).reshape((2,64))
shm = shared_memory.SharedMemory("godot_python_data", True, data.nbytes)
buf = np.ndarray(data.shape, data.dtype, shm.buf)
buf[:] = data[:]

print(buf)

for _ in range(10):
    time.sleep(1)
    print(".", end="", flush=True)
print("")

shm.close()
shm.unlink()