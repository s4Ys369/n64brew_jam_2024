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
	filesystem/game/mui.sprite \
	filesystem/game/red_tile.sprite \
	filesystem/game/yellow_tile.sprite \
	filesystem/game/chunkysans.font64 \
	filesystem/game/TitanOne-Regular.font64

# t3d flags
filesystem/game/capsule.t3dm: GLTF_FLAGS = --base-scale=1
filesystem/game/n64logo.t3dm: GLTF_FLAGS = --base-scale=1
filesystem/game/pipo.t3dm: GLTF_FLAGS = --base-scale=1
filesystem/game/room.t3dm: GLTF_FLAGS = --base-scale=1 --bvh
filesystem/game/testLevel.t3dm: GLTF_FLAGS = --base-scale=1200 --bvh


# Add rule for copying JSONs
$(FILESYSTEM_DIR)/%.json: $(ASSETS_DIR)/%.json
	@mkdir -p $(dir $@)
	@echo "    [JSON] $@"
	@cp $< $@