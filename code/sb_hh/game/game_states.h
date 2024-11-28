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

void gameState_setGameplay(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact);
void gameState_setPause(Game* game, Player* player, Actor* actor, Scenery* scenery);

void gameState_setGameOver();

void game_play(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact);


// new camera code ////

void camera_getMinigamePosition(Camera* camera, Actor* actor, Player* player, Vector3 camera_distance)
{
    Vector3 camera_target;

    uint8_t average_count = 0;

    vector3_init(&camera->position);
    vector3_init(&camera_target);

    for (uint8_t i = 0; i < ACTOR_COUNT; i++)
    {
        if (!player[i].died && player[i].isHuman) {

            vector3_add(&camera_target, &actor[player[i].actor_id].body.position);
            average_count++;
        }
    }

    if (average_count > 0) vector3_divideByNumber(&camera_target, average_count);
    camera_target.z = 200;

    if (camera_target.x != camera->target.x || camera_target.y != camera->target.y)
    camera->target = vector3_lerp(&camera->target, &camera_target, 0.2f);
    
    camera->position = camera->target;
    
    vector3_add(&camera->position, &camera_distance);
}

//////////////////////


void gameState_setIntro(Game* game, Player* player, Scenery* scenery)
{

	for (size_t j = 0; j < PLATFORM_COUNT; j++)
	{
		platform_loop(&hexagons[j], NULL, 0);
	}

	//move_lava(scenery);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);
	screen_applyColor_Depth(&game->screen, RGBA32(154, 181, 198, 0xFF), false);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	scenery_draw(scenery);

	//light_setAmbient(&game->scene.light, 0xFF);
	platform_drawBatch();
	light_resetAmbient(&game->scene.light);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_intro(&player[0].control);

	if(player[0].control.held.r)
	{
		ui_fps(game->timing.frame_rate);
		if(core_get_playercount() == 1) ui_input_display(&player[0].control);
	}

	rdpq_detach_show();
	sound_update();
}


void gameState_setMainMenu(Game* game, Player* player, Actor* actor, Scenery* scenery)
{
	//move_lava(scenery);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);
	screen_applyColor_Depth(&game->screen, RGBA32(154, 181, 198, 0xFF), false);

	light_set(&game->scene.light);
	// Instead drawing a dark transparent texture over the scene, just change the light direction
	game->scene.light.direction1 = (T3DVec3){{0,-1, 0}};

	t3d_matrix_push_pos(1);

	scenery_draw(scenery);

	//light_setAmbient(&game->scene.light, 0xFF);
	platform_drawBatch();
	light_resetAmbient(&game->scene.light);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	if(core_get_playercount() == 4)
	{
		if(player[0].control.pressed.b)
		{
			if(game->diff <= 1)
			{
				game->diff++;
			} else {
				game->diff = 0;
			}
		}
	}
	ui_main_menu(&player[0].control, game->diff);
	if(player[0].control.held.r)
	{
		ui_fps(game->timing.frame_rate);
		if(core_get_playercount() == 1) ui_input_display(&player[0].control);
	}

	rdpq_detach_show();
	sound_update();

#ifdef PROFILING
	rspq_profile_next_frame();
	if(game->timing.frame_counter > 29)
	{
		rspq_profile_dump();
		rspq_profile_reset();
		game->timing.frame_counter = 0;
	}
    rspq_profile_get_data(&profile_data);
#endif // PROFILING
}

void gameState_setCS(Game* game, Player* player, Actor* actor, Scenery* scenery)
{

	static uint8_t activePlayer = 0;
	const uint8_t totalPlayers = core_get_playercount();
	static uint8_t selectedCharacter[ACTOR_COUNT] = {0};
	static bool actorSelected[ACTOR_COUNT] = {false};

	if(activePlayer >= 4){game->state = GAMEPLAY;return;}

	controllerData_8way(&player[activePlayer].control);

	if (player[activePlayer].control.pressed.d_right)
	{
		uint8_t initialSelection = selectedCharacter[activePlayer];
		do {
			selectedCharacter[activePlayer] = (selectedCharacter[activePlayer] + 1) % ACTOR_COUNT;
		} while (actorSelected[selectedCharacter[activePlayer]] && selectedCharacter[activePlayer] != initialSelection);
	}

	if (player[activePlayer].control.pressed.d_left)
	{
		uint8_t initialSelection = selectedCharacter[activePlayer];
		do {
			selectedCharacter[activePlayer] = (selectedCharacter[activePlayer] - 1 + ACTOR_COUNT) % ACTOR_COUNT;
		} while (actorSelected[selectedCharacter[activePlayer]] && selectedCharacter[activePlayer] != initialSelection);
	}

	if (player[activePlayer].control.pressed.a)
	{
		// @TODO: More of a bandaid than a fix
		// Bugfix: Ensure selected actor is next available one
		if(!actorSelected[selectedCharacter[activePlayer]])
		{
			uint8_t selectedActorId = selectedCharacter[activePlayer];
			player[activePlayer].actor_id = selectedActorId;
			player[activePlayer].isHuman = true;
			actorSelected[selectedActorId] = true;

			// Visual feedback for selecting actor
			actor[selectedActorId].body.rotation.z = actor[selectedActorId].body.rotation.z + 180.0f;

			activePlayer++;
		} else {
			// Audio feedback for selecting unavailable actor
			sound_wavPlay(SFX_JUMP, false); 
		}

		// Automatically assign actors to AI players
		if (activePlayer >= totalPlayers)
		{
			for (uint8_t i = totalPlayers; i < 4; i++) // AI players start after human players
			{
				for (uint8_t j = 0; j < ACTOR_COUNT; j++)
				{
					if (!actorSelected[j]) // Assign the first unselected actor
					{
						player[i].actor_id = j;
						player[i].isHuman = false;
						actorSelected[j] = true;
						break;
					}
				}
			}
			activePlayer = 4; // Lock selection
		}

		if(activePlayer >= 4){game->state = GAMEPLAY;return;}
	}

	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		actor_update(&actor[i], NULL, &game->timing, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
		
		// Reset non-selected actors
		if (!actorSelected[i])
		{ 
        	actor[i].body.position = actor[i].home;
        	actor[i].body.rotation.z = 0;
    	}

		actor_updateMat(&actor[i]);
	}

	// Sync RSPQ once, and then update each actors' skeleton
	if(game->syncPoint)rspq_syncpoint_wait(game->syncPoint);
	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		t3d_skeleton_update(&actor[i].armature.main);
	}

	//move_lava(scenery);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);
	screen_applyColor_Depth(&game->screen, RGBA32(154, 181, 198, 0xFF), false);

	light_set(&game->scene.light);

	// Change light direction to illuminate players
	game->scene.light.direction1 = (T3DVec3){{0,-1,0}};
	//game->scene.light.directional_color1[0] = 200;
	//game->scene.light.directional_color1[1] = 200;
	//game->scene.light.directional_color1[2] = 200;

	t3d_matrix_push_pos(1);

	scenery_draw(scenery);

	//light_setAmbient(&game->scene.light, 0xBF);
	platform_drawBatch();
	light_resetAmbient(&game->scene.light);

	actor_draw(actor);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	if(activePlayer < MAXPLAYERS)
	{
		player[activePlayer].position.x = (actor[selectedCharacter[activePlayer]].body.position.x * 3.6f) - 30.0f;
		player[activePlayer].position.z = 125.0f;
		ui_print_playerNum(&player[activePlayer], &game->screen);
	}

	ui_character_select(&player[activePlayer].control, selectedCharacter[activePlayer]);

	if(player[0].control.held.r)
	{
		ui_fps(game->timing.frame_rate);
		if(core_get_playercount() == 1) ui_input_display(&player[0].control);
	}

	rdpq_detach_show();
	sound_update();
}


void gameState_setGameplay(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact)
{

	if (!game->actorSet)
	{
		// Unique spawn positions
		actor[0].body.position = hexagons[3].position;
		actor[1].body.position = hexagons[6].position;
		actor[2].body.position = hexagons[12].position;
		actor[3].body.position = hexagons[15].position;
		for (size_t i = 0; i < ACTOR_COUNT; i++) 
		{
			actor[i].body.position.z = actor[i].body.position.z + 150.0f;
			actor[i].home = actor[i].body.position;
			actor[player[i].actor_id].colorID = i;
		}
		game->actorSet ^= 1;
	}

// ======== Countdown ======== //
    if (game->countdownTimer > 0)
    {
		if(game->countdownTimer % 45 == 0) sound_wavPlay(SFX_JUMP, false);

		//move_lava(scenery);

        // ======== Draw ======== //
		screen_clearDisplay(&game->screen);
		screen_clearT3dViewport(&game->screen);
		screen_applyColor_Depth(&game->screen, RGBA32(154, 181, 198, 0xFF), false);

		light_set(&game->scene.light);

		game->scene.light.direction1 = (T3DVec3){{0,-1,0}};
		//game->scene.light.directional_color1[0] = 200;
		//game->scene.light.directional_color1[1] = 100;
		//game->scene.light.directional_color1[2] = 50;

		t3d_matrix_push_pos(1);

		scenery_draw(scenery);

		light_setAmbient(&game->scene.light, 0xBF);
		platform_drawBatch();
		light_resetAmbient(&game->scene.light);

		t3d_matrix_pop(1);

		game->syncPoint = rspq_syncpoint_new();

		// Convert frames to seconds based on refresh rate
		uint8_t secondsLeft = (game->countdownTimer / display_get_refresh_rate()) + 1;
		ui_countdown(secondsLeft);

		game->countdownTimer--;

		game->timing.frame_counter = 0;

		rdpq_detach_show();
		sound_update();
		return; // Exit early until countdown finishes
    }


// ======== Gameplay ======== //

	float endTimerFactor = 25.7f;

	// AI
#ifndef AI_BATTLE
	for (size_t i = 0; i < AI_COUNT; i++)
	{
		if(player[i+PLAYER_COUNT].died) continue;
		if(game->winnerSet) continue;
		ai_generateControlData(&ai[i], &player[i+PLAYER_COUNT].control, &actor[player[i+PLAYER_COUNT].actor_id], hexagons, game->scene.camera.offset_angle);
	}
#else
	for (size_t i = 0; i < AI_COUNT; i++)
	{
		if(player[i].died) continue;
		if(game->winnerSet) continue;
		ai_generateControlData(&ai[i], &player[i].control, &actor[player[i].actor_id], hexagons, game->scene.camera.offset_angle);
	}
#endif

	// Platforms
	if(game->timing.frame_counter > display_get_refresh_rate()*10)
	{
		platform_dropGroup(hexagons, 2, game->timing.fixed_time_s);
	}
	if(game->timing.frame_counter > display_get_refresh_rate()*20)
	{
		platform_dropGroup(hexagons, 1, game->timing.fixed_time_s);
	}

	if(!game->winnerSet)
	{
		for (size_t j = 0; j < PLATFORM_COUNT; j++)
		{
			platform_loop(&hexagons[j], actor, game->diff);
		}
	}



	// Actors
	uint8_t loserCount = 0;
	uint8_t aliveCount = 0;
    uint8_t lastAlivePlayer = 0;

	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		// Use player[i].actor_id to identify the assigned actor
		uint8_t actorIndex = player[i].actor_id;
		Actor* currentActor = &actor[actorIndex];
		// Sync player's position with the actor
		player[i].position = currentActor->body.position;
		if (currentActor->state != DEATH)
		{
			if (!game->winnerSet)
			{
				aliveCount++;
				lastAlivePlayer = i; // Track the last alive player
				// Update the assigned actor using its actor ID
				actor_update(currentActor, &player[i].control, &game->timing, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
				// Update collision data for the assigned actor
				actorCollision_collidePlatforms(currentActor, &actor_contact[actorIndex], &actor_collider[actorIndex], hexagons);

				// Update matrix
				actor_updateMat(currentActor);
			}
		} else {

			// Bugfix: Center dead actor's position to not break camera 
			currentActor->body.position = (Vector3){0,0,250};
			static int8_t timer[MAXPLAYERS] = {0};
			if (!player[i].control.has_rumbled)
			{
				controllerData_rumbleFrames(&player[i].control, i, 5);
				if (++timer[i] > 25) player[i].control.has_rumbled = true;
			} else {
				controllerData_rumbleStop(&player[i].control, i);
			}
			player[i].died = true;
			loserCount++;
		}
	}

	for (int i = 0; i < MAXPLAYERS; i++) {

        // Get the player's actor colorID
        int playerColorID = actor[player[i].actor_id].colorID;

        // Count platforms with the same colorID
        for (int j = 0; j < PLATFORM_COUNT; j++) {
            if (hexagons[j].colorID == playerColorID) {
                if(hexagons[j].platformTimer == 120 - (core_get_aidifficulty()*10)) player[i].score++;
            }
        }
    }

	// Check if we have a winner (only one alive player left) OR if there's only one platform left
	if ((aliveCount <= 1 || game->timing.frame_counter > display_get_refresh_rate()*endTimerFactor) && !game->winnerSet)
	{
		int scores[4] = {
			player[0].score,
			player[1].score,
			player[2].score,
			player[3].score
		};

		// Give last player alive bonus points
		player[lastAlivePlayer].score++;

		// Find the highest score
		int highestScore = scores[0];

		for (int i = 1; i < 4; i++)
		{
			if (scores[i] > highestScore) highestScore = scores[i];
		}

		// Identify all players with the highest score
		bool winners[4] = {false, false, false, false};
		int winnerCount = 0;

		for (int i = 0; i < 4; i++)
		{
			if (scores[i] == highestScore)
			{
				winners[i] = true;
				winnerCount++;
			}
		}

		// Set the winners
		if (winnerCount > 1)
		{
			// Handle tie case with multiple winners
			for (int i = 0; i < 4; i++)
			{
				if (winners[i])
				{
					core_set_winner(i); // Set each tied player as a winner
				}
			}
			game->winnerID = 5; // Optional: Use -1 to represent a tie scenario
		} else {
			// Handle single winner
			for (int i = 0; i < 4; i++)
			{
				if (winners[i])
				{
					core_set_winner(i);
					game->winnerID = i;
					break;
				}
			}
		}
		game->winnerSet = true;
	}


	// Sync RSPQ once, and then update each actors' skeleton
	if(game->syncPoint)rspq_syncpoint_wait(game->syncPoint);
	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		t3d_skeleton_update(&actor[i].armature.main);
	}

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);
	screen_applyColor_Depth(&game->screen, RGBA32(154, 181, 198, 0xFF), false);

	light_set(&game->scene.light);

	// Reset light direction to default in case players have paused
	game->scene.light.direction1 = (T3DVec3){{0,-1,0}};

	t3d_matrix_push_pos(1);

	scenery_draw(scenery);

	light_setAmbient(&game->scene.light, 0xBF);
	platform_drawBatch();
	light_resetAmbient(&game->scene.light);

	// Don't bother drawing shadows for the AI
	for (size_t s = 0; s < PLAYER_COUNT; s++)
	{
		if(actor[player[s].actor_id].state == FALLING || actor[player[s].actor_id].state == JUMP) 
			player_drawShadow(actor[player[s].actor_id].body.position, &game->screen.gameplay_viewport);
	}

	t3d_frame_start(); // reset after drawing shadows

	actor_draw(actor);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	// TPX
	for (int j = 0; j < PLATFORM_COUNT; j++) {
		ptx_draw(&game->screen.gameplay_viewport,&hexagons[j], &lavaBubbles);
	}

	for (size_t i = 0; i < ACTOR_COUNT; i++)
	{
		if(!game->winnerSet)
		{
			ui_print_playerNum(&player[i], &game->screen);
		}

		ui_playerScores(&player[i]);
	}

	if(loserCount >= 3 || game->timing.frame_counter > display_get_refresh_rate()*endTimerFactor)
	{
		if(game->winnerSet)
		{
			game->winTimer++;
			if(game->winTimer < 120)
			{
				if(game->winnerID != 5)
				{
					ui_print_winner(game->winnerID+1);
				} else {
					ui_print_winner(game->winnerID); // TIE condition
				}
			}
			if(game->winTimer >= 118) game->state = GAME_OVER;
		}
	}

	if(player[0].control.held.r)
	{
		ui_fps(game->timing.frame_rate);
		if(core_get_playercount() == 1) ui_input_display(&player[0].control);
	}

	rdpq_detach_show();
	sound_update();

#ifdef PROFILING
	rspq_profile_next_frame();
	if(game->timing.frame_counter > 29)
	{
		rspq_profile_dump();
		rspq_profile_reset();
		game->timing.frame_counter = 0;
	}
    rspq_profile_get_data(&profile_data);
#endif // PROFILING
}


void gameState_setPause(Game* game, Player* player, Actor* actor, Scenery* scenery)
{

	//move_lava(scenery);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);
	screen_applyColor_Depth(&game->screen, ui_color(VIOLET), false);

	light_set(&game->scene.light);

	// Instead drawing a dark transparent texture over the scene, just change the light direction
	game->scene.light.direction1 = (T3DVec3){{-1,-1,-1}};

	t3d_matrix_push_pos(1);

	scenery_draw(scenery);

	//light_setAmbient(&game->scene.light, 0xFF);
	platform_drawBatch();
	light_resetAmbient(&game->scene.light);

	actor_draw(actor);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_pause(&player[0].control);
	if(player[0].control.held.r)
	{
		ui_fps(game->timing.frame_rate);
		if(core_get_playercount() == 1) ui_input_display(&player[0].control);
	}

	rdpq_detach_show();
	sound_update();

#ifdef PROFILING
	rspq_profile_next_frame();
	if(game->timing.frame_counter > 29)
	{
		rspq_profile_dump();
		rspq_profile_reset();
		game->timing.frame_counter = 0;
	}
    rspq_profile_get_data(&profile_data);
#endif // PROFILING
}


void gameState_setGameOver()
{
    minigame_end();
}

void game_play(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact)
{
	for(;;)
	{

		// Tme
		time_setData(&game->timing);

		// Controls
		game_setControlData(game, player);
		player_setControlData(player);

		// Hold Z to quit on the Pause screen
		static int resetTimer = 0;
		if(game->state == PAUSE && player[0].control.held.z)
		{
			resetTimer++;
			if(resetTimer == 30)
			{
				sound_wavPlay(SFX_JUMP, false);
			} else if (resetTimer > 40) {
				game->state = GAME_OVER;
			}
		}

		if(game->state == PAUSE && player[0].control.released.z) resetTimer = 0;

//// CAMERA /////
		if(player[0].control.pressed.l) game->scene.camera.cam_mode ^= 1;

		Vector3 introStartPos = (Vector3){-1000,-3000,-100};
		Vector3 centerHex = hexagons[10].home;
		Vector3 csPos = (Vector3){0, -1000, 525};
		Vector3 gamePlayPos = (Vector3){0, -600, 1000};

		game->scene.camera.camTime += game->timing.fixed_time_s;

		if(game->state == INTRO)
		{
			float t = game->scene.camera.camTime / game->scene.camera.lerpTime;
			t = clamp(t, 0.0f, 1.0f);
			Vector3 camPos = vector3_lerp(&introStartPos, &centerHex, t);
			camera_getMinigamePosition(&game->scene.camera, actor, player, camPos);
		} else {

			if(game->scene.camera.cam_mode == 0)
			{
				camera_getOrbitalPosition(&game->scene.camera, csPos, game->timing.fixed_time_s);
			} else {
				float t = game->scene.camera.camTime / game->scene.camera.lerpTime;
				t = clamp(t, 0.0f, 1.0f);
				Vector3 camPos = vector3_lerp(&csPos, &gamePlayPos, t);
				camera_getMinigamePosition(&game->scene.camera, actor, player, camPos);
			}
		}
		camera_set(&game->scene.camera, &game->screen);
//// CAMERA /////

		// Sound: spatial reverb
		sound_spatial(&centerHex, &centerHex,  &game->scene.camera);

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
				game->scene.camera.cam_mode = 1;
				for(uint8_t p = 0; p < game->humanCount; p++)
				{
					if(player[p].isHuman && player[p].died && !player[p].deathCounted)
					{
						game->deadPool++;
						player[p].deathCounted = true;
					}

				}
				if(game->deadPool == game->humanCount)
				{
					display_set_fps_limit(0);
				} else {
					display_set_fps_limit((display_get_refresh_rate() / 4) * 2);
				}
				gameState_setGameplay(game, player, ai, actor, scenery, actor_collider, actor_contact);
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