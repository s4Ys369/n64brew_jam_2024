# Project-specific assets
ASSETS = filesystem/game
DIALOG_DIR = $(ASSETS)/dialogs
LEVEL_DIR = $(ASSETS)/levels
SOUND_DIR = $(ASSETS)/sound
UI_DIR = $(ASSETS)/ui

TXT_FILES :=  $(LEVEL_DIR)/levelA.txt

T3DM_FILES := $(ASSETS)/wolfie.t3dm \
              $(ASSETS)/s4ys.t3dm \
              $(ASSETS)/dogman.t3dm \
              $(ASSETS)/mew.t3dm \
              $(ASSETS)/room.t3dm \
              $(ASSETS)/lava.t3dm \

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
                $(ASSETS)/brick24.i8.sprite \
                $(ASSETS)/bricks48.i8.sprite \
                $(ASSETS)/gate01.ci8.sprite \
                $(ASSETS)/lava00.rgba16.sprite \
                $(ASSETS)/lava08.rgba16.sprite \

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
$(ASSETS)/s4ys.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/wolfie.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/dogman.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/mew.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/room.t3dm: T3DM_FLAGS = --base-scale=1
$(ASSETS)/lava.t3dm: T3DM_FLAGS = --base-scale=1

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