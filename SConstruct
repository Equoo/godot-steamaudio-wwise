#!/usr/bin/env python

env = SConscript("libs/godot-cpp/SConstruct")
opts = Variables([], ARGUMENTS)

opts.Add(
    PathVariable(
        "target_path",
        "The path where the lib is installed.",
        "project/addons/godot-steamaudio-wwise/bin/",
        PathVariable.PathIsDirCreate,
    )
)
opts.Add(PathVariable("wwise_sdk", "The Wwise SDK path", "", PathVariable.PathAccept))
opts.Add(EnumVariable("ios_arch", "Target iOS architecture", "arm64", ["arm64"]))
opts.Add("ios_min_version", "Target minimum iphoneos/iphonesimulator version", "12.0")
opts.Add(BoolVariable("ios_simulator", "Target iOS Simulator", False))
opts.Update(env)

# Local dependency paths, adapt them to your setup
steamaudio_headers_path = "libs/steamaudio/wwise/src/SoundEnginePlugin"
phonon_headers_path = "libs/steamaudio/wwise/include/phonon"
godot_headers_path = "libs/godot-cpp/gdextension/"
cpp_bindings_path = "libs/godot-cpp/"

# Wwise dependency paths
wwise_sdk_headers_path = env["wwise_sdk"] + "/include/"
wwise_sdk_libs_path = ""
steamaudio_libs_path = "libs/steamaudio/lib"

if env["platform"] == "windows":
    steamaudio_libs_path += "/windows-x64"
    env.Append(LIBS=["phonon"])
    if env["target"] == "template_debug":
        wwise_sdk_libs_path = env["wwise_sdk"] + "/x64_vc170/Debug(StaticCRT)/lib/"
    else:
        wwise_sdk_libs_path = env["wwise_sdk"] + "/x64_vc170/Release(StaticCRT)/lib/"

if env["platform"] == "macos":
    steamaudio_libs_path += "/osx"
    #env.Append(LIBS=["libphonon.dylib"])
    if env["target"] == "template_debug":
        wwise_sdk_libs_path = env["wwise_sdk"] + "/Mac_Xcode1500/Debug/lib/"
    else:
        wwise_sdk_libs_path = env["wwise_sdk"] + "/Mac_Xcode1500/Release/lib/"

if env["platform"] == "linux":
    steamaudio_libs_path += "/linux-x64"
    #env.Append(LIBS=["libphonon.so"])
    if env["target"] == "template_debug":
        wwise_sdk_libs_path = env["wwise_sdk"] + "/Linux_x64/Debug/lib/"
    else:
        wwise_sdk_libs_path = env["wwise_sdk"] + "/Linux_x64/Release/lib/"

if env["platform"] == "ios" and not env["ios_simulator"]:
    if env["target"] == "template_debug":
        wwise_sdk_libs_path = env["wwise_sdk"] + "/iOS_Xcode1500/Debug-iphoneos/lib/"
    else:
        wwise_sdk_libs_path = env["wwise_sdk"] + "/iOS_Xcode1500/Release-iphoneos/lib/"

if env["platform"] == "ios" and env["ios_simulator"]:
    if env["target"] == "template_debug":
        wwise_sdk_libs_path = env["wwise_sdk"] + "/iOS_Xcode1500/Debug-iphonesimulator/lib/"
    else:
        wwise_sdk_libs_path = env["wwise_sdk"] + "/iOS_Xcode1500/Release-iphonesimulator/lib/"

if env["target"] == "template_debug":
    env.Append(CPPDEFINES=["TEMPLATE_DEBUG"])
elif env["target"] == "template_release":
    env.Append(CPPDEFINES=["TEMPLATE_RELEASE"])

if env["platform"] == "windows":
    if env["target"] in ("template_debug"):
            env.Append(CPPDEFINES=["_DEBUG"])
            env.Append(LINKFLAGS=["-DEBUG"])
            env.Append(CCFLAGS=["/MTd"])
    else:
        if env["target"] in ("editor"):
            env.Append(CPPDEFINES=["NDEBUG"])
        else:
            env.Append(CPPDEFINES=["AK_OPTIMIZED"])
            env.Append(CPPDEFINES=["NDEBUG"])

    env.Append(
        CPPDEFINES=["WIN32", "_WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "UNICODE"]
    )

elif env["platform"] == "macos":
    if env["target"] in ("template_debug"):
        env.Append(CPPDEFINES=["_DEBUG"])

        env.Append(LINKFLAGS=["-DEBUG"])
    else:
        if env["target"] in ("editor"):
            env.Append(CPPDEFINES=["NDEBUG"])
            env.Append(CXXFLAGS=["-fexceptions"])
        else:
            env.Append(CPPDEFINES=["AK_OPTIMIZED"])
            env.Append(CPPDEFINES=["NDEBUG"])

    env.Append(CXXFLAGS=["-std=c++20"])

    env.Append(
        LINKFLAGS=[
            "-framework",
            "CoreAudio",
             "-framework",
            "AVFoundation",
            "-Wl,-undefined,dynamic_lookup"
        ]
    )

elif env["platform"] == "linux":
    env["CXX"] = "g++-11"
    if env["target"] in ("template_debug"):
        env.Append(CPPDEFINES=["_DEBUG"])
        env.Append(LINKFLAGS=["-DEBUG"])
    else:
        if env["target"] in ("editor"):
            env.Append(CPPDEFINES=["NDEBUG"])
            env.Append(CXXFLAGS=["-fexceptions"])
        else:
            env.Append(CPPDEFINES=["AK_OPTIMIZED"])
            env.Append(CPPDEFINES=["NDEBUG"])

    env.Append(CCFLAGS=["-fPIC", "-Wwrite-strings"])
    env.Append(CXXFLAGS=["-std=c++20"])
    symbols_file = env.File("linux/symbols-extension.map")
    env.Append(
    LINKFLAGS=[
        "-Wl,-R,'$$ORIGIN'",
        "-Wl,--no-undefined,--version-script=" + symbols_file.abspath,
        "-static-libgcc",
        "-static-libstdc++",
    ])


env.Append(
    CPPPATH=[
        ".",
        godot_headers_path,
        cpp_bindings_path + "include/",
        cpp_bindings_path + "include/core/",
        cpp_bindings_path + "include/gen/",
        wwise_sdk_headers_path,
        steamaudio_headers_path,
        phonon_headers_path 
    ]
)
env.Append(LIBPATH=[
        cpp_bindings_path + "bin/",
        wwise_sdk_libs_path,
        steamaudio_libs_path
    ]
)

env.Append(CPPPATH=["src/"])

sources = Glob("src/*.cpp")

#env.Append(LIBS=["SteamAudioWwiseFX"])
    

if env["target"] in ("editor", "template_debug"):
    if env["platform"] == "windows":
        env.Append(LIBS=["msacm32", "ws2_32"])

if env["target"] == "editor" and env["platform"] == "linux":
    env.Append(LIBS=["tbb"])

if env["platform"] == "windows":
    env.Append(LIBS=["advapi32", "user32", "ole32"])
if env["platform"] == "linux":
    env.Append(LIBS=["pthread", "dl"])

if env["platform"] == "windows":
    library = env.SharedLibrary(
        target=env["target_path"]
        + "libgodot-steamaudio-wwise.{}.{}.x86_64.dll".format(env["platform"], env["target"]),
        source=sources,
    )

if env["platform"] == "macos":
    library = env.SharedLibrary(
        target=env["target_path"]
        + "libgodot-steamaudio-wwise.{}.{}.x86_64.dylib".format(
            env["platform"], env["target"]
        ),
        source=sources,
    )

if env["platform"] == "linux":
    import shutil

    library = env.SharedLibrary(
        target=env["target_path"]
        + "libgodot-steamaudio-wwise.{}.{}.x86_64.so".format(env["platform"], env["target"]),
        source=sources,
    )

if env["platform"] == "ios":
    ios_lib_path = env[
        "target_path"
    ] + "libsteamaudio.{}.{}.framework/libsteamaudio.{}.{}".format(
        env["platform"], env["target"], env["platform"], env["target"]
    )

    library = env.SharedLibrary(
        target=ios_lib_path,
        source=sources,
    )

    output_file = "Info.plist"
    output_dir = env["target_path"] + "libsteamaudio.{}.{}.framework".format(
        env["platform"], env["target"]
    )

    env.Replace(LIB_NAME="libsteamaudio.{}.{}".format(env["platform"], env["target"]))
    env.Replace(MIN_IOS_VERSION=env["ios_min_version"])

    def create_output_file(target, source, env):
        # Create the output directory if it doesn't exist
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

        lib_name = "libsteamaudio.{}.{}".format(env["platform"], env["target"])

        with open(os.path.join(output_dir, output_file), "w") as f:
            xml_text = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
<key>CFBundleShortVersionString</key>
<string>1.0</string>
<key>CFBundleIdentifier</key>
<string>com.gdextension.framework.{}</string>
<key>CFBundleName</key>
<string>{}</string>
<key>CFBundleExecutable</key>
<string>{}</string>
<key>DTPlatformName</key>
<string>iphoneos</string>
<key>CFBundleInfoDictionaryVersion</key>
<string>6.0</string>
<key>CFBundleVersion</key>
<string>1</string>
<key>CFBundlePackageType</key>
<string>FMWK</string>
<key>MinimumOSVersion</key>
<string>{}</string>
</dict>
</plist>""".format(
                lib_name.replace("_", "-"), lib_name, lib_name, env["ios_min_version"]
            )

            unescaped_text = html.unescape(xml_text)
            f.write(unescaped_text)
        return None

    target = os.path.join(output_dir, output_file)
    create_output_file_action = Action(target, create_output_file)
    AddPostAction(library, create_output_file)


Default(library)

Help(opts.GenerateHelpText(env))

