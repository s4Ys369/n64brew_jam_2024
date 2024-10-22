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

#include "players/players.h"

#include "scene/scenery.h"

#include "ui/ui.h"

#include "game_states.h"

const MinigameDef minigame_def = {
    .gamename = "Fall Guys Clone",
    .developername = "zoncabe, float4, s4ys, kaelin",
    .description = "This is a game that exist.",
    .instructions = "Press A to win."
};

Screen screen;
ControllerData *control[MAXPLAYERS];
TimeData timing;

PlayerData *players[MAXPLAYERS];

void minigame_init()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

	dfs_init(DFS_DEFAULT_LOCATION);
	rdpq_init();

	screen_initDisplay(&screen);

	joypad_init();

	time_init(&timing);
	ui_init();

	for (size_t p = 0; p < MAXPLAYERS; ++p)
	{
		control[p] = malloc(sizeof(ControllerData));
    	players[p] = malloc(sizeof(PlayerData));
	}

}
void minigame_loop(float deltatime)
{
	for(uint32_t p = 0; p < core_get_playercount(); ++p)
	{
    	if (players[p] != NULL)
    	    player_init(players[p]); 
	}
	uint8_t game_state = GAMEPLAY;
	game_setState(game_state, &screen, &timing, control, players);
}
void minigame_cleanup()
{
	if(dpl_Temp)
        rspq_block_free(dpl_Temp);
	return;
}
