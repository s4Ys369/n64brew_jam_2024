#ifndef GAME_STATES_H
#define GAME_STATES_H

#define INTRO 0
#define MAIN_MENU 1
#define GAMEPLAY 2
#define PAUSE 3
#define GAME_OVER 4


// function prototypes

void gameState_setIntro();
void gameState_setMainMenu(Screen* screen, TimeData* timing, ControllerData* control);
void gameState_setGameplay(Screen* screen, TimeData* timing, ControllerData* control);
void gameState_setPause();
void gameState_setGameOver();

void game_setState(uint8_t state, Screen* screen, TimeData* timing, ControllerData* control);


// function implementations

void gameState_setIntro()
{
    // code for the intro state
}

void gameState_setMainMenu(Screen* screen, TimeData* timing, ControllerData* control)
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
		controllerData_getInputs(control);

		ui_poll();

		cameraControl_setOrbitalMovement(&camera, control);
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

void gameState_setGameplay(Screen* screen, TimeData* timing, ControllerData* control)
{
	screen_initGameplayViewport(screen);
	t3d_init((T3DInitParams){});
  	rspq_syncpoint_t syncPoint = 0;

	Camera camera = camera_create();

	//light
	LightData light = light_create();

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

		time_setData(timing);
		controllerData_getInputs(control);

		actor_setControlData(&player, control, timing->frame_time_s, camera.angle_around_barycenter, camera.offset_angle);
		actor_setState(&player, player.state);
		actor_setMotion(&player, timing->frame_time_s);
		actor_setAnimation(&player, &player_animation, timing->frame_time_s, &syncPoint);

		cameraControl_setOrbitalMovement(&camera, control);
		camera_getOrbitalPosition(&camera, player.body.position, timing->frame_time_s);
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

		actor_draw(&player);
   
   		t3d_matrix_pop(1);

		syncPoint = rspq_syncpoint_new();

		rdpq_detach_show();
	}

	t3d_skeleton_destroy(&player.armature.main);
	t3d_skeleton_destroy(&player.armature.blend);

	//t3d_anim_destroy(&animIdle);
	//t3d_anim_destroy(&animWalk);

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

void game_setState(uint8_t state, Screen* screen, TimeData* timing, ControllerData* control)
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
            gameState_setGameplay(screen, timing, control);
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