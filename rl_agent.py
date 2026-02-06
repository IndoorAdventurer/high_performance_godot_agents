## Simulates the agent process: receives observations and sends actions

from multiprocessing import shared_memory, resource_tracker
import numpy as np

name = "godot_python_data"

# Getting existing shared memory and making sure python doesn't pretty much
# take ownership 😤
shm = shared_memory.SharedMemory(name)
resource_tracker.unregister("/" + name, "shared_memory")

# Interpret it as numpy array and printing:
buffer = np.ndarray((2, 64), dtype=np.int64, buffer=shm.buf)
print(buffer)

# Editing the data such that the other process sees the changes 🤓
buffer[:] = 0
buffer[::2] = 29
buffer[:, ::3] = 5

shm.close()