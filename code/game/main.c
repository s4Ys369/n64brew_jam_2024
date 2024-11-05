#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include <stdbool.h>

#define ACTOR_COUNT 1
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

// @TODO: find a better place for this logic
ShapeFileData shapeData = {0};

Box* box_colliders = NULL;

void level_parse(const char *text_path, Box **colliders, ShapeFileData *shapeData)
{
    // If text file is successfully parsed
    if (parseFile(text_path, shapeData))
    {
        // Allocate dynamic shape memory
        *colliders = (Box *)malloc(sizeof(Box) * shapeData->numShapes);

        // @TODO: Add support for spheres
        for (size_t shapes = 0; shapes < shapeData->numShapes; ++shapes) 
        {

            // Parse position, scale, and rotation
            Vector3 tempPos = vector3_from_array(shapeData->shapes[shapes].info.pos);
            Vector3 tempScale = vector3_from_array(shapeData->shapes[shapes].info.dim);
            Quaternion tempRot = quat_from_array(shapeData->shapes[shapes].info.rot);

            // To match the input for the physics engine, switch to Z up, and convert rotations radians to degrees
            Vector3 pos = vector3_flip_up(tempPos);
            Vector3 scale = vector3_flip_up(tempScale);
			Vector3 rot = vector3_fromQuaternion(tempRot);
			Vector3 colRot = vector3_flip_up(rot);
            colRot.x = deg(colRot.x);
            colRot.y = deg(colRot.y);
            colRot.z = deg(colRot.z);

			// Applying transformed values to box colliders
            box_init(&(*colliders)[shapes], scale, pos, colRot, 500.0f); // If using `--base-scale=n` with T3D, enter n here as the scalar
        }
    }
}

void minigame_init()
{      
	game_init(&minigame);
    level_parse("rom:/game/levels/testLevel.txt", &box_colliders, &shapeData);

    // actors
    actor[0] = actor_create(0, "rom:/game/wolfie.t3dm");

    for (int i = 0; i < ACTOR_COUNT; i++) {
        actor_init(&actor[i]);
    }


    actorCollider_init(&actor_collider);
    
    // scenery
    scenery[0] = scenery_create(0, "rom:/game/testLevel.t3dm");

    for (int i = 0; i < SCENERY_COUNT; i++) {

        scenery_set(&scenery[i]);
    }

}

void minigame_fixedloop(){}

void minigame_loop()
{	
	game_play(&minigame, actor, scenery, &actor_collider, &actor_contact, box_colliders, shapeData.numShapes);
}
void minigame_cleanup()
{
    destroyShapeFileData(&shapeData); // REMEMBER to destroy shape data when switch levels or ending minigame

	for (int i = 0; i < SCENERY_COUNT; i++) {

		scenery_delete(&scenery[i]);
	};

	for (int i = 0; i < ACTOR_COUNT; i++) {

		actor_delete(&actor[i]);
	};

	t3d_destroy();
    
	return;
}
