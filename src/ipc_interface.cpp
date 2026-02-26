#include "ipc_interface.h"

#include <semaphore.h>

using namespace godot;


IPCInterface::IPCInterface()
:
    d_name(),
    d_env_ready(SEM_FAILED),
    d_act_ready(SEM_FAILED)
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

        other.d_env_ready = SEM_FAILED;
        other.d_act_ready = SEM_FAILED;
    }
    return *this;
}

void IPCInterface::_clear_resources() {
    if (d_env_ready != SEM_FAILED) {
        sem_close(d_env_ready);
        d_env_ready = SEM_FAILED;
    }

    if (d_act_ready != SEM_FAILED) {
        sem_close(d_act_ready);
        d_act_ready = SEM_FAILED;
    }
}

void IPCInterface::initialize(const String &name) {
    _clear_resources();
    d_name = name;

    // Open semaphores. These should already be initialized on the Python side.
    d_env_ready = sem_open(("/" + name + "_env_ready").utf8().get_data(), 0);
    d_act_ready = sem_open(("/" + name + "_act_ready").utf8().get_data(), 0);
}