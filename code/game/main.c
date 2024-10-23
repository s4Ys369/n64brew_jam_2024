#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

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

#include "scene/scene.h"
#include "scene/scenery.h"
//#include "scene/assets.h"

#include "ui/ui.h"

#include "game/game.h"
#include "game/game_states.h"
#include "game/game_controls.h"


const MinigameDef minigame_def = {
    .gamename = "Fall Guys Clone",
    .developername = "zoncabe, float4, s4ys, kaelin",
    .description = "This is a game that exist.",
    .instructions = "Press A to win."
};

Game minigame;


// this variables need to be declared here or in an assets.h header
// or loaded in a different way
// they probably need to be arranged and passed as arguments to the game_setState function

/*
//actor
Actor player = actor_create(0, "rom:/game/pipo.t3dm");
ActorAnimation player_animation = actorAnimation_create(&player);
actorAnimation_init(&player, &player_animation);

//scenery
Scenery room = scenery_create(0, "rom:/game/testLevel.t3dm");

Scenery n64logo = scenery_create(0, "rom:/game/n64logo.t3dm");
*/


void minigame_init()
{
	game_init(&minigame);
}

void minigame_loop()
{	
	game_setControlData(&minigame);
	game_setState(&minigame);
}

void minigame_cleanup()
{
	return;
}
