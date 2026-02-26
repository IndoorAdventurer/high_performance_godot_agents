# High Performance Godot Agents

GDExtension plugin for vision-based RL environments: runs N parallel environment instances
in Godot, communicates observations/actions to Python via POSIX shared memory and semaphores.

## Build

```bash
scons platform=linux target=editor    # editor build
scons platform=linux target=template_debug  # debug build
```

All `.cpp` files in `src/` are compiled automatically. Generated files go to `src/gen/`.

## Adding a new class

1. Create `src/my_class.h` and `src/my_class.cpp`
2. Register it in `src/register_types.cpp` with `GDREGISTER_CLASS(MyClass)`
3. Add XML documentation to `doc_classes/` if it's a public node

## Naming conventions

- Class member variables: `d_` prefix (e.g. `d_num_envs`)
- Method parameters: `p_` prefix (e.g. `p_scene`)
- Classes: `PascalCase`, methods: `snake_case`
- Follow Godot GDExtension patterns for `_bind_methods()` and property registration

## Architecture

- `HPAMasterNode` — root node; owns N SubViewports (one per environment) and one `IPCInterface`
- `IPCInterface` — POSIX shared memory + semaphores; Python creates semaphores, C++ creates shared memory
- Semaphore convention: `/<name>_env_ready`, `/<name>_act_ready`
- Visual data: collected on GPU (compute shader, single readback), sent as one bulk transfer
