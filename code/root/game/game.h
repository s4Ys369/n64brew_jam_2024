#ifndef GAME_H
#define GAME_H

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

void game_init(Game *game, uint8_t initialState);
void game_loop(Game *game, float deltaTime);
void game_fixedUpdate(Game *game, float fixedDeltaTime);
void game_cleanup(Game *game);


void game_init(Game *game, uint8_t initialState)
{
    // Initialize fields
    game->state = initialState;

    screen_initDisplay(&game->screen);
	screen_initT3dViewport(game->screen.gameplay_viewport, SCISSOR_DEFAULT);
    screen_initT3dViewport(game->screen.main_menu_viewport, SCISSOR_MAIN_MENU);
    screen_initT3dViewport(game->screen.current_viewport, SCISSOR_MAIN_MENU);
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
    t3d_viewport_attach(&game->screen.current_viewport);

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
    camera_getOrbitalPosition(&game->camera, (Vector3){0,0,0}, deltaTime);
    camera_set(&game->camera, &game->screen);

    // Clear and prepare the screen for rendering
    screen_clearDisplay(&game->screen);
    screen_clearT3dViewport(game->screen.current_viewport);

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
    ui_fps();
    ui_printf("STATE %d", game->state);
}

void game_fixedUpdate(Game *game, float fixedDeltaTime)
{
	// Any custom functions to be called at the fixed rate
	if (game->fixedLoopCallback)
        game->fixedLoopCallback(game);

    // Get input data
    controllerData_getInputs(&game->control);
    ui_input_display(&game->control);

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

#endif // GAME_H