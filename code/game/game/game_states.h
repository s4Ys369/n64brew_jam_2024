#ifndef GAME_STATES_H
#define GAME_STATES_H


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


void gameState_setGameplay(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[], size_t numBoxes)
{
	
	// ======== Update ======== //

	time_setData(&game->timing);
	controllerData_getInputs(game->control);

	for (uint8_t i = 0; i < ACTOR_COUNT; i++) {
		actor_update(&actor[i], &game->control[i], game->timing.frame_time_s, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
	}

	actorCollision_updateBoxes(&actor[0], actor_contact, actor_collider, box_collider, numBoxes);

	cameraControl_setOrbitalMovement(&game->scene.camera, &game->control[0]);
	camera_getMinigamePosition(&game->scene.camera, actor[0].body.position, game->timing.frame_time_s);
	camera_set(&game->scene.camera, &game->screen);


	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	for (int i = 0; i < SCENERY_COUNT; i++) {

		scenery_draw(&scenery[i]);
	};
	
	for (int i = 0; i < ACTOR_COUNT; i++) {
		
		actor_draw(&actor[i]);
	};

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_fps();

	rdpq_detach_show();
	sound_update_buffer();
}


void gameState_setPause(Game* game, Actor* actor, Scenery* scenery, PlayerData* player, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[])
{
	// ======== Update ======== //

	time_setData(&game->timing);
	controllerData_getInputs(game->control);

	cameraControl_setOrbitalMovement(&game->scene.camera, &game->control[0]);
	camera_getMinigamePosition(&game->scene.camera, actor[0].body.position, game->timing.frame_time_s);
	camera_set(&game->scene.camera, &game->screen);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	for (int i = 0; i < SCENERY_COUNT; i++) {

		scenery_draw(&scenery[i]);
	}
	
	for (int i = 0; i < ACTOR_COUNT; i++) {
		
		actor_draw(&actor[i]);
	};


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
				gameState_setPause(game, actor, scenery, players,actor_collider, actor_contact, box_collider);
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