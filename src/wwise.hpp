#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <phonon.h>

using namespace godot;

#if defined(WINDOWS_ENABLED)
    #include <windows.h>
#elif defined(LINUX_ENABLED) || defined(ANDROID_ENABLED)
    #include <dlfcn.h>
#elif defined(MACOS_ENABLED)
    #include <dlfcn.h>
#elif defined(IOS_ENABLED)
    // iOS: usually statically linked, not dynamically loaded
	extern "C" {
		void AKSOUNDENGINE_CALL iplWwiseGetVersion(unsigned int* Major, unsigned int* Minor, unsigned int* Patch);
		void AKSOUNDENGINE_CALL iplWwiseInitialize(IPLContext Context, IPLWwiseSettings* Settings);
		void AKSOUNDENGINE_CALL iplWwiseTerminate();
		void AKSOUNDENGINE_CALL iplWwiseSetHRTF(IPLHRTF HRTF);
		void AKSOUNDENGINE_CALL iplWwiseSetSimulationSettings(IPLSimulationSettings SimulationSettings);
		void AKSOUNDENGINE_CALL iplWwiseSetReverbSource(IPLSource ReverbSource);
		IPLint32 AKSOUNDENGINE_CALL iplWwiseAddSource(AkGameObjectID GameObjectID, IPLSource Source);
		void AKSOUNDENGINE_CALL iplWwiseRemoveSource(AkGameObjectID GameObjectID);
		void AKSOUNDENGINE_CALL iplWwiseSetHRTFDisabled(IPLbool bDisabled);
	}
#endif

typedef struct
{
	IPLfloat32 MetersPerUnit;
} IPLWwiseSettings;

typedef void (AKSOUNDENGINE_CALL* iplWwiseGetVersion_t)(unsigned int* Major, unsigned int* Minor, unsigned int* Patch);
typedef void (AKSOUNDENGINE_CALL* iplWwiseInitialize_t)(IPLContext Context, IPLWwiseSettings* Settings);
typedef void (AKSOUNDENGINE_CALL* iplWwiseTerminate_t)();
typedef void (AKSOUNDENGINE_CALL* iplWwiseSetHRTF_t)(IPLHRTF HRTF);
typedef void (AKSOUNDENGINE_CALL* iplWwiseSetSimulationSettings_t)(IPLSimulationSettings SimulationSettings);
typedef void (AKSOUNDENGINE_CALL* iplWwiseSetReverbSource_t)(IPLSource ReverbSource);
typedef void (AKSOUNDENGINE_CALL* iplWwiseAddSource_t)(AkGameObjectID GameObjectID, IPLSource Source);
typedef void (AKSOUNDENGINE_CALL* iplWwiseRemoveSource_t)(AkGameObjectID GameObjectID);
typedef void (AKSOUNDENGINE_CALL* iplWwiseSetHRTFDisabled_t)(IPLbool bDisabled);

class SteamAudioWwiseBridge : public Object
{
	GDCLASS(SteamAudioWwiseBridge, Object);

public:
	SteamAudioWwiseBridge();
	~SteamAudioWwiseBridge();
	static void _bind_methods();
	bool initialize();
	static SteamAudioWwiseBridge *get_singleton();

private:
	static SteamAudioWwiseBridge *self;

	/** Handle to the Steam Audio Wwise plugin (SteamAudioWwise.dll or similar). */
    void* lib_handle;

	bool	loadLibrary();
	void	loadSymbols();
	void	unloadLibrary();

	/** Function pointers for the game engine / audio engine communication API. */
    iplWwiseGetVersion_t iplWwiseGetVersion;
    iplWwiseInitialize_t iplWwiseInitialize;
    iplWwiseTerminate_t iplWwiseTerminate;
    iplWwiseSetHRTF_t iplWwiseSetHRTF;
    iplWwiseSetSimulationSettings_t iplWwiseSetSimulationSettings;
    iplWwiseSetReverbSource_t iplWwiseSetReverbSource;
    iplWwiseAddSource_t iplWwiseAddSource;
    iplWwiseRemoveSource_t iplWwiseRemoveSource;
    iplWwiseSetHRTFDisabled_t iplWwiseSetHRTFDisabled;

	template<typename T>
	T get_symbol(const char *name) {
	#if defined(WINDOWS_ENABLED)
		return reinterpret_cast<T>(GetProcAddress((HMODULE)lib_handle, name));
	#else
		return reinterpret_cast<T>(dlsym(lib_handle, name));
	#endif
	}

	static HashMap<AkGameObjectID, IPLSource> registered_events;

	void 	on_source_added(Node *p_node);
	void 	on_source_removed(Node *p_node);
};