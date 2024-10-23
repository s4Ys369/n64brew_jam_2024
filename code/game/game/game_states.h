#ifndef GAME_STATES_H
#define GAME_STATES_H


// function prototypes

void gameState_setIntro();
void gameState_setMainMenu(Game* game);
void gameState_setGameplay(Game* game);
void gameState_setPause();
void gameState_setGameOver();

void game_setState(Game* game);


// function implementations

void gameState_setIntro()
{
    // code for the intro state
}

void gameState_setMainMenu(Game* game)
{

	//scenery
	Scenery room = scenery_create(0, "rom:/game/testLevel.t3dm");

	Scenery n64logo = scenery_create(0, "rom:/game/n64logo.t3dm");

	for(;;)
	{
		// ======== Update ======== //

		time_setData(&game->timing);
		controllerData_getInputs(&game->control);

		cameraControl_setOrbitalMovement(&game->scene.camera, &game->control);
		camera_getOrbitalPosition(&game->scene.camera, (Vector3){0, 0, 0}, game->timing.frame_time_s);
		camera_set(&game->scene.camera, &game->screen);

		scenery_set(&room);
		scenery_set(&n64logo);
		n64logo.position = (Vector3){200, 200, 0};

		// ======== Draw ======== //
		
		screen_clearDisplay(&game->screen);
		screen_clearT3dViewport(&game->screen);
	
		light_set(&game->scene.light);
    
		t3d_matrix_push_pos(1);

		scenery_draw(&n64logo);
		scenery_draw(&room);
   
   		t3d_matrix_pop(1);

		ui_draw();

		rdpq_detach_show();
		
	}
}

void gameState_setGameplay(Game* game)
{

	//actor
	Actor player = actor_create(0, "rom:/game/pipo.t3dm");
	ActorAnimation player_animation = actorAnimation_create(&player);
	actorAnimation_init(&player, &player_animation);

	//scenery
	Scenery room = scenery_create(0, "rom:/game/testLevel.t3dm");

	Scenery n64logo = scenery_create(0, "rom:/game/n64logo.t3dm");

	for(;;)
	{
		// ======== Update ======== //

		time_setData(&game->timing);
		controllerData_getInputs(&game->control);

		actor_setControlData(&player, &game->control, game->timing.frame_time_s, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle);
		actor_setState(&player, player.state);
		actor_setMotion(&player, game->timing.frame_time_s);
		actor_setAnimation(&player, &player_animation, game->timing.frame_time_s, &game->syncPoint);

		cameraControl_setOrbitalMovement(&game->scene.camera, &game->control);
		camera_getMinigamePosition(&game->scene.camera, player.body.position, game->timing.frame_time_s);
		camera_set(&game->scene.camera, &game->screen);

		scenery_set(&room);
		scenery_set(&n64logo);
		n64logo.position = (Vector3){200, 200, 0};

		// ======== Draw ======== //
		
		screen_clearDisplay(&game->screen);
		screen_clearT3dViewport(&game->screen);
	
		light_set(&game->scene.light);
    
		t3d_matrix_push_pos(1);

		scenery_draw(&n64logo);
		scenery_draw(&room);

		actor_draw(&player);
   
   		t3d_matrix_pop(1);

		game->syncPoint = rspq_syncpoint_new();

		ui_draw();

		rdpq_detach_show();
	}

	t3d_skeleton_destroy(&player.armature.main);
	t3d_skeleton_destroy(&player.armature.blend);

	t3d_model_free(player.model);
	t3d_model_free(room.model);

	t3d_destroy();
}

void gameState_setPause()
{
    // code for the pause state
}

void gameState_setGameOver()
{
    // code for the game over state
}

void game_setState(Game* game)
{
    switch(game->state)
    {
        case INTRO:{
            gameState_setIntro();
            break;
		}
        case MAIN_MENU:{
            gameState_setMainMenu(game);
            break;
		}
        case GAMEPLAY:{
            gameState_setGameplay(game);
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
}

#endif