
#ifndef WWISE_PLAYER_HPP
#define WWISE_PLAYER_HPP

#include <godot_cpp/core/class_db.hpp>
#include <scene/ak_event.h>

// static inline AkGameObjectID get_ak_game_object_id(const Node* p_node)
// {
	// return p_node == nullptr ? AK_INVALID_GAME_OBJECT : p_node->get_instance_id();
// }

class SteamaudioEvent3D : public AkEvent3D {
    GDCLASS(SteamaudioEvent3D, AkEvent3D);

protected:
    static void _bind_methods();

public:
    SteamaudioEvent3D() {}
    ~SteamaudioEvent3D() {}

    void _enter_tree() override;
    void _exit_tree() override;
};

#endif