## Simulates the agent process: receives observations and sends actions

from multiprocessing import shared_memory, resource_tracker
import numpy as np

name = "godot_python_data"

shm = shared_memory.SharedMemory(name)
resource_tracker.unregister("/" + name, "shared_memory")

buffer = np.ndarray((2, 64), dtype=np.int64, buffer=shm.buf)

print(buffer)

shm.close()