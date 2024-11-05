#ifndef GAME_STATES_H
#define GAME_STATES_H


// function prototypes

void gameState_setIntro();
void gameState_setMainMenu();

void gameState_setGameplay(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[], size_t numBoxes);
void gameState_setPause(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[]);

void gameState_setGameOver();

void game_play(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[], size_t numBoxes);


// function implementations

void gameState_setIntro()
{
    // code for the intro state
}
void gameState_setMainMenu()
{
    // code for the game over state
}

void gameState_setGameplay(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[], size_t numBoxes)
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

	// Check if the actor is neither jumping nor falling
	if (actor->body.position.z != -2000.0f // magic number
		&& actor->state != JUMP
		&& actor->state != FALLING) {
			
		actor->state = FALLING;
		actor->grounded = false;
		actor->grounding_height = -2000.0f; // magic number
        
	}

	

	for (size_t i = 0; i < numBoxes; ++i) {
		if (actorCollision_contactBox(actor_collider, &box_collider[i])) {
			actorCollision_contactBoxSetData(actor_contact, actor_collider, &box_collider[i]);
			actorCollision_setResponse(&actor[0], actor_contact, actor_collider);
        
			actor->hasCollided = true; // Set to true only if collision occurs
			actor->grounded = true;    // Set grounded if we have a collision
			actor->state = STAND_IDLE;
			debugf("Grounding Height Updated to: %.2f\n", actor->grounding_height); // Debug output
			break; // Exit after the first collision
		} else {
			actor->hasCollided = false;
		}
	}

	// Call setState after processing collision responses
	actor_setState(actor, actor->state);

	if(actor->body.position.z <= -199.0f) actor->body.position = (Vector3){0.0f, 0.0f, 200.0f};

    
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
		"Contact %d\n"
		"gHeight %.2f\n"
		"Actor Z %.2f\n"
		"Grounded %d",
		actor->state,
		actor->previous_state,
		actor->hasCollided,
		actor->grounding_height,
		actor->body.position.z,
		actor->grounded
	);

	game->syncPoint = rspq_syncpoint_new();

	rdpq_detach_show();
}


void gameState_setPause(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[])
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

void game_play(Game* game, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact, Box box_collider[], size_t numBoxes)
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
				gameState_setGameplay(game, actor, scenery,actor_collider, actor_contact, box_collider, numBoxes);
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