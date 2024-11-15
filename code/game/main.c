#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#define ACTOR_COUNT 4
#define PLAYER_COUNT 4

#define SCENERY_COUNT 2
#define PLATFORM_COUNT 19

#define S4YS 0
#define WOLFIE 1
#define MEW 2
#define DOGMAN 3

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

#include "actor/collision/actor_collision_detection.h"
#include "actor/collision/actor_collision_response.h"

#include "player/player.h"

#include "scene/scene.h"
#include "scene/scenery.h"
#include "scene/platform.h"
#include "scene/room.h"

#include "player/ai.h"

#include "sound/sound.h"

#include "ui/ui.h"

#include "game/game.h"
#include "game/game_control.h"
#include "game/game_states.h"


const MinigameDef minigame_def = {
    .gamename = "Hot Hot Hexagons",
    .developername = "Strawberry Byte: zoncabe, s4ys",
    .description = "The floor is lava!",
    .instructions = "Jump from platform to platform\nto avoid a terrible fate."
};

Game minigame = {
	.state = INTRO
};

Player player[PLAYER_COUNT];

AI aiPlayer[PLAYER_COUNT];

Actor actors[ACTOR_COUNT];

ActorCollider actor_collider[ACTOR_COUNT];

ActorContactData actor_contact[ACTOR_COUNT];

Scenery scenery[SCENERY_COUNT];


void minigame_init()
{      
	game_init(&minigame);

    display_set_fps_limit(30.0f); // @TODO: There's a CPU race condition for multiple actor collisions, why the limiter is required

    // actors
    actors[0] = actor_create(0, "rom:/game/dogman.t3dm");
    actors[1] = actor_create(1, "rom:/game/mew.t3dm");
    actors[2] = actor_create(2, "rom:/game/wolfie.t3dm");
    actors[3] = actor_create(3, "rom:/game/s4ys.t3dm");

    for (uint8_t i = 0; i < ACTOR_COUNT; i++) {
        actor_init(&actors[i]);
        actorCollider_init(&actor_collider[i]);
        actor_collider[i].settings.body_radius = 35.0f;
        actor_collider[i].settings.body_height = 190.f;
        actors[i].body.position.z = 300.0f;

        // Individual Character Placement
        switch(i)
        {
            case 0:
                actors[i].body.position.x = -300.0f;
                break;
            case 1:
                actors[i].body.position.x = -150.0f;
                break;
            case 2:
                break;
            case 3:
                actors[i].body.position.x = 150.0f;
                break;
        }
    }

    // AI
    for (uint8_t i = 1; i < ACTOR_COUNT; i++) {
        ai_init(&aiPlayer[i], core_get_aidifficulty());
    }
    
	// scenery
    scenery[0] = scenery_create(0, "rom:/game/room.t3dm");
    scenery[1] = scenery_create(1, "rom:/game/lava.t3dm");

    for (uint8_t i = 0; i < SCENERY_COUNT; i++)
	{
        scenery_set(&scenery[i]);
    }

    // platforms
    platform_hexagonGrid(hexagons, t3d_model_load("rom:/game/platform.t3dm"), 250.0f, ui_color(N_YELLOW));

}



void minigame_fixedloop()
{
	game_play(&minigame, player, aiPlayer, actors, scenery, actor_collider, actor_contact);
}


void minigame_loop()
{
}

void minigame_cleanup()
{
    //destroyShapeFileData(&shapeData); // REMEMBER to destroy shape data when switch levels or ending minigame

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
    platform_destroy(hexagons);
    t3d_destroy(); // Then destroy library

    // Step 5: Free allocated surface buffers
    surface_free(&minigame.screen.depthBuffer);
	
    display_close();
}
