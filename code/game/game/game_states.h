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

void gameState_setGameplay(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[], size_t numBoxes);
void gameState_setPause(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[]);

void gameState_setGameOver();

void game_play(Game* game, Actor* actor, Scenery* scenery, PlayerData* players, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[], size_t numBoxes);


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
void gameState_setGameplay(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[], size_t numBoxes)
{
	
	// ======== Update ======== //
	rdpq_sync_pipe();
    t3d_frame_start();
	rdpq_sync_tile();

	time_setData(&game->timing);
	frameCounter++;

	for (int i = 0; i < ACTOR_COUNT; i++)
	{
		controllerData_getInputs(player[i].port, game->control[i]);
		actor_update(&actor[i], game->control[i], game->timing.frame_time_s, game->scene.camera[i].angle_around_barycenter, game->scene.camera[i].offset_angle, &game->syncPoint);
		cameraControl_setOrbitalMovement(&game->scene.camera[i], game->control[i]);
		camera_getMinigamePosition(&game->scene.camera[i], actor[i].body.position, game->timing.frame_time_s);
		camera_set(&game->scene.camera[i], &game->screen.gameplay_viewport[i]);
	}

	actorCollision_updateBoxes(&actor[0], actor_contact, actor_collider, box_collider, numBoxes);


	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	light_set(&game->scene.light);
	

	for (int i = 0; i < ACTOR_COUNT; i++)
	{
		rdpq_sync_pipe();
		rdpq_sync_tile();
		screen_clearT3dViewport(&game->screen.gameplay_viewport[i]);

		t3d_matrix_push_pos(1);

		for (int i = 0; i < SCENERY_COUNT; i++) {
			
			scenery_draw(&scenery[i]);
		};
	
		for (int i = 0; i < ACTOR_COUNT; i++) {
			actor_draw(&actor[i]);
		};

		t3d_matrix_pop(1);

		game->syncPoint = rspq_syncpoint_new();

	}


	int sizeX = display_get_width();
    int sizeY = display_get_height();
    rdpq_sync_pipe();
    rdpq_set_scissor(0, 0, sizeX, sizeY);
    rdpq_set_mode_standard();
    rdpq_set_mode_fill(ui_color(BLACK));

    // draw thick lines between the screens
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


void gameState_setPause(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[])
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

	t3d_matrix_push_pos(1);

	for (int i = 0; i < SCENERY_COUNT; i++) {

		scenery_draw(&scenery[i]);
	}

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	rdpq_detach_show();
	sound_update_buffer();
}


void gameState_setGameOver()
{
    // code for the game over state
}

void game_play(Game* game, Actor* actor, Scenery* scenery, PlayerData* players, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[], size_t numBoxes)
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
				gameState_setGameplay(game, actor, scenery, players,actor_collider, actor_contact, box_collider, numBoxes);
				break;
			}
			case PAUSE:{
				//gameState_setPause(game, actor, scenery, players,actor_collider, actor_contact, box_collider);
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