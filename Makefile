ROMNAME = gamejam2024
ROMTITLE = "N64BREW GAMEJAM 2024"

BUILD_DIR = build
ASSETS_DIR = assets
MINIGAME_DIR = code
FILESYSTEM_DIR = filesystem
MINIGAMEDSO_DIR = $(FILESYSTEM_DIR)/minigames

SRC = main.c core.c minigame.c menu.c

filesystem/squarewave.font64: MKFONT_FLAGS += --outline 1 --range all

###

include $(N64_INST)/include/n64.mk

MINIGAMES_LIST = $(notdir $(wildcard $(MINIGAME_DIR)/*))
DSO_LIST = $(addprefix $(MINIGAMEDSO_DIR)/, $(addsuffix .dso, $(MINIGAMES_LIST)))

IMAGE_LIST = $(wildcard $(ASSETS_DIR)/*.png)
FONT_LIST  = $(wildcard $(ASSETS_DIR)/*.ttf)
ASSETS_LIST += $(addprefix $(FILESYSTEM_DIR)/,$(notdir $(IMAGE_LIST:%.png=%.sprite)))
ASSETS_LIST += $(addprefix $(FILESYSTEM_DIR)/,$(notdir $(FONT_LIST:%.ttf=%.font64)))

ifeq ($(DEBUG), 1)
	N64_CFLAGS += -g -O0
	N64_LDFLAGS += -g
else
	N64_CFLAGS += -O2
endif

all: $(ROMNAME).z64

$(FILESYSTEM_DIR)/%.sprite: $(ASSETS_DIR)/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	@$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

$(FILESYSTEM_DIR)/%.font64: $(ASSETS_DIR)/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o filesystem "$<"

MAIN_ELF_EXTERNS := $(BUILD_DIR)/$(ROMNAME).externs
$(BUILD_DIR)/$(ROMNAME).dfs: $(ASSETS_LIST) $(DSO_LIST)
$(BUILD_DIR)/$(ROMNAME).elf: $(SRC:%.c=$(BUILD_DIR)/%.o) $(MAIN_ELF_EXTERNS)
$(MAIN_ELF_EXTERNS): $(DSO_LIST)

define MINIGAME_template
SRC_$(1) = $$(wildcard $$(MINIGAME_DIR)/$(1)/*.c) $$(wildcard $$(MINIGAME_DIR)/$(1)/*.cpp)
$$(MINIGAMEDSO_DIR)/$(1).dso: $$(SRC_$(1):%.c=$$(BUILD_DIR)/%.o)
endef

$(foreach minigame, $(MINIGAMES_LIST), $(eval $(call MINIGAME_template,$(minigame))))

$(ROMNAME).z64: N64_ROM_TITLE=$(ROMTITLE)
$(ROMNAME).z64: $(BUILD_DIR)/$(ROMNAME).dfs $(BUILD_DIR)/$(ROMNAME).msym

$(BUILD_DIR)/$(ROMNAME).msym: $(BUILD_DIR)/$(ROMNAME).elf

clean:
	rm -rf $(BUILD_DIR) $(FILESYSTEM_DIR) $(DSO_LIST) $(ROMNAME).z64 

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
	