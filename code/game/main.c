#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#define ACTOR_COUNT 2
#define PLAYER_COUNT 2
#define SCENERY_COUNT 2
#define ANIMATED_SCENERY_COUNT 1

#define S4YS 0
#define WOLFIE 1
#define MEW 2
#define DOGMAN 3

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

#include "player/player.h"

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

Player player[PLAYER_COUNT];

Actor actors[ACTOR_COUNT];

ActorCollider actor_collider = {

    settings: {
        body_radius: 35.0f,
        body_height: 190.0f,
    }
};

ActorContactData actor_contact;

Scenery scenery[SCENERY_COUNT];
Scenery animated_scenery[ANIMATED_SCENERY_COUNT];


void minigame_init()
{      
	game_init(&minigame);

    display_set_fps_limit(30.0f);

    level_parse("rom:/game/levels/levelA.txt", &box_colliders, &shapeData);

    // actors
    actors[0] = actor_create(0, "rom:/game/dogman.t3dm");
    actors[1] = actor_create(1, "rom:/game/mew.t3dm");

    for (uint8_t i = 0; i < ACTOR_COUNT; i++) {
        actor_init(&actors[i]);
    }

    actorCollider_init(&actor_collider);
    
	// scenery
    scenery[0] = scenery_create(0, "rom:/game/room.t3dm");
    scenery[1] = scenery_create(1, "rom:/game/lava.t3dm");

    for (uint8_t i = 0; i < SCENERY_COUNT; i++)
	{
        scenery_set(&scenery[i]);
    }
}


void minigame_fixedloop()
{
	game_play(&minigame, player, actors, scenery, &actor_collider, &actor_contact, box_colliders, shapeData.numShapes);
}


void minigame_loop()
{
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
	for (uint8_t i = 0; i < ACTOR_COUNT; i++) {

		actor_delete(&actors[i]);
	};

    for (uint8_t i = 0; i < SCENERY_COUNT; i++) {

		scenery_delete(&scenery[i]);
	}
    t3d_destroy(); // Then destroy library

    // Step 5: Free allocated surface buffers
    surface_free(&minigame.screen.depthBuffer);
	
    display_close();
}
