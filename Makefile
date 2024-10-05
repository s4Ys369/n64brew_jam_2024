ROMNAME = gamejam2024
BUILD_DIR = build
MINIGAME_DIR = code
FILESYSTEM_DIR = filesystem
include $(N64_INST)/include/n64.mk

SRC = main.c core.c minigame.c games.c

MINIGAMES_LIST = $(foreach dir, $(sort $(dir $(wildcard $(MINIGAME_DIR)/*/))), $(shell basename $(dir)))

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

$(DSO_LIST): MINIGAME_FILES=$(wildcard $(MINIGAME_DIR)/$(basename $(notdir $@))/*.c)
$(DSO_LIST): $(MINIGAME_FILES:%.c=$(BUILD_DIR)/%.o)
	echo "Building $(MINIGAME_FILES)"

$(ROMNAME).z64: N64_ROM_TITLE="N64BREW GAMEJAM 2024"
$(ROMNAME).z64: $(BUILD_DIR)/$(ROMNAME).dfs $(BUILD_DIR)/$(ROMNAME).msym

$(BUILD_DIR)/$(ROMNAME).msym: $(BUILD_DIR)/$(ROMNAME).elf

clean:
	rm -rf $(BUILD_DIR) $(DSO_LIST) $(ROMNAME).z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
	