#pragma once

#include <godot_cpp/classes/node.hpp>

namespace godot {

	/**
	 * Runs N instances of your environment in parallel, and is responsible
	 * for syncing/communicating with Python via shared memory.
	 */
	class HighPerfMasterNode : public Node {
		GDCLASS(HighPerfMasterNode, Node)

		private:
			// TODO
		
		public:
			HighPerfMasterNode();
			~HighPerfMasterNode() = default;

			void _ready() override;

		protected:
			static void _bind_methods();
	};

} // namespace godot