# High-performance Godot Agents
**A high-performance interface between Godot and Python for Reinforcement Learning (RL) using POSIX shared memory.**

## Overview
This library provides a low-latency bridge for RL applications. While inspired by the [Godot RL Agents Package](https://github.com/edbeeching/godot_rl_agents), this implementation is optimized for high-throughput use cases where standard networking protocols become a bottleneck.

### Key Use Case: Vision-Based RL
This library is specifically designed for environments with **large observation spaces**, such as high-resolution camera feeds or depth maps. By leveraging POSIX shared memory, it eliminates the serialization overhead typically found in socket-based communication.

> **Note:** As this relies on POSIX shared memory, this library is currently optimized for Linux/Unix-based environments.