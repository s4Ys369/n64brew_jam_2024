#ifndef GAME_STATES_H
#define GAME_STATES_H


// function prototypes

void gameState_setIntro();
void gameState_setMainMenu(Game* game, Scenery* scenery);
void gameState_setGameplay(Game* game, Actor* actors, Scenery* scenery);
void gameState_setPause();
void gameState_setGameOver();

void game_play(Game* game, Actor* actors, Scenery* scenery);


// function implementations

void gameState_setIntro()
{
    // code for the intro state
}

void gameState_setMainMenu(Game* game, Scenery* scenery)
{
	// ======== Update ======== //

	time_setData(&game->timing);
	controllerData_getInputs(&game->control);

	cameraControl_setOrbitalMovement(&game->scene.camera, &game->control);
	camera_getOrbitalPosition(&game->scene.camera, (Vector3){0, 0, 0}, game->timing.frame_time_s);
	camera_set(&game->scene.camera, &game->screen);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	for (int i = 0; i < SCENERY_COUNT; i++) {

		scenery_draw(&scenery[i]);
	}

	t3d_matrix_pop(1);

	ui_draw();

	rdpq_detach_show();
}

void gameState_setGameplay(Game* game, Actor* actors, Scenery* scenery)
{
	
	// ======== Update ======== //

	time_setData(&game->timing);
	controllerData_getInputs(&game->control);

	for (int i = 0; i < ACTOR_COUNT; i++) {

		actor_update(&actors[i], &game->control, game->timing.frame_time_s, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
	};

	cameraControl_setOrbitalMovement(&game->scene.camera, &game->control);
	camera_getMinigamePosition(&game->scene.camera, actors[0].body.position, game->timing.frame_time_s);
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
		
		actor_draw(&actors[i]);
	};

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_draw();

	rdpq_detach_show();
}

void gameState_setPause()
{
    // code for the pause state
}

void gameState_setGameOver()
{
    // code for the game over state
}

void game_play(Game* game, Actor* actors, Scenery* scenery)
{

	for(;;)
	{
		switch(game->state)
		{
			case INTRO:{
				gameState_setIntro();
				break;
			}
			case MAIN_MENU:{
				gameState_setMainMenu(game, scenery);
				game->state = MAIN_MENU;
				break;
			}
			case GAMEPLAY:{
				gameState_setGameplay(game, actors, scenery);
				game->state = GAMEPLAY;
				break;
			}
			case PAUSE:{
				gameState_setPause();
				break;
			}
			case GAME_OVER:{
				gameState_setGameOver();
				break;
			}
		}
		
		game_setControlData(game);
	}
}

#endif