# Project-specific assets
ASSETS = filesystem/strawberry_byte
SOUND_DIR = $(ASSETS)/sound
UI_DIR = $(ASSETS)/ui

T3DM_FILES := $(ASSETS)/wolfie.t3dm \
              $(ASSETS)/s4ys.t3dm \
              $(ASSETS)/dogman.t3dm \
              $(ASSETS)/mew.t3dm \
              $(ASSETS)/platform.t3dm \
              $(ASSETS)/lava.t3dm \
              $(ASSETS)/platform2.t3dm \
              $(ASSETS)/cloud_base.t3dm

SPRITE_FILES := $(ASSETS)/wolf_eye.sprite \
                $(ASSETS)/frog_eye.sprite \
                $(ASSETS)/libdragon_logo.sprite \
                $(ASSETS)/nose.sprite \
                $(ASSETS)/n64brew.sprite \
                $(ASSETS)/mew_eye.sprite \
                $(ASSETS)/mew_ear.sprite \
                $(ASSETS)/jam_logo.sprite \
                $(ASSETS)/dogman_eye.sprite \
                $(ASSETS)/dogman_eyebrow.sprite \
                $(ASSETS)/dogman_mouth.sprite \
                $(ASSETS)/fast64.sprite \
                $(ASSETS)/bricks48.i8.sprite \
                $(ASSETS)/lava00.rgba16.sprite \
                $(ASSETS)/lava08.rgba16.sprite

SOUND_FILES := $(SOUND_DIR)/tribeof.xm64 \
               $(SOUND_DIR)/grunt-01.wav64 \
               $(SOUND_DIR)/lava-loop-2.wav64 \
               $(SOUND_DIR)/stones-falling.wav64 \
               $(SOUND_DIR)/strong_wind_blowing.wav64

UI_SPRITE_FILES := $(UI_DIR)/buttons/control_stick.ia8.sprite \
                   $(UI_DIR)/buttons/d_pad_triggers.ia8.sprite \
                   $(UI_DIR)/buttons/c_buttons0.rgba32.sprite \
                   $(UI_DIR)/buttons/c_buttons1.rgba32.sprite \
                   $(UI_DIR)/buttons/face_buttons0.rgba32.sprite \
                   $(UI_DIR)/buttons/face_buttons1.rgba32.sprite \
                   $(UI_DIR)/logos/libdragon.ia4.sprite \
                   $(UI_DIR)/logos/mixamo.ia4.sprite \
                   $(UI_DIR)/logos/t3d.ia8.sprite \
                   $(UI_DIR)/logos/sb_b0.rgba32.sprite \
                   $(UI_DIR)/logos/sb_b1.rgba32.sprite \
                   $(UI_DIR)/logos/sb_top.rgba32.sprite \
                   $(UI_DIR)/panels/gloss.ia4.sprite \
                   $(UI_DIR)/panels/clouds.ia8.sprite \
                   $(UI_DIR)/panels/pattern_tessalate.ia4.sprite

FONT_FILES := $(UI_DIR)/fonts/TitanOne-Regular.font64 \
              $(UI_DIR)/fonts/OilOnTheWater-ee5O.font64

# Final assets list
ASSETS_LIST += $(T3DM_FILES) $(SPRITE_FILES) $(SOUND_FILES) $(UI_SPRITE_FILES) $(FONT_FILES)

# t3d flags
$(ASSETS)/s4ys.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/wolfie.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/dogman.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/mew.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/platform.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/lava.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/platform2.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/cloud_base.t3dm: T3DM_FLAGS = --base-scale=1

# audioconv flags
$(ASSETS)/tribeof.xm64: AUDIOCONV_FLAGS = ''
$(ASSETS)/lava-loop-2.wav64: AUDIOCONV_FLAGS = --wav-compress=3 --wav-mono
$(ASSETS)/stones-falling.wav64: AUDIOCONV_FLAGS = --wav-compress=3 --wav-mono
$(ASSETS)/strong_wind_blowing.wav64: AUDIOCONV_FLAGS = --wav-compress=3 --wav-mono
$(ASSETS)/grunt-01.wav64: AUDIOCONV_FLAGS = --wav-compress=3 --wav-mono


# font64 flags
$(UI_DIR)/fonts/OilOnTheWater-ee5O.font64: MKFONT_FLAGS += --outline 2 --size 18
$(UI_DIR)/fonts/TitanOne-Regular.font64: MKFONT_FLAGS += --outline 1 --size 12