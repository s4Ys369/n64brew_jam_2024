#ifndef GAME_STATES_H
#define GAME_STATES_H


// function prototypes

void gameState_setIntro();
void gameState_setMainMenu();

void gameState_setGameplay(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* box_collider);
void gameState_setPause(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* box_collider);

void gameState_setGameOver();

void game_play(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* box_collider);


// function implementations

void gameState_setIntro()
{
    // code for the intro state
}
void gameState_setMainMenu()
{
    // code for the game over state
}

void gameState_setGameplay(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* box_collider)
{
	
	// ======== Update ======== //

	time_setData(&game->timing);
	controllerData_getInputs(&game->control);

	for (int i = 0; i < ACTOR_COUNT; i++) {

		actor_update(&actor[i], &game->control, game->timing.frame_time_s, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
	};


	// new code for collision detection /////////////////////////////////

    actorContactData_clear(actor_contact);
    actorCollider_setVertical(actor_collider, &actor->body.position);

    if (actor->body.position.z != 0 
		&& actor->state != FALLING 
        && actor->state != JUMP) {

        actor->grounding_height = 0.0f;
        if(!(actor->grounded))actor->state = FALLING;
    }

	if (actorCollision_contactBox(actor_collider, box_collider)) {
        actorCollision_contactBoxSetData(actor_contact, actor_collider, box_collider);
        actorCollision_setResponse(&actor[0], actor_contact, actor_collider);
    } else {
		if(actor->body.position.z > actor->grounding_height)
		actor->grounded = false;
	}

	actor_setState(actor, actor->state);
    
	///////////////////////////////////////////////////////////////////


	cameraControl_setOrbitalMovement(&game->scene.camera, &game->control);
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
	ui_fps();
	ui_printf(
		"cState %d\n"
		"pState %d\n"
		"Grounded %d",
		actor->state,
		actor->previous_state,
		actor->grounded
	);

	game->syncPoint = rspq_syncpoint_new();

	rdpq_detach_show();
}


void gameState_setPause(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* box_collider)
{
	// ======== Update ======== //

	time_setData(&game->timing);
	controllerData_getInputs(&game->control);

	cameraControl_setOrbitalMovement(&game->scene.camera, &game->control);
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

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	rdpq_detach_show();
}


void gameState_setGameOver()
{
    // code for the game over state
}

void game_play(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box* box_collider)
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
				gameState_setGameplay(game, actor, scenery,actor_collider, actor_contact, box_collider);
				break;
			}
			case PAUSE:{
				gameState_setPause(game, actor, scenery,actor_collider, actor_contact, box_collider);
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