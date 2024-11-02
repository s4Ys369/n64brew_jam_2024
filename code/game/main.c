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
ShapeFileData shapeData = {0};

Box* box_colliders;

void init_box(Box *box, Vector3 size, Vector3 center, Vector3 rotation, float scalar)
{
    box->size = vector3_returnScaled(&size, scalar);
    box->center = vector3_returnScaled(&center, scalar);
    box->rotation = rotation;
}

// The bullet exporter stores rotations as quaternions, but Tiny3D and the physics engine uses Euler Angles
Vector3 quaternion_to_euler(Quaternion q) {
    Vector3 euler;

    // Roll (X-axis rotation)
    float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    euler.x = atan2f(sinr_cosp, cosr_cosp);

    // Pitch (Y-axis rotation)
    float sinp = 2 * (q.w * q.y - q.z * q.x);
    if (fabs(sinp) >= 1)
        euler.y = copysignf(M_PI / 2, sinp);  // Use 90 degrees if out of range
    else
        euler.y = asinf(sinp);

    // Yaw (Z-axis rotation)
    float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    euler.z = atan2f(siny_cosp, cosy_cosp);

    return euler;
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
    scenery[0] = scenery_create(0, "rom:/game/peeweeplayhouse.t3dm");

    if (parseFile("rom:/game/levels/peeweeplayhouse.txt", &shapeData))
    {
        box_colliders = (Box *)malloc(sizeof(Box) * shapeData.numShapes);
        for (size_t shapes = 0; shapes < shapeData.numShapes; ++shapes) 
        {

            // Parsing and conversion for position, scale, and rotation
            Vector3 tempPos = vector3_from_array(shapeData.shapes[shapes].info.pos);
            Vector3 tempScale = vector3_from_array(shapeData.shapes[shapes].info.dim);
            Quaternion tempRot = quat_from_array(shapeData.shapes[shapes].info.rot);
            Vector3 pos = vector3_flip_up(vector3_returnScaled(&tempPos, 2.0f));     // returnScaled is only necessary when using `base-scale=` with t3d
            Vector3 scale = vector3_flip_up(vector3_returnScaled(&tempScale, 2.0f)); // returnScaled is only necessary when using `base-scale=` with t3d
			Vector3 rot = quaternion_to_euler(tempRot);

            // To match the input for the physics engine, switch to Z up, and convert rotations radians to degrees
			Vector3 colRot = vector3_flip_up(rot);
            colRot.x = deg(colRot.x);
            colRot.y = deg(colRot.y);
            colRot.z = deg(colRot.z);

			/*Optional Visual Display*/
			//scenery[shapes + 1] = scenery_create(shapes + 1, "rom:/game/cube.t3dm");
            //scenery[shapes + 1].position = pos;
            //scenery[shapes + 1].scale = scale;
            //scenery[shapes + 1].rotation = rot;

			// Applying transformed values to box colliders
            init_box(&box_colliders[shapes], scale, pos, colRot, 1.0f);
        }
        //parsePrint(&shapeData);
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
