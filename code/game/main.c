#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include <stdbool.h>

#define ACTOR_COUNT 1
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
            body_radius: 15.0f,
            body_height: 100.0f,
        }
};
ActorContactData actor_contact;

Scenery scenery[SCENERY_COUNT];

Box logo_collider = {
        size: {100.0f, 100.0f, 70.0f,},
        center: {200.0f, 200.0f, 50.0f},
        rotation: {0.0f, 0.0f, 45.0f},
};

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
    scenery[1] = scenery_create(1, "rom:/game/n64logo.t3dm");
    scenery[1].position = (Vector3){200, 200, 0};

    for (int i = 0; i < SCENERY_COUNT; i++) {

        scenery_set(&scenery[i]);
    }

}

void minigame_loop()
{	
	game_play(&minigame, actor, scenery, &actor_collider, &actor_contact, &logo_collider);
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
