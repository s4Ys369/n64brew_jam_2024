#ifndef GAME_STATES_H
#define GAME_STATES_H

#define INTRO 0
#define MAIN_MENU 1
#define GAMEPLAY 2
#define PAUSE 3
#define GAME_OVER 4



// function prototypes

void gameState_setIntro();
void gameState_setMainMenu();
void gameState_setGameplay();
void gameState_setPause();
void gameState_setGameOver();

void gameState_init(Game *game, uint8_t state, size_t max_actors, size_t max_sceneries, LoopCallback fixedLoopCallback, LoopCallback loopCallback);
void gameState_set(Game *game, uint8_t state, size_t maxActors, size_t maxSceneries, LoopCallback fixedLoopCallback, LoopCallback loopCallback);
void gameState_setPaused(Game *game);

// function implementations

void gameState_setIntro()
{
    // code for the intro state
}

void gameState_setMainMenu()
{

}

void gameState_setGameplay()
{

}

void gameState_setPause()
{
    // code for the pause state
}

void gameState_setGameOver()
{
    // code for the game over state
}


void gameState_init(Game *game, uint8_t state, size_t max_actors, size_t max_sceneries, LoopCallback fixedLoopCallback, LoopCallback loopCallback)
{

    syncPoint = 0;

    // Initialize camera and light
    game->camera = camera_create();
    game->light = light_create();

    // Allocate dynamic arrays for actors and sceneries
    game->actors = (Actor*)malloc(max_actors * sizeof(Actor));
    game->animations = (ActorAnimation*)malloc(max_actors * sizeof(ActorAnimation));
    game->sceneries = (Scenery*)malloc(max_sceneries * sizeof(Scenery));
    game->actor_count = 0;
    game->scenery_count = 0;

    // Set callbacks
    game->fixedLoopCallback = fixedLoopCallback;
    game->loopCallback = loopCallback;

    // Initialize actors and scenery depending on state
    switch (state)
    {
		case MAIN_MENU:
			game->actor_count = 0;

        	game->sceneries[0] = scenery_create(0, "rom:/root/room.t3dm");
        	game->sceneries[1] = scenery_create(0, "rom:/root/n64logo.t3dm");
			game->sceneries[1].position = (Vector3){200, 200, 0};
        	game->scenery_count = 2;
			break;

		case GAMEPLAY:
			// actors
        	game->actors[0] = actor_create(0, "rom:/root/pipo.t3dm");
        	game->animations[0] = actorAnimation_create(&game->actors[0]);
        	actorAnimation_init(&game->actors[0], &game->animations[0]);
        	game->actor_count = 1;

        	// scenery
        	game->sceneries[0] = scenery_create(0, "rom:/root/room.t3dm");
        	game->sceneries[1] = scenery_create(0, "rom:/root/n64logo.t3dm");
			game->sceneries[1].position = (Vector3){200, 200, 0};
        	game->scenery_count = 2;
			break;
        default:
            break;
    }
}

void gameState_set(Game *game, uint8_t state, size_t maxActors, size_t maxSceneries, LoopCallback fixedLoopCallback, LoopCallback loopCallback)
{
    // Clean up the current state to free any allocated resources
    game_cleanup(game);

    gameState_init(game, state, maxActors, maxSceneries, fixedLoopCallback, loopCallback);
}

void gameState_setPaused(Game *game)
{
    static uint8_t isPaused = 0;
    static uint8_t lastState = MAIN_MENU;

    // Check if the start button is pressed
    if (game->control->released.start)
    {
        isPaused ^= 1;
    }

    // Determine the new state based on the pause status
    uint8_t newState = isPaused ? MAIN_MENU : GAMEPLAY;

    // Only switch states if the new state is different from the last state
    if (newState != lastState)
    {
        // Clean up the current state to free any allocated resources
        gameState_set(game, newState, core_get_playercount(), 2, NULL, NULL);

    }

    lastState = newState;  // Update the last state to the new state

}

#endif