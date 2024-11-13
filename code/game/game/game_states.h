#ifndef GAME_STATES_H
#define GAME_STATES_H


// function prototypes

void gameState_setIntro();
void gameState_setMainMenu();

void gameState_setGameplay(Game* game, Player* player, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact);
void gameState_setPause(Game* game, Player* player, Actor* actor, Scenery* scenery);

void gameState_setGameOver();

void game_play(Game* game, Player* player, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact);


void gameState_setIntro()
{
}
void gameState_setMainMenu()
{
}

void gameState_setGameplay(Game* game, Player* player, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact)
{
	
	// ======== Update ======== //

	time_setData(&game->timing);
	player_setControlData(player);

	Box allBoxes[PLATFORM_COUNT * 3];
    size_t boxIndex = 0;

    for (size_t i = 0; i < PLATFORM_COUNT; i++) {
		platform_loop(&hexagons[i], i);
        for (size_t j = 0; j < 3; j++) {
            allBoxes[boxIndex++] = hexagons[i].collider.boxes[j];
        }
    }

	for (uint8_t i = 0; i < ACTOR_COUNT; i++) {

		actorCollision_updateBoxes(&actor[i], &actor_contact[i], &actor_collider[i], allBoxes, boxIndex);
		actor_update(&actor[i], &player[i].control, game->timing.frame_time_s, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
	
	}

	cameraControl_setOrbitalMovement(&game->scene.camera, &player[0].control);
	camera_getMinigamePosition(&game->scene.camera, actor[0].body.position, game->timing.frame_time_s);
	camera_set(&game->scene.camera, &game->screen);


	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	//scenery_draw(scenery);
	move_lava(scenery);
	room_draw(scenery);
	platform_drawBatch();

	actor_draw(actor);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_fps();

	rdpq_detach_show();
	sound_update_buffer();
}


void gameState_setPause(Game* game, Player* player, Actor* actor, Scenery* scenery)
{
	// ======== Update ======== //

	time_setData(&game->timing);
	player_setControlData(player);

	camera_set(&game->scene.camera, &game->screen);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	//scenery_draw(scenery);
	//move_lava(scenery);
	room_draw(scenery);
	
	actor_draw(actor);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	rdpq_detach_show();
	sound_update_buffer();
}


void gameState_setGameOver()
{
    // code for the game over state
}

void game_play(Game* game, Player* player, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact)
{
	for(;;)
	{
		game_setControlData(game, player);
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
				gameState_setGameplay(game, player, actor, scenery, actor_collider, actor_contact);
				break;
			}
			case PAUSE:{
				gameState_setPause(game, player, actor, scenery);
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