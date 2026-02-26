#include "hpa_master_node.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/sprite2d.hpp> // TODO: remove. Just for test now.
#include <godot_cpp/classes/viewport_texture.hpp>

using namespace godot;

HPAMasterNode::HPAMasterNode()
:
	d_env_scene(),
	d_num_envs(2),
	d_obs_res(128, 128)
{}

void HPAMasterNode::_ready() {
	if (Engine::get_singleton()->is_editor_hint())
		return;
	
	_init_envs();

}

void HPAMasterNode::_init_envs() {
	if (d_env_scene.is_null())
		return;

	for (int idx = 0; idx != d_num_envs; ++idx) {
		// Create subviewport:
		SubViewport *subview = memnew(SubViewport);
		subview->set_size(d_obs_res);
		subview->set_update_mode(SubViewport::UPDATE_ALWAYS);
		subview->set_use_own_world_3d(true);

		// Instantiate simulation scene:
		Node *scene_inst = d_env_scene->instantiate();
		subview->add_child(scene_inst);
		add_child(subview);

		// Create sprite to display the scene:
		Sprite2D *sprite = memnew(Sprite2D);
		sprite->set_texture(subview->get_texture());
		sprite->set_centered(false);
		sprite->set_position(Vector2(
			idx * d_obs_res.x,
			0
		));
		add_child(sprite);
	}
}

PackedStringArray HPAMasterNode::_get_configuration_warnings() const {
	PackedStringArray warnings = Node::_get_configuration_warnings();
	if (d_env_scene.is_null())
		warnings.push_back(
			"An environment scene must be provided for the simulation to run.");
	
	if (d_num_envs <= 0)
			warnings.push_back("Number of environments must be at least 1.");
	
	if (d_obs_res.x <= 0 or d_obs_res.y < 0)
		warnings.push_back("Observation space resolution must be positive.");
	
		return warnings;
}

void HPAMasterNode::set_env_scene(const Ref<PackedScene> p_scene) {
	d_env_scene = p_scene;
	update_configuration_warnings();
}

Ref<PackedScene> HPAMasterNode::get_env_scene() const {
	return d_env_scene;
}

void HPAMasterNode::set_num_envs(int p_num) {
	d_num_envs = p_num;
	update_configuration_warnings();
}

int HPAMasterNode::get_num_envs() const {
	return d_num_envs;
}

void HPAMasterNode::set_obs_res(Vector2i p_res) {
	d_obs_res = p_res;
	update_configuration_warnings();
}

Vector2i HPAMasterNode::get_obs_res() const {
	return d_obs_res;
}

void HPAMasterNode::_bind_methods() {
	// Environment scene property:
	ClassDB::bind_method(
		D_METHOD("set_env_scene", "p_scene"), &HPAMasterNode::set_env_scene);
	ClassDB::bind_method(
		D_METHOD("get_env_scene"), &HPAMasterNode::get_env_scene);
	ADD_PROPERTY(
		PropertyInfo(
			Variant::OBJECT,
			"environment_scene",
			PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"),
		"set_env_scene",
		"get_env_scene");
	
	// Number of environments property:
	ClassDB::bind_method(
		D_METHOD("set_num_envs", "p_num"), &HPAMasterNode::set_num_envs);
	ClassDB::bind_method(
		D_METHOD("get_num_envs"), &HPAMasterNode::get_num_envs);
	ADD_PROPERTY(
		PropertyInfo(
			Variant::INT,
			"num_envs",
			PROPERTY_HINT_RANGE, "0,1024,1,or_greater"),
		"set_num_envs",
		"get_num_envs");
	
	// Screen resolution:
	ClassDB::bind_method(
		D_METHOD("set_obs_res", "p_res"), &HPAMasterNode::set_obs_res);
	ClassDB::bind_method(
		D_METHOD("get_obs_res"), &HPAMasterNode::get_obs_res);
	ADD_PROPERTY(
		PropertyInfo(
			Variant::VECTOR2I, "obs_resolution"),
			"set_obs_res", "get_obs_res");
}