#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#include "../../core.h"
#include "../../minigame.h"

#include "screen/screen.h"
#include "control/controls.h"
#include "time/time.h"

#include "game/game_states.h"

#include "physics/physics.h"

#include "camera/camera.h"
#include "camera/camera_states.h"
#include "camera/camera_control.h"

#include "actor/actor.h"
#include "actor/actor_states.h"
#include "actor/actor_motion.h"
#include "actor/actor_control.h"
#include "actor/actor_animation.h"

#include "scene/scenery.h"

#include "ui/ui.h"


const MinigameDef minigame_def = {
    .gamename = "Fall Guys Clone",
    .developername = "zoncabe, float4, s4ys, kaelin",
    .description = "This is a game that exist.",
    .instructions = "Press A to win."
};


typedef struct Game Game;
typedef void (*LoopCallback)(Game *game);

struct Game
{
    uint8_t state;
    Screen screen;
    ControllerData control;
    TimeData timing;
    Camera camera;
    LightData light;
    
    Actor *actors;           // Dynamic array for actors
    ActorAnimation *animations;
    size_t actor_count;

    Scenery *sceneries;       // Dynamic array for sceneries
    size_t scenery_count;

    LoopCallback fixedLoopCallback;  // Fixed loop callback
    LoopCallback loopCallback;       // Main loop callback
};

rspq_syncpoint_t syncPoint;

void game_init(Game *game, uint8_t initialState)
{
    // Initialize fields
    game->state = initialState;

    screen_initDisplay(&game->screen);
	screen_initT3dViewport(&game->screen);

    controllerData_getInputs(&game->control);

	time_init(&game->timing);

    game->camera = camera_create();
    game->light = light_create();

    // Allocate memory for dynamic arrays
    game->actors = (Actor *)malloc(sizeof(Actor));
    game->animations = (ActorAnimation *)malloc(sizeof(ActorAnimation));
    game->sceneries = (Scenery *)malloc(sizeof(Scenery));

    // Set initial actor and scenery counts
    game->actor_count = 0;
    game->scenery_count = 0;

    // Assign callback functions
    game->fixedLoopCallback = NULL;
    game->loopCallback = NULL;

	syncPoint = 0;
}

void game_loop(Game *game, float deltaTime)
{
	// Any custom functions to be called in the loop
	if (game->loopCallback)
        game->loopCallback(game);

    // Update time data
    time_setData(&game->timing);
    
    // Update actor motions and animations
    for(size_t a = 0; a < game->actor_count; ++a)
    {
        actor_setMotion(&game->actors[a], deltaTime);
        actor_setAnimation(&game->actors[a], &game->animations[a], deltaTime, &syncPoint);
    }
    // Update camera position and set camera to screen
    cameraControl_setOrbitalMovement(&game->camera, &game->control);
    camera_getMinigamePosition(&game->camera, game->actors[0].body.position, deltaTime);
    camera_set(&game->camera, &game->screen);

    // Clear and prepare the screen for rendering
    screen_clearDisplay(&game->screen);
    screen_clearT3dViewport(&game->screen);

    // Set lighting
    light_set(&game->light);

    // Render the scene (scenery and actors)
    t3d_matrix_push_pos(1);
    for (size_t i = 0; i < game->scenery_count; i++)
        scenery_draw(&game->sceneries[i]);
    for (size_t i = 0; i < game->actor_count; i++)
        actor_draw(&game->actors[i]);
    t3d_matrix_pop(1);

    // Sync the RSP and draw the UI
    syncPoint = rspq_syncpoint_new();
    ui_draw();
    rdpq_detach_show();
}

void game_fixedUpdate(Game *game, float fixedDeltaTime)
{
	// Any custom functions to be called at the fixed rate
	if (game->fixedLoopCallback)
        game->fixedLoopCallback(game);

    // Get input data
    controllerData_getInputs(&game->control);

    // Update player controls, state, and apply physics
    actor_setControlData(&game->actors[0], &game->control, fixedDeltaTime, game->camera.angle_around_barycenter, game->camera.offset_angle);
    actor_setState(&game->actors[0], game->actors[0].state);

    // Update scenery state
    for (size_t i = 0; i < game->scenery_count; i++)
        scenery_set(&game->sceneries[i]);
}

void game_cleanup(Game *game)
{
    // Destroy armatures
    for (size_t i = 0; i < game->actor_count; i++)
    {
        t3d_skeleton_destroy(&game->actors[i].armature.main);
        t3d_skeleton_destroy(&game->actors[i].armature.blend);
    }

    // Free models and scenery
    for (size_t i = 0; i < game->scenery_count; i++)
        t3d_model_free(game->sceneries[i].model);
    for (size_t i = 0; i < game->actor_count; i++)
        t3d_model_free(game->actors[i].model);

	// Free dynamically allocated memory
    if (game->actors)
    {
        free(game->actors);
        game->actors = NULL;
    }
    
    if (game->animations)
    {
        free(game->animations);
        game->animations = NULL;
    }

    if (game->sceneries)
    {
        free(game->sceneries);
        game->sceneries = NULL;
    }

	// Reset syncPoint
    syncPoint = 0;
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

		default:
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
    }

    // Set the game's state
    game->state = state;
}

void gameState_set(Game *game, uint8_t state, size_t maxActors, size_t maxSceneries, LoopCallback fixedLoopCallback, LoopCallback loopCallback)
{
    // Clean up the current state to free any allocated resources
    game_cleanup(game);

    gameState_init(game, state, maxActors, maxSceneries, fixedLoopCallback, loopCallback);

    // Set the new state
    game->state = state;
}

static uint8_t isPaused = 0;
static uint8_t lastState = GAMEPLAY;

void game_setPaused(Game *game)
{
    // Check if the start button is pressed
    if (game->control.pressed.start)
    {
        isPaused ^= 1;  // Toggle the paused state
    }

    // Determine the new state based on the pause status
    uint8_t newState = isPaused ? MAIN_MENU : GAMEPLAY;

    // Only switch states if the new state is different from the last state
    if (newState != lastState)
    {
        // Clean up the current state to free any allocated resources
        gameState_set(game, newState, newState == MAIN_MENU ? 1 : core_get_playercount(), 2, NULL, NULL);
        lastState = newState;  // Update the last state to the new state
    }
}

/// actual main.c

Game minigame = {
	.state = GAMEPLAY
};

void minigame_init()
{
	debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

	dfs_init(DFS_DEFAULT_LOCATION);
	rdpq_init();
	joypad_init();
	t3d_init((T3DInitParams){});

	joypad_init();

	time_init(&minigame.timing);
	ui_init();

	game_init(&minigame, minigame.state);
	gameState_set(&minigame, minigame.state, core_get_playercount(), 2, NULL, NULL);

}

void minigame_fixedloop(float deltaTime)
{
	game_fixedUpdate(&minigame, deltaTime);
	game_setPaused(&minigame);
}

void minigame_loop(float deltaTime)
{	
	game_loop(&minigame, deltaTime);
	
}

void minigame_cleanup()
{
	game_cleanup(&minigame);
	t3d_destroy();
	// TODO: find and free everything dangling
	display_close();
}
