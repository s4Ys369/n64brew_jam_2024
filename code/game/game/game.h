#ifndef GAME_H
#define GAME_H

#define INTRO 0
#define MAIN_MENU 1
#define GAMEPLAY 2
#define PAUSE 3
#define GAME_OVER 4


typedef struct
{

    uint8_t state;
    Screen screen;
    ControllerData* control[MAXPLAYERS];
    TimeData timing;
  	rspq_syncpoint_t syncPoint;

    Scene scene;

}Game;


void game_init(Game *game);


void game_init(Game *game)
{
    debug_init_isviewer();
	debug_init_usblog();
	asset_init_compression(2);

	dfs_init(DFS_DEFAULT_LOCATION);
	rdpq_init();

	screen_initDisplay(&game->screen);
	screen_initT3dViewport(&game->screen);

	t3d_init((T3DInitParams){});

	joypad_init();

	time_init(&game->timing);

    scene_init(&game->scene);
	ui_init();
	sound_load();

	for (size_t p = 0; p < MAXPLAYERS; ++p)
	{
		game->control[p] = malloc(sizeof(ControllerData));
	}

  	game->syncPoint = 0;
	game->state = GAMEPLAY;
}


#endif