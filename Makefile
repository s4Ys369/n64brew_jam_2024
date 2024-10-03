ROMNAME = gamejam2024
BUILD_DIR = build
include $(N64_INST)/include/n64.mk

SRC = main.c core.c minigame.c
SRC_MINIGAMES = $(wildcard code/*/*.c) # Needs to be split into DSOs

SRC += $(SRC_MINIGAMES)

ifeq ($(DEBUG), 1)
	N64_CFLAGS += -g -O0
	N64_LDFLAGS += -g
else
	N64_CFLAGS += -O2
endif

all: $(ROMNAME).z64

-include $(wildcard $(BUILD_DIR)/*.d)

$(BUILD_DIR)/$(ROMNAME).elf: $(SRC:%.c=$(BUILD_DIR)/%.o)

$(ROMNAME).z64: N64_ROM_TITLE="N64BREW GAMEJAM 2024"

clean:
	rm -rf $(BUILD_DIR) $(ROMNAME).z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
	