#ifndef GAME_H
#define GAME_H



enum GAME_STATES {
	INTRO,
	MAIN_MENU,
	CHARACTER_SELECT,
	GAMEPLAY,
	PAUSE,
	GAME_OVER
};


typedef struct
{

    uint8_t state;
    Screen screen;
    TimeData timing;
  	rspq_syncpoint_t syncPoint;

    Scene scene;

}Game;


void game_init(Game *game);


void game_init(Game *game)
{

	screen_initDisplay(&game->screen);
	screen_initT3dViewport(&game->screen);

	t3d_init((T3DInitParams){});

	time_init(&game->timing);

    scene_init(&game->scene);

	//
	ui_init();
	sound_load();
	//
	
  	game->syncPoint = 0;
	game->state = INTRO;
}


#endif