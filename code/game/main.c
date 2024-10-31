#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include <stdbool.h>

#define ACTOR_COUNT 1
#define SCENERY_COUNT 13

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
ShapeFileData shapeData = {0};

Box* box_colliders;

// Testing making the boxes' collisions larger than the model's 
Vector3 boxSize =  {160.0f, 160.0f, 150.0f};
Vector3 rampSize = {410.0f, 150.0f, 150.0f};

void init_box(Box *box, Vector3 size, Vector3 center, Vector3 rotation, float scalar)
{
    box->size = vector3_returnScaled(&size, scalar);
    box->center = vector3_returnScaled(&center, scalar);
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

    if (parseFile("rom:/game/levels/testLevel.txt", &shapeData))
    {
        box_colliders = (Box *)malloc(sizeof(Box) * shapeData.numShapes);
        for (size_t shapes = 0; shapes < shapeData.numShapes; ++shapes) 
        {
            scenery[shapes + 1] = scenery_create(shapes + 1, "rom:/game/cube.t3dm");

            // Parsing and conversion for position, scale, and rotation
            Vector3 tempPos = vector3_from_array(shapeData.shapes[shapes].info.pos);
            Vector3 tempScale = vector3_from_array(shapeData.shapes[shapes].info.dim);
            Vector3 tempRot = vector3_from_array(shapeData.shapes[shapes].info.rot);
            Vector3 pos = vector3_flip_coords(vector3_returnScaled(&tempPos, -500.0f));
            Vector3 scale = vector3_flip_coords(vector3_returnScaled(&tempScale, 5.0f));
            Vector3 rot = vector3_flip_coords(vector3_returnScaled(&tempRot, 100.0f));

            // Applying transformed values to scenery and box collider
            scenery[shapes + 1].position = pos;
            scenery[shapes + 1].scale = scale;
            scenery[shapes + 1].rotation = rot;

            init_box(&box_colliders[shapes], pos, scale, vector3_returnScaled(&tempRot, 100.0f), 1.0f);
        }
        parsePrint(&shapeData);
    }


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
	for (int i = 0; i < SCENERY_COUNT; i++) {

		scenery_delete(&scenery[i]);
	};

	for (int i = 0; i < ACTOR_COUNT; i++) {

		actor_delete(&actor[i]);
	};

	t3d_destroy();
    
	return;
}
