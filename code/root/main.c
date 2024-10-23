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

#include "ui/ui.h"

#include "game/game.h"
#include "game/game_states.h"


const MinigameDef minigame_def = {
    .gamename = "Fall Guys Clone",
    .developername = "zoncabe, float4, s4ys, kaelin",
    .description = "This is a game that exist.",
    .instructions = "Press A to win."
};

Game minigame = {
	.state = GAMEPLAY
};

void minigame_init()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

	dfs_init(DFS_DEFAULT_LOCATION);
	rdpq_init();
	joypad_init();
	t3d_init((T3DInitParams){});

	joypad_init();

	time_init(&minigame.timing);
	ui_init();

	game_init(&minigame, MAIN_MENU);
	gameState_set(&minigame, MAIN_MENU, core_get_playercount(), 2, NULL, NULL);

}

void minigame_fixedloop(float deltaTime)
{
    gameState_setPaused(&minigame);
	game_fixedUpdate(&minigame, deltaTime);
}

void minigame_loop(float deltaTime)
{	
	game_loop(&minigame, deltaTime);	
	rdpq_detach_show();
}

void minigame_cleanup()
{
	game_cleanup(&minigame);
	t3d_destroy();
	// TODO: find and free everything dangling
	display_close();
}
