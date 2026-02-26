#pragma once
#include <godot_cpp/variant/string.hpp>
#include <semaphore.h>


namespace godot
{
    /**
     * Encapsulates POSIX Shared Memory and Semaphores for IPC with the Python
     * TODO: reference corresponding Python file (receiving end).
     * 
     * Python creates the semaphores, while the C++ side creates the shared
     * memory. This is because the Python process starts before Godot, while
     * Godot is the one that knows how large the shared memory must be.
     */
    class IPCInterface {
    
        String d_name;          // Name associated with specific instance
        sem_t *d_env_ready;     // Environment Ready Semaphore
        sem_t *d_act_ready;     // Action Ready Semaphore

        public:
            IPCInterface();
            ~IPCInterface();

            // No copying:
            IPCInterface(IPCInterface const &other) = delete;
            IPCInterface &operator=(IPCInterface const &other) = delete;

            // Move semantics:
            IPCInterface(IPCInterface &&other) noexcept;
            IPCInterface &operator=(IPCInterface &&other) noexcept;

            /**
             * Initializes the semaphores and shared memory.
             */
            void initialize(const String &name);
        
        private:
            /**
             * Releases semaphores and resources.
             */
            void _clear_resources();
    };
} // namespace godot

