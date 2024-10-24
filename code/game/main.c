#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#define ACTOR_COUNT 2
#define SCENERY_COUNT 2

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

#include "players/players.h"

#include "scene/scene.h"
#include "scene/scenery.h"

#include "sound/sound.h"

#include "ui/ui.h"

#include "game/game.h"
#include "game/game_control.h"
#include "game/game_states.h"


const MinigameDef minigame_def = {
    .gamename = "Fall Guys Clone",
    .developername = "zoncabe, float4, s4ys",
    .description = "This is a game that exist.",
    .instructions = "Press A to win."
};

Game minigame = {
	.state = GAMEPLAY
};

PlayerData players[MAXPLAYERS];

Actor actors[ACTOR_COUNT];

Scenery scenery[SCENERY_COUNT];

void minigame_init()
{      
	game_init(&minigame);

    // actors
	for (int i = 0; i < ACTOR_COUNT; i++)
	{
		actors[i] = actor_create(i, "rom:/game/pipo.t3dm");
        actor_init(&actors[i]);
    }
    
	// scenery
    scenery[0] = scenery_create(0, "rom:/game/testLevel.t3dm");
    scenery[1] = scenery_create(1, "rom:/game/n64logo.t3dm");
    scenery[1].position = (Vector3){200, 200, 0};

    for (int i = 0; i < SCENERY_COUNT; i++)
	{
        scenery_set(&scenery[i]);
    }

	for (size_t p = 0; p < ACTOR_COUNT; ++p)
	{
		player_init(p, actors[p], &players[p]);
	}

}

void minigame_loop()
{	
	game_play(&minigame, actors, scenery, players);
}
void minigame_cleanup()
{
	t3d_destroy();
	sound_xm_stop();
	display_close();
}
