#ifndef GAME_STATES_H
#define GAME_STATES_H

#define INTRO 0
#define MAIN_MENU 1
#define GAMEPLAY 2
#define PAUSE 3
#define GAME_OVER 4

// Core variable accumulators
static uint32_t playerCount;

// function prototypes

void gameState_setIntro();
void gameState_setMainMenu(Screen* screen, TimeData* timing, ControllerData** control);
void gameState_setGameplay(Screen* screen, TimeData* timing, ControllerData** control, PlayerData** player);
void gameState_setPause();
void gameState_setGameOver();

void game_setState(uint8_t state, Screen* screen, TimeData* timing, ControllerData** control, PlayerData** players);


// function implementations

void gameState_setIntro()
{
    // code for the intro state
}

void gameState_setMainMenu(Screen* screen, TimeData* timing, ControllerData** control)
{
	screen_initGameplayViewport(screen);
	t3d_init((T3DInitParams){});

	Camera camera = camera_create();

	//light
	LightData light = light_create();

	//scenery
	Scenery room = scenery_create(0, "rom:/game/testLevel.t3dm");

	Scenery n64logo = scenery_create(0, "rom:/game/n64logo.t3dm");

	for(;;)
	{
		// ======== Update ======== //

		time_setData(timing);
		controllerData_getInputs(core_get_playercontroller(PLAYER_1), control[PLAYER_1]);

		cameraControl_setOrbitalMovement(&camera, control[PLAYER_1]);
		camera_getOrbitalPosition(&camera, (Vector3){0, 0, 0}, timing->frame_time_s);
		camera_set(&camera, screen);

		scenery_set(&room);
		scenery_set(&n64logo);
		n64logo.position = (Vector3){200, 200, 0};

		// ======== Draw ======== //
		
		screen_clear(screen);
		screen_clearGameplayViewport(screen);
	
		light_set(&light);
    
		t3d_matrix_push_pos(1);

		scenery_draw(&n64logo);
		scenery_draw(&room);
   
   		t3d_matrix_pop(1);

		ui_draw();

		rdpq_detach_show();
		
	}
}

void gameState_setGameplay(Screen* screen, TimeData* timing, ControllerData** control, PlayerData** player)
{
	screen_initGameplayViewport(screen);
	t3d_init((T3DInitParams){});
  	rspq_syncpoint_t syncPoint = 0;

	Camera camera = camera_create();

	//light
	LightData light = light_create();

	//actor
	playerCount = core_get_playercount();
	for(uint32_t p = 0; p < playerCount; ++p)
	{
		if (player[p] != NULL)
		{
			*player[p]->actor = actor_create(0, "rom:/game/pipo.t3dm");
			*player[p]->animation = actorAnimation_create(player[p]->actor);
			actorAnimation_init(player[p]->actor, player[p]->animation);
		}
	}

	//scenery
	Scenery room = scenery_create(0, "rom:/game/room.t3dm");

	Scenery n64logo = scenery_create(0, "rom:/game/n64logo.t3dm");

	for(;;)
	{
		// ======== Update ======== //

		time_setData(timing);
	
		for(uint32_t p = 0; p < playerCount; ++p)
		{
			if (player[p] != NULL && player[p]->actor != NULL)
			{
				controllerData_getInputs(player[p]->port, control[p]);
				actor_setControlData(player[p]->actor, control[p], timing->frame_time_s, camera.angle_around_barycenter, camera.offset_angle);
				actor_setMotion(player[p]->actor, timing->frame_time_s);
			}
		}
		cameraControl_setOrbitalMovement(&camera, control[PLAYER_1]);

		// CAM SWITCH TEST
		static uint8_t camSwitch = 0;
		static Vector3 camOrbit; 
		static Vector3 targetPosition;

		if (control[PLAYER_1]->pressed.b && playerCount > PLAYER_2)
		{
    		camSwitch ^= 1;
			targetPosition = player[camSwitch]->actor->body.position;
		} else {
			targetPosition = player[camSwitch]->actor->body.position;
		}

		if (camOrbit.x != targetPosition.x || camOrbit.y != targetPosition.y || camOrbit.z != targetPosition.z)
    		camOrbit = vector3_lerp(&camOrbit, &targetPosition, 0.2f);

		camera_getOrbitalPosition(&camera, camOrbit, timing->frame_time_s);
		camera_set(&camera, screen);

		scenery_set(&room);
		scenery_set(&n64logo);
		n64logo.position = (Vector3){200, 200, 0};

		// ======== Draw ======== //
		
		screen_clear(screen);
		screen_clearGameplayViewport(screen);
	
		light_set(&light);
    
		t3d_matrix_push_pos(1);

		scenery_draw(&n64logo);
		scenery_draw(&room);

		for(uint32_t p = 0; p < playerCount; ++p)
		{
			if (player[p] != NULL && player[p]->actor != NULL)
			{
				actor_setState(player[p]->actor, player[p]->actor->state);
				actor_setAnimation(player[p]->actor, player[p]->animation, timing->frame_time_s, &syncPoint);
				actor_draw(player[p]->actor);
			}
		}

		t3d_matrix_pop(1);

		syncPoint = rspq_syncpoint_new();

		ui_draw();

		rdpq_detach_show();
	}

	for (uint32_t p = 0; p < core_get_playercount(); ++p)
	{
    	if (player[p] != NULL) {
    	    t3d_skeleton_destroy(&player[p]->actor->armature.main);
    	    t3d_skeleton_destroy(&player[p]->actor->armature.blend);
    	    t3d_model_free(player[p]->actor->model);
    	    free(player[p]);
    	}
	}
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

void game_setState(uint8_t state, Screen* screen, TimeData* timing, ControllerData** control, PlayerData** players)
{
    switch(state)
    {
        case INTRO:{
            gameState_setIntro();
            break;
		}
        case MAIN_MENU:{
            gameState_setMainMenu(screen, timing, control);
            break;
		}
        case GAMEPLAY:{
            gameState_setGameplay(screen, timing, control, players);
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