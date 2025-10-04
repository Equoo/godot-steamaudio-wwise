release:
	scons platform=android arch=arm64 target=template_release && scons platform=android arch=x86_64 target=template_release && \
		scons platform=android arch=arm64 target=template_debug && scons platform=android arch=x86_64 target=template_debug && \
		scons platform=linux target=template_debug && scons platform=windows target=template_debug && \
		scons platform=linux target=template_release && scons platform=windows target=template_release
	mkdir godot-steam-audio-demo
	mkdir godot-steam-audio
	cp -r ./project/* ./godot-steam-audio-demo
	rm -rf ./godot-steam-audio-demo/addons/godot-steam-audio/bin/libphonon.so.dbg
	cp -r ./godot-steam-audio-demo/addons ./godot-steam-audio
