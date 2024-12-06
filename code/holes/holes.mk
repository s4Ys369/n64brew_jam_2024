
ASSETS_LIST += \
	filesystem/holes/map.t3dm \
	filesystem/holes/building.t3dm \
	filesystem/holes/car.t3dm \
	filesystem/holes/hole.t3dm \
	filesystem/holes/hydrant.t3dm \
	filesystem/holes/hole.ia8.sprite \
	filesystem/holes/hydrant.ci8.sprite \
	filesystem/holes/street.ci4.sprite \
	filesystem/holes/window.ci4.sprite \
	filesystem/holes/window1.ci4.sprite \
	filesystem/holes/car.ci4.sprite \
	filesystem/holes/car.wav64 \
	filesystem/holes/building.wav64 \
	filesystem/holes/hydrant.wav64 \

filesystem/holes/car.wav64: AUDIOCONV_FLAGS_FLAGS += --wav-compress 3 --wav-mono
filesystem/holes/building.wav64: AUDIOCONV_FLAGS_FLAGS += --wav-compress 3 --wav-mono
filesystem/holes/hydrant.wav64: AUDIOCONV_FLAGS_FLAGS += --wav-compress 3 --wav-mono
