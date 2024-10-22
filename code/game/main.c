#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include "../../core.h"
#include "../../minigame.h"

#include "screen/screen.h"
#include "control/controls.h"
#include "time/time.h"

#include "physics/physics.h"

#include "camera/camera.h"
#include "camera/camera_states.h"
#include "camera/camera_control.h"

#include "actor/actor.h"
#include "actor/actor_states.h"
#include "actor/actor_motion.h"
#include "actor/actor_control.h"
#include "actor/actor_animation.h"

#include "scene/scenery.h"

#include "sound/sound.h"

#include "ui/ui.h"

#include "game_states.h"

const MinigameDef minigame_def = {
    .gamename = "Fall Guys Clone",
    .developername = "zoncabe, float4, s4ys, kaelin",
    .description = "This is a game that exist.",
    .instructions = "Press A to win."
};

Screen screen;
ControllerData control;
TimeData timing;

void minigame_init()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

	dfs_init(DFS_DEFAULT_LOCATION);
	rdpq_init();

	screen_init(&screen);

	joypad_init();

	time_init(&timing);
	ui_init();
	sound_load();
}
void minigame_loop(float deltatime)
{
	uint8_t game_state = GAMEPLAY;
	game_setState(game_state, &screen, &timing, &control);
}
void minigame_cleanup()
{
	t3d_destroy();
	sound_xm_stop();
	return;
}
