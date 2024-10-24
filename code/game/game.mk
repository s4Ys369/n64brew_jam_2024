# Project-specific assets
ASSETS_LIST += \
	filesystem/game/levels/levelA.txt \
	filesystem/game/capsule.t3dm \
	filesystem/game/n64logo.t3dm \
	filesystem/game/pipo.t3dm \
	filesystem/game/room.t3dm \
	filesystem/game/testLevel.t3dm \
	filesystem/game/levelA.t3dm \
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
filesystem/game/testLevel.t3dm: GLTF_FLAGS = --base-scale=1 --bvh
filesystem/game/levelA.t3dm: GLTF_FLAGS = --base-scale=100 --bvh

# font64 flags
filesystem/game/chunkysans.font64: MKFONT_FLAGS += --outline 2 --size 12
filesystem/game/TitanOne-Regular.font64: MKFONT_FLAGS += --outline 1 --size 12

# Add rule for copying TXTs
$(FILESYSTEM_DIR)/%.txt: $(ASSETS_DIR)/%.txt
	@mkdir -p $(dir $@)
	@echo "    [TXT] $@"
	@cp $< $@