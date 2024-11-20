#ifndef GAME_STATES_H
#define GAME_STATES_H

// Comment out to disable RSPQ Profiling
//#define PROFILING

#ifdef PROFILING
#include "rspq_profile.h"
static rspq_profile_data_t profile_data;
#endif

// function prototypes

void gameState_setIntro(Game* game, Player* player, Scenery* scenery);
void gameState_setMainMenu(Game* game, Player* player, Actor* actor, Scenery* scenery);
void gameState_setCS(Game* game, Player* player, Actor* actor, Scenery* scenery);

void gameState_setGameplay(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* boxes);
void gameState_setPause(Game* game, Player* player, Actor* actor, Scenery* scenery);

void gameState_setGameOver();

void game_play(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* boxes);

static uint32_t frameCounter = 0;


// new camera code ////

void camera_getMinigamePosition(Camera* camera, Actor* actor, Vector3 camera_distance)
{
	Vector3 actor_distance;

    camera->target = actor[0].body.position;
	vector3_init(&camera->position);
	vector3_init(&actor_distance);

    for (uint8_t i = 0; i < ACTOR_COUNT - 1; i++)
    {	
		actor_distance = vector3_difference(&actor[i + 1].body.position, &actor[i].body.position);
        vector3_addScaledVector(&camera->target, &actor_distance, 0.5f);
    }

    camera->position = camera->target;
    
    vector3_add(&camera->position, &camera_distance);
}

//////////////////////


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
	ui_fps(game->timing.frame_rate);

	rdpq_detach_show();
	sound_update();
}

void gameState_setMainMenu(Game* game, Player* player, Actor* actor, Scenery* scenery)
{

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
	ui_fps(game->timing.frame_rate);

	rdpq_detach_show();
	sound_update();

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

	selectedActor->body.position.x = -50.0f;
	selectedActor->body.position.y = -800.0f;
	selectedActor->body.rotation.z -= 3.0f;

	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		actor_update(&actor[i], NULL, &game->timing, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
		
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

	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		player[i].position = actor[i].body.position;
		ui_print_playerNum(&player[i], &game->screen);
	}

	ui_fps(game->timing.frame_rate);

	rdpq_detach_show();
	sound_update();
}

void gameState_setGameplay(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* boxes)
{

	frameCounter++;
	static bool actorSet = false;
	if (!actorSet)
	{
		actor[0].body.position = hexagons[3].position;
		actor[1].body.position = hexagons[6].position;
		actor[2].body.position = hexagons[12].position;
		actor[3].body.position = hexagons[15].position;
		for (size_t i = 0; i < ACTOR_COUNT; i++) 
		{
			actor[i].body.position.z = actor[i].body.position.z + 100.0f;
			actor[i].home = actor[i].body.position;
		}
		actorSet ^= 1;
	}

	// AI
	static bool winnerSet = false;
	for (size_t i = 0; i < AI_COUNT; i++)
	{
		if(player[i+PLAYER_COUNT].died) continue;
		if(winnerSet) continue;
		ai_generateControlData(&ai[i], &player[i+PLAYER_COUNT].control, &actor[i+PLAYER_COUNT], hexagons, PLATFORM_COUNT, game->scene.camera.offset_angle);
	}

	// Actors
	uint8_t loserCount = 0;
	static uint8_t winnerID = 0;
	uint8_t aliveCount = 0;
    uint8_t lastAlivePlayer = 0;

	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		player[i].position = actor[i].body.position;
		if (actor[i].state != DEATH)
		{
			
			if(!winnerSet)
			{
				aliveCount++;
				lastAlivePlayer = i; // Track the last alive player
				actor_update(&actor[i], &player[i].control, &game->timing, game->scene.camera.angle_around_barycenter,
						game->scene.camera.offset_angle, &game->syncPoint);
				actorCollision_updateBoxes(&actor[i], &actor_contact[i], &actor_collider[i], boxes, PLATFORM_COUNT * 3);
			}

		} else {

			static bool rumbled[MAXPLAYERS] = {false}; 
			static int8_t timer[MAXPLAYERS] = {0};   
			if (!rumbled[i])
			{
				controllerData_rumbleFrames(&player[i].control, i, 5);
				if (++timer[i] > 25) rumbled[i] = true;
			} else {
				controllerData_rumbleStop(&player[i].control, i);
			}

			player[i].died = true;
			loserCount++;
		}
	}

	// Check if we have a winner (only one alive player left)
	if (aliveCount == 1 && !winnerSet)
	{
		core_set_winner(lastAlivePlayer); // Set the winner to the last remaining player
		winnerID = lastAlivePlayer;
		winnerSet = true;
	}

	// Platforms
	for (size_t j = 0; j < PLATFORM_COUNT; j++)
	{
		platform_loop(&hexagons[j], actor);
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

	static int8_t winTimer = 0;
	if(loserCount == 3)
	{
		if(winnerSet)
		{
			winTimer++;
			if(winTimer < 120) ui_print_winner(winnerID+1);
			if(winTimer >= 118) game->state = GAME_OVER;
		}
	} else if(loserCount > 3) {
		winTimer++;
		if(winTimer < 120) ui_print_winner(5);
		if(winTimer >= 118) game->state = GAME_OVER;
	}

	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		ui_print_playerNum(&player[i], &game->screen);
	}
	ui_fps(game->timing.frame_rate);

	rdpq_detach_show();
	sound_update();

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


void gameState_setPause(Game* game, Player* player, Actor* actor, Scenery* scenery)
{

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

	ui_pause(&player[0].control);
	ui_fps(game->timing.frame_rate);

	rdpq_detach_show();
	sound_update();

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
    minigame_end();
}

void game_play(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* boxes)
{
	for(;;)
	{
		game_setControlData(game, player);

		time_setData(&game->timing);

		player_setControlData(player);

		static uint8_t camSwitch = 0;

		if(camSwitch == 0)
		{
			camera_getOrbitalPosition(&game->scene.camera, hexagons[1].home, game->timing.fixed_time_s);
		} else {
			camera_getMinigamePosition(&game->scene.camera, actor, (Vector3){0, -600, 800});
		}
		camera_set(&game->scene.camera, &game->screen);

		sound_spatial(&hexagons[1].home, &hexagons[1].home,  &game->scene.camera);

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
				gameState_setMainMenu(game, player, actor, scenery);
				break;
			}
			case CHARACTER_SELECT:{
				gameState_setCS(game, player, actor, scenery);
				break;
			}
			case GAMEPLAY:{
				camSwitch = 1;
				gameState_setGameplay(game, player, ai, actor, scenery, actor_collider, actor_contact, boxes);
				break;
			}
			case PAUSE:{
				gameState_setPause(game, player, actor, scenery);
				break;
			}
			case GAME_OVER:{
				gameState_setGameOver();
				return;
			}
		}
	}

}

#endif