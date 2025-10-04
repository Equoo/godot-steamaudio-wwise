#include "wwise.hpp"
#include "server.hpp"
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Plugin/SteamAudioWwiseFXFactory.h>

using namespace godot;

SteamAudioWwiseBridge::SteamAudioWwiseBridge() {}
SteamAudioWwiseBridge::~SteamAudioWwiseBridge() {}

void SteamAudioWwiseBridge::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("initialize"), &SteamAudioWwiseBridge::initialize);
}

bool SteamAudioWwiseBridge::initialize()
{
	unsigned int major, minor, patch;
	iplWwiseGetVersion(&major, &minor, &patch);
	UtilityFunctions::print("Steam Audio Wwise version: ", major, ".", minor, ".", patch);
	
	IPLWwiseSettings WwiseSettings{};
	WwiseSettings.MetersPerUnit = 0.01f;
	
	iplWwiseInitialize(SteamAudioServer::get_singleton()->get_global_state()->ctx, &WwiseSettings);
	UtilityFunctions::print("Steam Audio Wwise initialized.");
	return true;
}