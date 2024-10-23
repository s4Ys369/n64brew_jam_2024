#ifndef GAME_H
#define GAME_H

typedef struct
{

    uint8_t state;
    Screen screen;
    ControllerData control;
    TimeData timing;

}Game;


#endif