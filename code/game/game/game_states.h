#ifndef GAME_STATES_H
#define GAME_STATES_H

// Comment out to disable RSPQ Profiling
#define PROFILING

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
		actor_update(&actor[i], game->control[i], game->timing.frame_time_s, game->scene.camera[i].angle_around_barycenter, game->scene.camera[i].offset_angle, &game->syncPoint);
		cameraControl_setOrbitalMovement(&game->scene.camera[i], game->control[i]);
		camera_getMinigamePosition(&game->scene.camera[i], actor[i].body.position, game->timing.frame_time_s);
		actor_updateMat(&actor[i]);
	}

	Box allBoxes[90];
    size_t boxIndex = 0;

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 3; j++) {
            allBoxes[boxIndex++] = platform[i].collider->boxes[j];
        }
    }

	for (int i = 0; i < ACTOR_COUNT; i++) actorCollision_updateBoxes(&actor[i], &actor_contact[i], &actor_collider[i], allBoxes, boxIndex);


	// ======== Draw 3D ======== //
	screen_clearDisplay(&game->screen);
	light_set(&game->scene.light);
	

	// For each player
	for (int i = 0; i < ACTOR_COUNT; i++)
	{
		// Set projection and look at for each viewport based on player camera
		camera_set(&game->scene.camera[i], &game->screen.gameplay_viewport[i]);

		// Clear and attach player viewport
		screen_clearT3dViewport(&game->screen.gameplay_viewport[i]);

		t3d_matrix_push_pos(1);

			scenery_drawBatch(rspqBlocks, blockCount);
		
			// Draw updated players
			actor_draw(&actor[0]);
			actor_draw(&actor[1]);
		
		t3d_matrix_pop(1);
	}
	

	game->syncPoint = rspq_syncpoint_new();

	// ======== Draw 2D ======== //

	rdpq_sync_pipe();
	rdpq_set_mode_standard();
	rdpq_set_scissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	ui_fps();
	ui_printf("v0.1");
	ui_input_display(game->control[0]);

	// draw thick lines between the screens
	rdpq_set_mode_fill(ui_color(BLACK));
	int sizeX = SCREEN_WIDTH; int sizeY = SCREEN_HEIGHT;
	switch (ACTOR_COUNT){
      case 1:
        break;
      case 2:
        rdpq_fill_rectangle(0, sizeY/2-1, sizeX, sizeY/2+1);
        break;
      case 3:
        rdpq_fill_rectangle(0, sizeY/2-1, sizeX, sizeY/2+1);
        rdpq_fill_rectangle(sizeX/2-1, sizeY/2, sizeX/2+1, sizeY);
        break;
      case 4:
        rdpq_fill_rectangle(0, sizeY/2-1, sizeX, sizeY/2+1);
        rdpq_fill_rectangle(sizeX/2-1, 0, sizeX/2+1, sizeY);
        break;
    }

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