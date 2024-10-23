#ifndef GAME_CONTROLS_H
#define GAME_CONTROLS_H

void gameControl_setPause(Game *game)
{
    if (game->control.pressed.start && (game->state == GAMEPLAY)) {
        
        game->state = PAUSE;
    }
}

void gameControl_setGameplay(Game *game)
{
    if (game->control.pressed.start && (game->state == PAUSE)) {
        
        game->state = GAMEPLAY;
    }
}


void game_setControlData(Game *game)
{
    gameControl_setGameplay(game);
    gameControl_setPause(game);
}

#endif