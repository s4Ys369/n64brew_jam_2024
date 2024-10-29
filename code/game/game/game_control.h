#ifndef GAME_CONTROLS_H
#define GAME_CONTROLS_H




void game_setControlData(Game *game)
{
    if (game->control[0]->released.start) {

        if (game->state == PAUSE) game->state = GAMEPLAY;
        else if (game->state == GAMEPLAY) game->state = PAUSE;
    }
}

#endif