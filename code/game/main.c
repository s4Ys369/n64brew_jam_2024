#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include <stdbool.h>

#define ACTOR_COUNT 1
#define SCENERY_COUNT 4

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

#include "scene/scene.h"
#include "scene/scenery.h"

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

Game minigame;

Actor actor[ACTOR_COUNT];
ActorCollider actor_collider = {
        settings: {
            body_radius: 30.0f, // Testing large Player capsule
            body_height: 110.0f,
        }
};
ActorContactData actor_contact;

Scenery scenery[SCENERY_COUNT];

Box box_colliders[SCENERY_COUNT - 1]; // Objects minus room

// Testing making the boxes' collisions larger than the model's 
Vector3 boxSize =  {160.0f, 160.0f, 150.0f};
Vector3 rampSize = {410.0f, 150.0f, 150.0f};

void init_box(Box *box, Vector3 size, Vector3 center, Vector3 rotation) {
        box->size = size;
        box->center = center;
        box->rotation = rotation;
}

void minigame_init()
{      
	game_init(&minigame);

    // actors
    actor[0] = actor_create(0, "rom:/game/pipo.t3dm");

    for (int i = 0; i < ACTOR_COUNT; i++) {
        actor_init(&actor[i]);
    }


    actorCollider_init(&actor_collider);
    
     // scenery
    scenery[0] = scenery_create(0, "rom:/game/testLevel.t3dm");

    // Ceiling Test
    scenery[1] = scenery_create(1, "rom:/game/cube.t3dm");
    scenery[1].position = (Vector3){200.0f, 200.0f, 150.0f};
    scenery[1].scale = (Vector3){1.5f, 1.5f, 1.5f};
    scenery[1].rotation = (Vector3){0.0f, 0.0f, 0.0f};

    // Box Test
    scenery[2] = scenery_create(2, "rom:/game/cube.t3dm");
    scenery[2].position = (Vector3){-170.0f, 200.0f, 60.0f};
    scenery[2].scale = (Vector3){1.5f, 1.5f, 1.5f};
    scenery[2].rotation = (Vector3){0.0f, 0.0f, 0.0f};

    // Slope Test
    scenery[3] = scenery_create(3, "rom:/game/cube.t3dm");
    scenery[3].position = (Vector3){-200.0f, -200.0f, 25.0f};
    scenery[3].scale =    (Vector3){4.0f, 1.5f, 1.5f};
    scenery[3].rotation = (Vector3){0.0f, -30.0f, 20.0f};

    init_box(&box_colliders[0], boxSize, (Vector3){200.0f, 200.0f, 150.0f}, (Vector3){0.0f, 0.0f, 0.0f}); // Ceiling
    init_box(&box_colliders[1], boxSize, (Vector3){-170.0f, 200.0f, 60.0f}, (Vector3){0.0f, 0.0f, 0.0f}); // Box
    init_box(&box_colliders[2], rampSize, (Vector3){-200.0f, -200.0f, 25.0f}, (Vector3){0.0f, 30.0f, -20.0f}); // Slope

    for (int i = 0; i < SCENERY_COUNT; i++) {

        scenery_set(&scenery[i]);
    }

}

void minigame_loop()
{	
	game_play(&minigame, actor, scenery, &actor_collider, &actor_contact, box_colliders);
}
void minigame_cleanup()
{
	for (int i = 0; i < SCENERY_COUNT; i++) {

		scenery_delete(&scenery[i]);
	};

	for (int i = 0; i < ACTOR_COUNT; i++) {

		actor_delete(&actor[i]);
	};

	t3d_destroy();
    
	return;
}
