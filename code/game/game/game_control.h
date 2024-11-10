#ifndef GAME_CONTROLS_H
#define GAME_CONTROLS_H




void game_setControlData(Game *game)
{
    for (uint8_t i = 0; i < PLAYER_COUNT; i++) {

        if (game->control[i].pressed.start) {

            if (game->state == PAUSE) game->state = GAMEPLAY;
            else if (game->state == GAMEPLAY) game->state = PAUSE;
        }
    }
}

#endif