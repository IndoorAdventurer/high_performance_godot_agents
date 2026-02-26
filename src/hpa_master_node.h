#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

namespace godot {

	/**
	 * Runs N instances of your environment in parallel, and is responsible
	 * for syncing/communicating with Python via shared memory.
	 */
	class HPAMasterNode : public Node {
		GDCLASS(HPAMasterNode, Node)

		private:
			Ref<PackedScene> d_env_scene; // The scene representing the simulation
			int d_num_envs;				  // Number of parallel environments
			Vector2i d_obs_res;	          // Resolution of observation space
			// TODO: identifier for semaphore/shared memory
		
		public:
			HPAMasterNode();
			~HPAMasterNode() = default;

			void _ready() override;
			PackedStringArray _get_configuration_warnings() const override;

			/**
			 * Create the simulation environments. Gets called in _ready().
			 */
			void _init_envs();

			// Getters and setters:
			void set_env_scene(const Ref<PackedScene> p_scene);
			Ref<PackedScene> get_env_scene() const;
			void set_num_envs(int p_num);
			int get_num_envs() const;
			void set_obs_res(Vector2i p_res);
			Vector2i get_obs_res() const;
			

		protected:
			static void _bind_methods();
	};

} // namespace godot