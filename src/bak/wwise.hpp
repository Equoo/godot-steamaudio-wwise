#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <phonon.h>

typedef struct
{
	IPLfloat32 MetersPerUnit;
} IPLWwiseSettings;

void iplWwiseGetVersion(unsigned int* Major, unsigned int* Minor, unsigned int* Patch);
void iplWwiseInitialize(IPLContext Context, IPLWwiseSettings* Settings);
void iplWwiseTerminate();
void iplWwiseSetHRTF(IPLHRTF HRTF);
void iplWwiseSetSimulationSettings(IPLSimulationSettings SimulationSettings);
void iplWwiseSetReverbSource(IPLSource ReverbSource);
void iplWwiseAddSource(AkGameObjectID GameObjectID, IPLSource Source);
void iplWwiseRemoveSource(AkGameObjectID GameObjectID);
//void* iplWwiseSetHRTFDisabled(IPLbool bDisabled);

class SteamAudioWwiseBridge : public godot::Object
{
	GDCLASS(SteamAudioWwiseBridge, godot::Object);

public:
	SteamAudioWwiseBridge();
	~SteamAudioWwiseBridge();
	static void _bind_methods();
	bool initialize();
};