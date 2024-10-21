# Project-specific assets
ASSETS_LIST += \
	filesystem/game/dialogs/dialog_test.json \
	filesystem/game/levels/saveGameDataTest.json \
	filesystem/game/levels/testLevel.json \
	filesystem/game/capsule.t3dm \
	filesystem/game/n64logo.t3dm \
	filesystem/game/pipo.t3dm \
	filesystem/game/room.t3dm \
	filesystem/game/testLevel.t3dm \
	filesystem/game/black_filled_tile.sprite \
	filesystem/game/black_tile.sprite \
	filesystem/game/capsule_skin.sprite \
	filesystem/game/eye.sprite \
	filesystem/game/green_tile.sprite \
	filesystem/game/grey_tile.sprite \
	filesystem/game/red_tile.sprite \
	filesystem/game/yellow_tile.sprite \
	filesystem/game/ui/control_stick.ia8.sprite \
	filesystem/game/ui/d_pad_triggers.ia8.sprite \
	filesystem/game/ui/c_buttons0.rgba32.sprite \
	filesystem/game/ui/c_buttons1.rgba32.sprite \
	filesystem/game/ui/face_buttons0.rgba32.sprite \
	filesystem/game/ui/face_buttons1.rgba32.sprite \
	filesystem/game/chunkysans.font64 \
	filesystem/game/TitanOne-Regular.font64

# t3d flags
filesystem/game/capsule.t3dm: T3DM_FLAGS = --base-scale=1
filesystem/game/n64logo.t3dm: T3DM_FLAGS = --base-scale=1
filesystem/game/pipo.t3dm: T3DM_FLAGS = --base-scale=1
filesystem/game/room.t3dm: T3DM_FLAGS = --base-scale=1 --bvh
filesystem/game/testLevel.t3dm: T3DM_FLAGS = --base-scale=1200 --bvh

# font64 flags
filesystem/game/chunkysans.font64: MKFONT_FLAGS += --outline 2 --size 12
filesystem/game/TitanOne-Regular.font64: MKFONT_FLAGS += --outline 1 --size 12

# Add rule for copying JSONs
$(FILESYSTEM_DIR)/%.json: $(ASSETS_DIR)/%.json
	@mkdir -p $(dir $@)
	@echo "    [JSON] $@"
	@cp $< $@