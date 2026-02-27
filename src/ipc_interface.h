#pragma once
#include <godot_cpp/variant/string.hpp>
#include <semaphore.h>
#include <cstddef>


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
        void  *d_shm_ptr;       // Pointer to shared memory region
        size_t d_shm_size;      // Size of shared memory region in bytes

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
             * Opens the semaphores (created by Python) and creates and maps
             * the shared memory region. Returns false on failure.
             */
            bool initialize(const String &name, size_t shm_size);

        private:
            /**
             * Unmaps the shared memory, unlinks its name, and closes the
             * semaphores. Safe to call on a partially or fully initialized
             * instance.
             */
            void _clear_resources();

            /**
             * Creates and maps the POSIX shared memory region. Pre-unlinks
             * any stale region left by a previous crashed run. Returns false
             * on failure.
             */
            bool _init_shared_memory();
    };
} // namespace godot

