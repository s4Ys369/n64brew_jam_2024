ROMNAME = gamejam2024
BUILD_DIR = build
MINIGAME_DIR = code
FILESYSTEM_DIR = filesystem
include $(N64_INST)/include/n64.mk

SRC = main.c core.c minigame.c games.c

MINIGAMES_LIST = $(notdir $(wildcard $(MINIGAME_DIR)/*))
DSO_LIST = $(addprefix $(FILESYSTEM_DIR)/, $(addsuffix .dso, $(MINIGAMES_LIST)))

ifeq ($(DEBUG), 1)
	N64_CFLAGS += -g -O0
	N64_LDFLAGS += -g
else
	N64_CFLAGS += -O2
endif

all: $(ROMNAME).z64

-include $(wildcard $(BUILD_DIR)/*.d)

MAIN_ELF_EXTERNS := $(BUILD_DIR)/$(ROMNAME).externs
$(BUILD_DIR)/$(ROMNAME).dfs: $(DSO_LIST)
$(BUILD_DIR)/$(ROMNAME).elf: $(SRC:%.c=$(BUILD_DIR)/%.o) $(MAIN_ELF_EXTERNS)
$(MAIN_ELF_EXTERNS): $(DSO_LIST)

define MINIGAME_template
SRC_$(1) = $$(wildcard $$(MINIGAME_DIR)/$(1)/*.c) $$(wildcard $$(MINIGAME_DIR)/$(1)/*.cpp)
$$(FILESYSTEM_DIR)/$(1).dso: $$(SRC_$(1):%.c=$$(BUILD_DIR)/%.o)
endef

$(foreach minigame, $(MINIGAMES_LIST), $(eval $(call MINIGAME_template,$(minigame))))

$(ROMNAME).z64: N64_ROM_TITLE="N64BREW GAMEJAM 2024"
$(ROMNAME).z64: $(BUILD_DIR)/$(ROMNAME).dfs $(BUILD_DIR)/$(ROMNAME).msym

$(BUILD_DIR)/$(ROMNAME).msym: $(BUILD_DIR)/$(ROMNAME).elf

clean:
	rm -rf $(BUILD_DIR) $(DSO_LIST) $(ROMNAME).z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
	