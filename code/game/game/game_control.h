#ifndef GAME_CONTROLS_H
#define GAME_CONTROLS_H




void game_setControlData(Game* game, Player *player)
{
    for (uint8_t i = 0; i < PLAYER_COUNT; i++) {

        if (player[i].control.pressed.start) {

            if (game->state == PAUSE) game->state = GAMEPLAY;
            else if (game->state == GAMEPLAY) game->state = PAUSE;
            else if (game->state == INTRO) game->state = CHARACTER_SELECT;
            else if (game->state == CHARACTER_SELECT) game->state = GAMEPLAY;
            else game->state = GAMEPLAY;
        }
    }
}

#endif