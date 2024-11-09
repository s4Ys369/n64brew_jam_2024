#ifndef GAME_STATES_H
#define GAME_STATES_H

// Comment out to disable RSPQ Profiling
//#define PROFILING

#ifdef PROFILING
#include "rspq_profile.h"
static rspq_profile_data_t profile_data;
#endif


// function prototypes

void gameState_setIntro();
void gameState_setMainMenu();

void gameState_setGameplay(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Platform* platform);
void gameState_setPause(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Platform* platform);

void gameState_setGameOver();

void game_play(Game* game, Actor* actor, Scenery* scenery, PlayerData* players, ActorCollider* actor_collider, ActorContactData* actor_contact, Platform* platform);


// function implementations

void gameState_setIntro()
{
    // code for the intro state
}
void gameState_setMainMenu()
{
    // code for the game over state
}

const char* modelNames[5] = {
    "All",
    "Wolfie",
	"s4ys",
	"Dogman",
	"Mew"
};

static uint32_t frameCounter = 0;
void gameState_setGameplay(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Platform* platform)
{
	
	// ======== Update ======== //

	time_setData(&game->timing);
	frameCounter++;

	for (int i = 0; i < ACTOR_COUNT; i++)
	{
		controllerData_getInputs(player[i].port, game->control[i]);
		if(game->control[i]->pressed.a) sound_wav_bounce();
		actor_update(&actor[i], game->control[i], game->timing.frame_time_s, game->scene.camera[0].angle_around_barycenter, game->scene.camera[0].offset_angle, &game->syncPoint);
		actor_updateMat(&actor[i]);

		// Temp for testing Jump animations
		if (actor[i].body.position.z != 225.0f // magic number
		&& actor[i].state != JUMP
		&& actor[i].state != FALLING)
		{
			
			actor[i].state = FALLING;
			actor[i].grounded = false;
			actor[i].grounding_height = 225.0f;
        
		}
	}

	// CAM SWITCH TEST
	static uint8_t camSwitch = 0;
	const Vector3 camCenter = (Vector3){-200.0f,-200.0f,300.0f};
	static Vector3 camOrbit; 
	static Vector3 targetPosition;

	if (game->control[0]->pressed.b)
	{
		if(camSwitch < 4)
		{
			camSwitch += 1;
		} else {
			camSwitch = 0;
		}
	}

	targetPosition = (camSwitch == 0) ? camCenter : actor[camSwitch-1].body.position;

	if (camOrbit.x != targetPosition.x || camOrbit.y != targetPosition.y || camOrbit.z != targetPosition.z)
		camOrbit = vector3_lerp(&camOrbit, &targetPosition, 0.2f);

	camera_getMinigamePosition(&game->scene.camera[0], camOrbit, game->timing.frame_time_s);
	cameraControl_setOrbitalMovement(&game->scene.camera[0], game->control[0]);

	/* @TODO: Collisions need lots of work, CPU bottleneck

	Box allBoxes[90];
    size_t boxIndex = 0;

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 3; j++) {
            allBoxes[boxIndex++] = platform[i].collider->boxes[j];
        }
    }

	for (int i = 0; i < ACTOR_COUNT; i++)
	{
		actorCollision_updateBoxes(&actor[i], &actor_contact[i], &actor_collider[i], allBoxes, boxIndex);
		// Checks for actor collision with T3D AABB and assigns color
		for (int j = 0; j < NUM_HEXAGONS; j++)
		{
			platform_collideCheck(&hexagons[j], &actor[i]);
		}
	}

	for (int j = 0; j < NUM_HEXAGONS; j++)
	{
		platform_despawn(j, &hexagons[j]);
		despawn[j] = hexagons[j].despawned;
	}

	*/

	platform_getColor(hexagons);
	

	// Set projection and look at for each viewport based on player camera
	camera_set(&game->scene.camera[0], &game->screen.gameplay_viewport[0]);


	// ======== Draw 3D ======== //
	screen_clearDisplay(&game->screen);

	// Clear and attach player viewport
	screen_clearT3dViewport(&game->screen.gameplay_viewport[0]);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

		//scenery_drawBatch(rspqBlocks, blockCount);

		scenery_drawBatch2(scenery, SCENERY_COUNT, platformColor, despawn);

		// Draw updated players
		for (int i = 0; i < ACTOR_COUNT; i++) actor_draw(&actor[i]);

	t3d_matrix_pop(1);
	

	game->syncPoint = rspq_syncpoint_new();

	// ======== Draw 2D ======== //

	//rdpq_sync_pipe();
	//rdpq_set_mode_standard();
	//rdpq_set_scissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	ui_fps();
	ui_printf("Press B to Switch Camera Focus\n"
				"Current: %s", 
				modelNames[camSwitch]);

	rdpq_detach_show();
	sound_update_buffer();

#ifdef PROFILING
	rspq_profile_next_frame();
	if(frameCounter > 29)
	{
		rspq_profile_dump();
		rspq_profile_reset();
		frameCounter = 0;
	}
    rspq_profile_get_data(&profile_data);
#endif // PROFILING

}


void gameState_setPause(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Platform* platform)
{
	// ======== Update ======== //

	time_setData(&game->timing);
	controllerData_getInputs(player[0].port, game->control[0]);

	cameraControl_setOrbitalMovement(&game->scene.camera[0], game->control[0]);
	camera_getMinigamePosition(&game->scene.camera[0], actor[0].body.position, game->timing.frame_time_s);
	camera_set(&game->scene.camera[0], &game->screen.gameplay_viewport[0]);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen.gameplay_viewport[0]);

	light_set(&game->scene.light);

	scenery_drawBatch(rspqBlocks, blockCount);

	game->syncPoint = rspq_syncpoint_new();

	rdpq_detach_show();
	sound_update_buffer();
}


void gameState_setGameOver()
{
    // code for the game over state
}

void game_play(Game* game, Actor* actor, Scenery* scenery, PlayerData* players, ActorCollider* actor_collider, ActorContactData* actor_contact, Platform* platform)
{
	for(;;)
	{
		game_setControlData(game);
		switch(game->state)
		{
			case INTRO:{
				gameState_setIntro();
				break;
			}
			case MAIN_MENU:{
				gameState_setMainMenu();
				break;
			}
			case GAMEPLAY:{
				gameState_setGameplay(game, actor, scenery, players, actor_collider, actor_contact, platform);
				break;
			}
			case PAUSE:{
				gameState_setPause(game, actor, scenery, players, actor_collider, actor_contact, platform);
				break;
			}
			case GAME_OVER:{
				gameState_setGameOver();
				break;
			}
		}
	}

}

#endif