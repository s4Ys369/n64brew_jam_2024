# Project-specific assets
ASSETS = filesystem/game
DIALOG_DIR = $(ASSETS)/dialogs
LEVEL_DIR = $(ASSETS)/levels
SOUND_DIR = $(ASSETS)/sound
UI_DIR = $(ASSETS)/ui

JSON_FILES := $(DIALOG_DIR)/dialog_test.json \
              $(LEVEL_DIR)/saveGameDataTest.json \
              $(LEVEL_DIR)/testLevel.json

T3DM_FILES := $(ASSETS)/capsule.t3dm \
              $(ASSETS)/n64logo.t3dm \
              $(ASSETS)/pipo.t3dm \
              $(ASSETS)/room.t3dm \
              $(ASSETS)/testLevel.t3dm

SPRITE_FILES := $(ASSETS)/black_filled_tile.sprite \
                $(ASSETS)/black_tile.sprite \
                $(ASSETS)/capsule_skin.sprite \
                $(ASSETS)/eye.sprite \
                $(ASSETS)/green_tile.sprite \
                $(ASSETS)/grey_tile.sprite \
                $(ASSETS)/red_tile.sprite \
                $(ASSETS)/yellow_tile.sprite

SOUND_FILES := $(SOUND_DIR)/ene.xm64 \
               $(SOUND_DIR)/boing.wav64

UI_SPRITE_FILES := $(UI_DIR)/control_stick.ia8.sprite \
                   $(UI_DIR)/d_pad_triggers.ia8.sprite \
                   $(UI_DIR)/c_buttons0.rgba32.sprite \
                   $(UI_DIR)/c_buttons1.rgba32.sprite \
                   $(UI_DIR)/face_buttons0.rgba32.sprite \
                   $(UI_DIR)/face_buttons1.rgba32.sprite

FONT_FILES := $(ASSETS)/chunkysans.font64 \
              $(ASSETS)/TitanOne-Regular.font64

# Final assets list
ASSETS_LIST += $(JSON_FILES) $(T3DM_FILES) $(SPRITE_FILES) $(SOUND_FILES) $(UI_SPRITE_FILES) $(FONT_FILES)

# t3d flags
$(ASSETS)/capsule.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/n64logo.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/pipo.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/room.t3dm: T3DM_FLAGS = --base-scale=1 --bvh
$(ASSETS)/testLevel.t3dm: T3DM_FLAGS = --base-scale=1200 --bvh

# audioconv flags
$(ASSETS)/ene.xm64: AUDIOCONV_FLAGS = ''
$(ASSETS)/boing.wav64: AUDIOCONV_FLAGS = ''

# font64 flags
$(ASSETS)/chunkysans.font64: MKFONT_FLAGS += --outline 2 --size 12
$(ASSETS)/TitanOne-Regular.font64: MKFONT_FLAGS += --outline 1 --size 12

# Add rule for copying JSONs
$(FILESYSTEM_DIR)/%.json: $(ASSETS_DIR)/%.json
	@mkdir -p $(dir $@)
	@echo "    [JSON] $@"
	@cp $< $@