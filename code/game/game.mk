# Project-specific assets
ASSETS = filesystem/game
DIALOG_DIR = $(ASSETS)/dialogs
LEVEL_DIR = $(ASSETS)/levels
SOUND_DIR = $(ASSETS)/sound
UI_DIR = $(ASSETS)/ui

TXT_FILES :=  $(LEVEL_DIR)/levelA.txt

T3DM_FILES := $(ASSETS)/capsule.t3dm \
              $(ASSETS)/n64logo.t3dm \
              $(ASSETS)/pipo.t3dm \
              $(ASSETS)/wolfie.t3dm \
              $(ASSETS)/s4ys.t3dm \
              $(ASSETS)/dogman.t3dm \
              $(ASSETS)/mew.t3dm \
              $(ASSETS)/room.t3dm \
              $(ASSETS)/cube.t3dm \
              $(ASSETS)/hex_platform.t3dm \
              $(ASSETS)/levelA.t3dm

SPRITE_FILES := $(ASSETS)/black_filled_tile.sprite \
                $(ASSETS)/black_tile.sprite \
                $(ASSETS)/capsule_skin.sprite \
                $(ASSETS)/eye.sprite \
                $(ASSETS)/wolf_eye.sprite \
                $(ASSETS)/frog_eye.sprite \
                $(ASSETS)/libdragon_logo.sprite \
                $(ASSETS)/nose.sprite \
                $(ASSETS)/green_tile.sprite \
                $(ASSETS)/grey_tile.sprite \
                $(ASSETS)/red_tile.sprite \
                $(ASSETS)/yellow_tile.sprite \
                $(ASSETS)/n64brew.sprite \
                $(ASSETS)/mew_eye.sprite \
                $(ASSETS)/mew_ear.sprite \
                $(ASSETS)/jam_logo.sprite \
                $(ASSETS)/dogman_eye.sprite \
                $(ASSETS)/dogman_eyebrow.sprite \
                $(ASSETS)/dogman_mouth.sprite \
                $(ASSETS)/fast64.sprite \

SOUND_FILES := $(SOUND_DIR)/ene.xm64 \
               $(SOUND_DIR)/boing.wav64

UI_SPRITE_FILES := $(UI_DIR)/buttons/control_stick.ia8.sprite \
                   $(UI_DIR)/buttons/d_pad_triggers.ia8.sprite \
                   $(UI_DIR)/buttons/c_buttons0.rgba32.sprite \
                   $(UI_DIR)/buttons/c_buttons1.rgba32.sprite \
                   $(UI_DIR)/buttons/face_buttons0.rgba32.sprite \
                   $(UI_DIR)/buttons/face_buttons1.rgba32.sprite \
                   $(UI_DIR)/panels/border.ia4.sprite \
                   $(UI_DIR)/panels/border.ia8.sprite \
                   $(UI_DIR)/panels/gloss.ia4.sprite \
                   $(UI_DIR)/panels/gloss.ia8.sprite \
                   $(UI_DIR)/panels/gradient.ia4.sprite \
                   $(UI_DIR)/panels/pattern_bubble_grid.ia4.sprite \
                   $(UI_DIR)/panels/pattern_tessalate.ia4.sprite \
                   $(UI_DIR)/panels/star.ia8.sprite

FONT_FILES := $(UI_DIR)/fonts/chunkysans.font64 \
              $(UI_DIR)/fonts/TitanOne-Regular.font64

# Final assets list
ASSETS_LIST += $(TXT_FILES) $(T3DM_FILES) $(SPRITE_FILES) $(SOUND_FILES) $(UI_SPRITE_FILES) $(FONT_FILES)

# t3d flags
$(ASSETS)/capsule.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/n64logo.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/cube.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/pipo.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/s4ys.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/wolfie.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/dogman.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/mew.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/room.t3dm: T3DM_FLAGS = --base-scale=1 --bvh
$(ASSETS)/levelA.t3dm: T3DM_FLAGS = --base-scale=1 --bvh
$(ASSETS)/hex_platform.t3dm: T3DM_FLAGS = --base-scale=1 --bvh

# audioconv flags
$(ASSETS)/ene.xm64: AUDIOCONV_FLAGS = ''
$(ASSETS)/boing.wav64: AUDIOCONV_FLAGS = ''

# font64 flags
$(UI_DIR)/fonts/chunkysans.font64: MKFONT_FLAGS += --outline 2 --size 12
$(UI_DIR)/fonts/TitanOne-Regular.font64: MKFONT_FLAGS += --outline 1 --size 12

ifeq ($(DEBUG),1)
  N64_CFLAGS += -ggdb
endif

# Add rule for copying TXTs
$(FILESYSTEM_DIR)/%.txt: $(ASSETS_DIR)/%.txt
	@mkdir -p $(dir $@)
	@echo "    [TXT] $@"
	@cp $< $@