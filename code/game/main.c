#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#define ACTOR_COUNT 4
#define SCENERY_COUNT 1

#include "../../core.h"
#include "../../minigame.h"

#include "txt/shapeParser.h"

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

#include "actor/collision/actor_collision_detection.h"
#include "actor/collision/actor_collision_response.h"

#include "players/players.h"

#include "scene/scene.h"
#include "scene/scenery.h"

#include "sound/sound.h"

#include "ui/ui.h"

#include "game/game.h"
#include "game/game_control.h"
#include "game/game_states.h"

#include "objects/level.h"
#include "objects/platform.h"


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
ActorCollider actor_collider = {
        settings: {
            body_radius: 30.0f, // Testing large Player capsule
            body_height: 110.0f,
        }
};
ActorContactData actor_contact;

Scenery scenery[SCENERY_COUNT];

void minigame_init()
{      
	game_init(&minigame);

    // actors

    for (int i = 0; i < ACTOR_COUNT; i++) {
        actors[i] = actor_create(i, "rom:/game/s4ys.t3dm");
        actor_init(&actors[i]);
    }


    actorCollider_init(&actor_collider);
    
	// scenery
    scenery[0] = scenery_create(0, "rom:/game/hex_platform.t3dm");

    for (int i = 0; i < SCENERY_COUNT; i++)
	{
        scenery_set(&scenery[i]);
    }

	for (size_t p = 0; p < ACTOR_COUNT; ++p)
	{
		player_init(p, actors[p], &players[p]);
	}

    platform_init(&hexagon, scenery[0].model);


}

void minigame_fixedloop()
{
	for (int i = 0; i < core_get_playercount(); i++)
		controllerData_getInputs(players[i].port, minigame.control[i]);

}

void minigame_loop()
{
	game_play(&minigame, actors, scenery, players, &actor_collider, &actor_contact, hexagon.collider->boxes, 3);
}
void minigame_cleanup()
{
    //destroyShapeFileData(&shapeData); // REMEMBER to destroy shape data when switch levels or ending minigame
    platform_free(&hexagon);

    // Step 1: Free subsystems
    sound_cleanup();
    ui_cleanup();

    // Step 2: Flush and execute all remaining RSPQ commands
    rspq_flush();
    rspq_wait();

    // Step 3: Destroy Tiny3D models, matrices, animations and RSPQ blocks
	for (int i = 0; i < ACTOR_COUNT; i++) {

		actor_delete(&actors[i]);
	};

    for (int i = 0; i < SCENERY_COUNT; i++) {

		scenery_delete(&scenery[i]);
	}
    t3d_destroy(); // Then destroy library

    // Step 4: Free controller data
    for (size_t p = 0; p < MAXPLAYERS; ++p)
	{
		free(minigame.control[p]);
	}

    // Step 5: Free allocated surface buffers
    surface_free(&minigame.screen.depthBuffer);
	display_close();
}
