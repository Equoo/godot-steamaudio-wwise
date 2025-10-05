#include "wwise.hpp"
#include "server.hpp"
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <scene/ak_event.h>

using namespace godot;

SteamAudioWwiseBridge::SteamAudioWwiseBridge() {
	self = this;
	lib_handle = nullptr;
}
SteamAudioWwiseBridge::~SteamAudioWwiseBridge() {
	unloadLibrary();
}

bool SteamAudioWwiseBridge::loadLibrary()
{
	String base_path = OS::get_singleton()->get_executable_path().get_base_dir();

	String lib_path;
#if defined(WINDOWS_ENABLED) // NOTE: ADD IF DEBUG/RELEASE
	lib_path = "addons/Wwise/native/lib/win64/template_debug/profile/DSP/SteamAudioWwise.dll";
	lib_handle = (void *)LoadLibraryW((LPCWSTR)lib_path.wide_string().ptr());
#elif defined(LINUX_ENABLED)
	lib_path = base_path.path_join("addons/Wwise/native/lib/win64/template_debug/profile/DSP/SteamAudioWwise.so");
	lib_handle = dlopen(lib_path.utf8().get_data(), RTLD_LAZY);
#elif defined(ANDROID_ENABLED)
	lib_path = base_path.path_join("addons/Wwise/native/lib/win64/template_debug/profile/DSP/SteamAudioWwise.so");
	lib_handle = dlopen(lib_path.utf8().get_data(), RTLD_LAZY);
#elif defined(MACOS_ENABLED)
	lib_path = base_path.path_join("addons/Wwise/native/lib/win64/template_debug/profile/DSP/SteamAudioWwise.dylib");
	lib_handle = dlopen(lib_path.utf8().get_data(), RTLD_LAZY);
#elif defined(IOS_ENABLED)
	// On iOS: normally statically linked, no dlopen allowed
	return true;
#endif

	if (!lib_handle) {
		UtilityFunctions::print("Failed to load: ", lib_path);
		return false;
	}

	return true;
}

void SteamAudioWwiseBridge::loadSymbols() {
	this->iplWwiseGetVersion = get_symbol<iplWwiseGetVersion_t>("iplWwiseGetVersion");
	this->iplWwiseInitialize = get_symbol<iplWwiseInitialize_t>("iplWwiseInitialize");
	this->iplWwiseTerminate = get_symbol<iplWwiseTerminate_t>("iplWwiseTerminate");
	this->iplWwiseSetHRTF = get_symbol<iplWwiseSetHRTF_t>("iplWwiseSetHRTF");
	this->iplWwiseSetSimulationSettings = get_symbol<iplWwiseSetSimulationSettings_t>("iplWwiseSetSimulationSettings");
	this->iplWwiseSetReverbSource = get_symbol<iplWwiseSetReverbSource_t>("iplWwiseSetReverbSource");
	this->iplWwiseAddSource = get_symbol<iplWwiseAddSource_t>("iplWwiseAddSource");
	this->iplWwiseRemoveSource = get_symbol<iplWwiseRemoveSource_t>("iplWwiseRemoveSource");
	this->iplWwiseSetHRTFDisabled = get_symbol<iplWwiseSetHRTFDisabled_t>("iplWwiseSetHRTFDisabled");
}

void SteamAudioWwiseBridge::unloadLibrary() {
	if (lib_handle) {
#if defined(WINDOWS_ENABLED)
		FreeLibrary((HMODULE)lib_handle);
#elif defined(LINUX_ENABLED) || defined(ANDROID_ENABLED) || defined(MACOS_ENABLED)
		dlclose(lib_handle);
#endif
		lib_handle = nullptr;
	}
}


void SteamAudioWwiseBridge::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("initialize"), &SteamAudioWwiseBridge::initialize);
}

bool SteamAudioWwiseBridge::initialize()
{
	if (!loadLibrary()) {
		UtilityFunctions::print("Failed to load Steam Audio Wwise library.");
		return false;
	}
	UtilityFunctions::print("Loaded Steam Audio Wwise library.");
	
	loadSymbols();
	if (!this->iplWwiseGetVersion) {
		UtilityFunctions::print("Failed to load symbols from Steam Audio Wwise library.");
		return false;
	}

	unsigned int major, minor, patch;
	this->iplWwiseGetVersion(&major, &minor, &patch);
	UtilityFunctions::print("Steam Audio Wwise version: ", major, ".", minor, ".", patch);
	
	IPLWwiseSettings WwiseSettings{};
	WwiseSettings.MetersPerUnit = 0.01f;
	
	this->iplWwiseInitialize(SteamAudioServer::get_singleton()->get_global_state()->ctx, &WwiseSettings);
	UtilityFunctions::print("Steam Audio Wwise initialized.");

	registered_events.clear();
	registered_events.reserve(64);

	SceneTree *tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
    if (tree) {
        tree->connect("node_added", Callable(this, "on_node_added"));
        tree->connect("node_removed", Callable(this, "on_node_removed"));
    }

	return true;
}

void SteamAudioWwiseBridge::on_source_added(Node *p_node) {
	AkEvent3D *ak_event = Object::cast_to<AkEvent3D>(p_node);
	if (ak_event) {
		auto gs = SteamAudioServer::get_singleton()->get_global_state();
		AkGameObjectID game_object_id = ak_event->get_instance_id();

		IPLSource source;
		IPLSourceSettings src_cfg{};
		src_cfg.flags = static_cast<IPLSimulationFlags>(IPL_SIMULATIONFLAGS_DIRECT | IPL_SIMULATIONFLAGS_REFLECTIONS);
		
		IPLerror Status = iplSourceCreate(gs->sim, &src_cfg, &source);
		if (Status != IPL_STATUS_SUCCESS)
		{
			UtilityFunctions::print("Unable to create source for Wwise event. [", Status, "]");
			// iplSimulatorRelease(&gs->sim);
			return;
		}

		this->iplWwiseAddSource(game_object_id, source);
		iplSourceAdd(source, gs->sim);
		iplSimulatorCommit(gs->sim);

		registered_events.insert(game_object_id, source);
	}
}

void SteamAudioWwiseBridge::on_source_removed(Node *p_node) {
	AkEvent3D *ak_event = Object::cast_to<AkEvent3D>(p_node);
	if (ak_event) {
		auto gs = SteamAudioServer::get_singleton()->get_global_state();
		AkGameObjectID game_object_id = ak_event->get_instance_id();

		auto it = registered_events.find(game_object_id);
		if (it != registered_events.end()) {
			this->iplWwiseRemoveSource(game_object_id);
			iplSourceRemove(it->value, SteamAudioServer::get_singleton()->get_global_state()->sim);
			iplSimulatorCommit(gs->sim);
			iplSourceRelease(const_cast<IPLSource *>(&it->value));
		}

		registered_events.erase(game_object_id);
	}
}

SteamAudioWwiseBridge *SteamAudioWwiseBridge::get_singleton() {
	return self;
}

SteamAudioWwiseBridge *SteamAudioWwiseBridge::self = nullptr;
