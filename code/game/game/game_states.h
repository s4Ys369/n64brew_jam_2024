#ifndef GAME_STATES_H
#define GAME_STATES_H


// function prototypes

void gameState_setIntro(Game* game, Player* player, Scenery* scenery);
void gameState_setMainMenu();

void gameState_setGameplay(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact);
void gameState_setPause(Game* game, Player* player, Actor* actor, Scenery* scenery);

void gameState_setGameOver();

void game_play(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact);


void gameState_setIntro(Game* game, Player* player, Scenery* scenery)
{
	// ======== Update ======== //

	time_setData(&game->timing);
	player_setControlData(player);

	// ======== Draw ======== //
	
	screen_clearDisplay(&game->screen);
	screen_clearT3dViewport(&game->screen);

	light_set(&game->scene.light);

	t3d_matrix_push_pos(1);

	//scenery_draw(scenery);
	move_lava(scenery);
	room_draw(scenery);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_fps();
	ui_intro(&player[0].control);

	rdpq_detach_show();
	sound_update_buffer();
}

void gameState_setMainMenu()
{
}

void gameState_setGameplay(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact)
{
	
	// ======== Update ======== //

	time_setData(&game->timing);
	player_setControlData(player);

	Box allBoxes[PLATFORM_COUNT*3];
    size_t boxIndex = 0;

    for (size_t i = 0; i < PLATFORM_COUNT; i++) {
		for (int j = 0; j < 3; j++) {
            allBoxes[boxIndex++] = hexagons[i].collider.box[j];
        }
		for (size_t a = 1; a < ACTOR_COUNT; a++) {
			platform_loop(&hexagons[i], &actor[a]);
		}
    }

	// @TODO: need a function to determine the amount of human players versus AI players
	for (size_t i = 1; i < 4; i++) {
		ai_generateControlData(&ai[i], &player[i].control, &actor[i], hexagons, PLATFORM_COUNT, game->scene.camera.offset_angle);
	}

	for (uint8_t i = 0; i < ACTOR_COUNT; i++) {

		actorCollision_updateBoxes(&actor[i], &actor_contact[i], &actor_collider[i], allBoxes, boxIndex);
		actor_update(&actor[i], &player[i].control, game->timing.frame_time_s, game->scene.camera.angle_around_barycenter, game->scene.camera.offset_angle, &game->syncPoint);
	
	}

	for (size_t i = 0; i < PLATFORM_COUNT; i++)
	{
		for (size_t a = 0; a < ACTOR_COUNT; a++)
		{
			platform_loop(&hexagons[i], &actor[a]);
		}
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
	move_lava(scenery);
	room_draw(scenery);

	platform_drawBatch();
	
	actor_draw(actor);

	t3d_matrix_pop(1);

	game->syncPoint = rspq_syncpoint_new();

	ui_fps();
	ui_main_menu(&player[0].control);

	rdpq_detach_show();
	sound_update_buffer();
}


void gameState_setGameOver()
{
    // code for the game over state
}

void game_play(Game* game, Player* player, AI* ai, Actor* actor, Scenery* scenery, ActorCollider* actor_collider, ActorContactData* actor_contact)
{
	for(;;)
	{
		game_setControlData(game, player);
		switch(game->state)
		{
			case INTRO:{
				gameState_setIntro(game, player, scenery);
				break;
			}
			case MAIN_MENU:{
				gameState_setMainMenu();
				break;
			}
			case GAMEPLAY:{
				gameState_setGameplay(game, player, ai, actor, scenery, actor_collider, actor_contact);
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