
#include "ak_steamaudio_event.hpp"

using namespace godot;

void SteamaudioEvent3D::_enter_tree() {
    AkEvent3D::_enter_tree();
    if (is_inside_tree() && get_node_or_null<Spatial>("..")) {
        AkSoundEngine::RegisterGameObj(get_ak_game_object_id(get_parent()), StringName(get_name().utf8().get_data()));
    }
}

void SteamaudioEvent3D::_exit_tree() {
    if (is_inside_tree() && get_node_or_null<Spatial>("..")) {
        AkSoundEngine::UnregisterGameObj(get_ak_game_object_id(get_parent()));
    }
    AkEvent3D::_exit_tree();
}