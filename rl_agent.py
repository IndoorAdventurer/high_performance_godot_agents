## Simulates the agent process: receives observations and sends actions

import posix_ipc
import mmap
import numpy as np

shared_mem_name = "/godot_python_data"
agent_sem_name = "/godot_python_agent_sem"
env_sem_name = "/godot_python_env_sem"

# Creating semaphores:
agent_sem = posix_ipc.Semaphore(agent_sem_name, posix_ipc.O_CREX)
env_sem = posix_ipc.Semaphore(env_sem_name, posix_ipc.O_CREX, initial_value=1)

print("Getting the agent semaphore")
agent_sem.acquire()

# Getting existing shared memory:
shm = posix_ipc.SharedMemory(shared_mem_name)
data = mmap.mmap(shm.fd, shm.size)
shm.close_fd()
# resource_tracker.unregister("/" + name, "shared_memory")

# Interpret it as numpy array and printing:
buffer = np.ndarray((10), dtype=np.int64, buffer=data)
# print(buffer)

# # Editing the data such that the other process sees the changes 🤓
# buffer[:] = 0
# buffer[::2] = 29
# buffer[:, ::3] = 5

# The main loop:
while buffer[0] > 0:
    agent_sem.acquire()
    
    print(buffer)

    buffer[5:] += 2

    env_sem.release()


data.close()
agent_sem.unlink()
env_sem.unlink()