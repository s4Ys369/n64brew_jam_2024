#ifndef GAME_STATES_H
#define GAME_STATES_H

// Comment out to disable RSPQ Profiling
#define PROFILING

#ifdef PROFILING
#include "rspq_profile.h"
static rspq_profile_data_t profile_data;
#endif

// function prototypes

void gameState_setIntro(Game* game, Player* player, Scenery* scenery);
void gameState_setMainMenu();
void gameState_setCS(Game* game, Player* player, Actor* actor, Scenery* scenery);

void gameState_setGameplay(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* boxes);
void gameState_setPause(Game* game, Player* player, Actor* actor, Scenery* scenery);

void gameState_setGameOver();

void game_play(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* boxes);


void gameState_setIntro(Game* game, Player* player, Scenery* scenery)
{

	for (size_t j = 0; j < PLATFORM_COUNT; j++)
	{
		
		platform_loop(&hexagons[j], NULL);
	}

	move_lava(scenery);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	room_draw(scenery);

	light_setAmbient(&game->scene.light, 0xBF);
	platform_drawBatch();
	light_resetAmbient(&game->scene.light);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_intro(&player[0].control);
	ui_fps();

	rdpq_detach_show();
	sound_update_buffer();
}

void gameState_setMainMenu()
{
}

void gameState_setCS(Game* game, Player* player, Actor* actor, Scenery* scenery)
{
	for (size_t j = 0; j < PLATFORM_COUNT; j++)
	{
		
		platform_loop(&hexagons[j], NULL);
	}

	static uint8_t selectedCharacter[4] = {0};

	controllerData_8way(&player[0].control);

	if (player[0].control.pressed.d_right)
    	selectedCharacter[0] = (selectedCharacter[0] + 1) % ACTOR_COUNT;

	if (player[0].control.pressed.d_left)
	    selectedCharacter[0] = (selectedCharacter[0] - 1 + ACTOR_COUNT) % ACTOR_COUNT;

	Actor* selectedActor = &actor[selectedCharacter[0]];

	selectedActor->body.position.x = 0.0f;
	selectedActor->body.position.y = -800.0f;
	selectedActor->body.rotation.z -= 3.0f;

	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		actor_update(&actor[i], NULL, game->timing.frame_time_s, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
		
		// Reset non-selected actors
		if (i != selectedCharacter[0])
		{ 
        	actor[i].body.position = actor[i].home;
        	actor[i].body.rotation.z = 0;
    	}
	}

	move_lava(scenery);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	room_draw(scenery);

	light_setAmbient(&game->scene.light, 0xBF);
	platform_drawBatch();
	light_resetAmbient(&game->scene.light);

	actor_draw(actor);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_fps();
	ui_printf("Char %d", selectedCharacter[0]);

	rdpq_detach_show();
	sound_update_buffer();
}

void gameState_setGameplay(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* boxes)
{

	// AI
	for (size_t i = 1; i < ACTOR_COUNT; i++)
	{
		ai_generateControlData(&ai[i], &player[i].control, &actor[i], hexagons, PLATFORM_COUNT, game->scene.camera.offset_angle);
	}

	// Actors
	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		actor_update(&actor[i], &player[i].control, game->timing.frame_time_s, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
		actorCollision_updateBoxes(&actor[i], &actor_contact[i], &actor_collider[i], boxes, PLATFORM_COUNT*3);
	}

	// Platforms
	for (size_t j = 0; j < PLATFORM_COUNT; j++)
	{
		for (size_t i = 0; i < ACTOR_COUNT; i++)
		{
			platform_loop(&hexagons[j], &actor[i]);
		}
	}

	move_lava(scenery);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	room_draw(scenery);

	light_setAmbient(&game->scene.light, 0xBF);
	platform_drawBatch();
	light_resetAmbient(&game->scene.light);

	actor_draw(actor);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_fps();

	rdpq_detach_show();
	sound_update_buffer();
}


void gameState_setPause(Game* game, Player* player, Actor* actor, Scenery* scenery)
{

	static uint32_t frameCounter = 0;
	frameCounter++;
	move_lava(scenery);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	room_draw(scenery);

	light_setAmbient(&game->scene.light, 0xBF);
	platform_drawBatch();
	light_resetAmbient(&game->scene.light);

	actor_draw(actor);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_main_menu(&player[0].control);
	ui_fps();

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


void gameState_setGameOver()
{
    // code for the game over state
}

void game_play(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* boxes)
{
	for(;;)
	{
		game_setControlData(game, player);

		time_setData(&game->timing);

		player_setControlData(player);

		static uint8_t camSwitch = 0;
		if(player[0].control.pressed.b) camSwitch ^= 1;

		Vector3 playersCenter = vector3_average4(&actor[0].body.position, &actor[1].body.position, &actor[2].body.position, &actor[3].body.position);

		Vector3 camFocus = camSwitch ?  playersCenter : hexagons[1].home;

		cameraControl_setOrbitalMovement(&game->scene.camera, &player[0].control);
		camera_getMinigamePosition(&game->scene.camera, camFocus, game->timing.frame_time_s);
		camera_set(&game->scene.camera, &game->screen);

		// Precompute all collision boxes to avoid recomputing them repeatedly.
    	for (size_t i = 0, boxIndex = 0; i < PLATFORM_COUNT; i++) {
    	    memcpy(&boxes[boxIndex], hexagons[i].collider.box, sizeof(Box) * 3);
    	    boxIndex += 3;
    	}

		switch(game->state)
		{
			case INTRO:{
				gameState_setIntro(game, player, scenery);
				break;
			}
			case MAIN_MENU:{
				gameState_setMainMenu();
				break;
			}
			case CHARACTER_SELECT:{
				gameState_setCS(game, player, actor, scenery);
				break;
			}
			case GAMEPLAY:{
				gameState_setGameplay(game, player, ai, actor, scenery, actor_collider, actor_contact, boxes);
				break;
			}
			case PAUSE:{
				gameState_setPause(game, player, actor, scenery);
				break;
			}
			case GAME_OVER:{
				gameState_setGameOver();
				break;
			}
		}
	}

}

void game_playFixed(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* boxes)
{

	
}

#endif