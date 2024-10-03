ROMNAME = gamejam2024
BUILD_DIR = build
include $(N64_INST)/include/n64.mk

SRC = main.c minigame.c games.c core.c

ifeq ($(DEBUG), 1)
	N64_CFLAGS += -g -O0
	N64_LDFLAGS += -g
else
	N64_CFLAGS += -O2
endif

all: $(ROMNAME).z64

-include $(wildcard $(BUILD_DIR)/*.d)

$(BUILD_DIR)/$(ROMNAME).elf: $(SRC:%.c=$(BUILD_DIR)/%.o)

$(ROMNAME).z64: N64_ROM_TITLE="N64BREW GAME JAM 2024"

clean:
	rm -rf $(BUILD_DIR) $(ROMNAME).z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
	