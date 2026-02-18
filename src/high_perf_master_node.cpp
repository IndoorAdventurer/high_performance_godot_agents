#include "high_perf_master_node.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

HighPerfMasterNode::HighPerfMasterNode()
{}

void HighPerfMasterNode::_ready() {
	UtilityFunctions::print("It works!");
}

void HighPerfMasterNode::_bind_methods() {
	// ClassDB::bind_method(D_METHOD("get_amplitude"), &HighPerfMasterNode::get_amplitude);
	// ClassDB::bind_method(D_METHOD("set_amplitude", "p_amplitude"), &HighPerfMasterNode::set_amplitude);
	// ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "amplitude"), "set_amplitude", "get_amplitude");

	// ClassDB::bind_method(D_METHOD("get_speed"), &HighPerfMasterNode::get_speed);
	// ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &HighPerfMasterNode::set_speed);

	// ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");
}