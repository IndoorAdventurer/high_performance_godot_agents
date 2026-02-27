#include "ipc_interface.h"

#include <godot_cpp/core/error_macros.hpp>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace godot;


IPCInterface::IPCInterface()
:
    d_name(),
    d_env_ready(SEM_FAILED),
    d_act_ready(SEM_FAILED),
    d_shm_ptr(nullptr),
    d_shm_size(0)
{}

IPCInterface::~IPCInterface() {
    _clear_resources();
}

IPCInterface::IPCInterface(IPCInterface &&other) noexcept
:
    IPCInterface()
{
    *this = std::move(other);
}

IPCInterface &IPCInterface::operator=(IPCInterface &&other) noexcept {
    if (this != &other) {
        _clear_resources();

        d_name = std::move(other.d_name);
        d_env_ready = other.d_env_ready;
        d_act_ready = other.d_act_ready;
        d_shm_ptr   = other.d_shm_ptr;
        d_shm_size  = other.d_shm_size;

        other.d_env_ready = SEM_FAILED;
        other.d_act_ready = SEM_FAILED;
        other.d_shm_ptr   = nullptr;
        other.d_shm_size  = 0;
    }
    return *this;
}

void IPCInterface::_clear_resources() {
    if (d_shm_ptr != nullptr) {
        munmap(d_shm_ptr, d_shm_size);
        shm_unlink(("/" + d_name).utf8().get_data());
        d_shm_ptr  = nullptr;
        d_shm_size = 0;
    }

    if (d_env_ready != SEM_FAILED) {
        sem_close(d_env_ready);
        d_env_ready = SEM_FAILED;
    }

    if (d_act_ready != SEM_FAILED) {
        sem_close(d_act_ready);
        d_act_ready = SEM_FAILED;
    }
}

bool IPCInterface::initialize(const String &name, size_t shm_size) {
    _clear_resources();
    d_name     = name;
    d_shm_size = shm_size;

    // Open semaphores. These should already be initialized on the Python side.
    d_env_ready = sem_open(("/" + name + "_env_ready").utf8().get_data(), 0);
    d_act_ready = sem_open(("/" + name + "_act_ready").utf8().get_data(), 0);

    return _init_shared_memory();
}

bool IPCInterface::_init_shared_memory() {
    // References:
    // https://man7.org/linux/man-pages/man7/shm_overview.7.html
    // https://www.geeksforgeeks.org/linux-unix/posix-shared-memory-api/

    const CharString shm_name = ("/" + d_name).utf8();

    // Pre-unlink to clear any stale region from a previous crashed run.
    // ENOENT means it didn't exist, which is fine.
    if (shm_unlink(shm_name.get_data()) == -1 && errno != ENOENT) {
        ERR_PRINT("shm_unlink (pre-unlink) failed: " + String(strerror(errno)));
        return false;
    }

    int fd = shm_open(shm_name.get_data(), O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        ERR_PRINT("shm_open failed: " + String(strerror(errno)));
        return false;
    }

    if (ftruncate(fd, d_shm_size) == -1) {
        ERR_PRINT("ftruncate failed: " + String(strerror(errno)));
        close(fd);
        return false;
    }

    void *ptr = mmap(nullptr, d_shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd); // fd can be closed immediately after mmap
    if (ptr == MAP_FAILED) {
        ERR_PRINT("mmap failed: " + String(strerror(errno)));
        return false;
    }

    d_shm_ptr = ptr;
    return true;
}