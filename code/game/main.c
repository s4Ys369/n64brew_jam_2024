#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include <stdbool.h>

#define ACTOR_COUNT 1
#define SCENERY_COUNT 19

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

#include "scene/scene.h"
#include "scene/scenery.h"

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

Game minigame;

Actor actor[ACTOR_COUNT];
ActorCollider actor_collider = {
        settings: {
            body_radius: 30.0f, // Testing large Player capsule
            body_height: 110.0f,
        }
};
ActorContactData actor_contact;

Scenery *scenery = NULL;

void minigame_init()
{      
	game_init(&minigame);

    // actors
    actor[0] = actor_create(0, "rom:/game/wolfie.t3dm");

    for (int i = 0; i < ACTOR_COUNT; i++) {
        actor_init(&actor[i]);
    }


    actorCollider_init(&actor_collider);
    
    // Initialize the scenery objects (batched creation of object, matrices, RSPQ blocks, etc.)
    rspqBlocks = scenery_createBatch(SCENERY_COUNT, "rom:/game/hex_platform.t3dm");

    // Initialize the platforms of a certain model, based on the hexagonal grid layout with desired height
    platform_init_grid(hexagons, batchModel, -100.0f);

    // Now associate the scenery objects with the platforms
    for (int i = 0; i < SCENERY_COUNT; i++)
    {
        // Linking scenery to the corresponding platform
        scenery[i].position = hexagons[i].position;  // Set the position of scenery to match the platform
        scenery_set(&scenery[i]);  // Construct the objects' matrices
    }

}

void minigame_fixedloop(){}

void minigame_loop()
{	
	game_play(&minigame, actor, scenery, &actor_collider, &actor_contact, hexagons[0].collider->boxes, 3);
}
void minigame_cleanup()
{
    destroyShapeFileData(&shapeData); // REMEMBER to destroy shape data when switch levels or ending minigame
    platform_free(hexagons);

	scenery_deleteBatch(scenery, SCENERY_COUNT, rspqBlocks, blockCount);

	for (int i = 0; i < ACTOR_COUNT; i++) {

		actor_delete(&actor[i]);
	};

	t3d_destroy();
    
	return;
}
