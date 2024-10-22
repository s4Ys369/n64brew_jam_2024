#ifndef PLAYERS_H
#define PLAYERS_H

#include <libdragon.h>
#include "../actor/actor.h"
#include "../actor/actor_animation.h"
#include "../control/controls.h"


typedef struct {

    ControllerData controller;
    Actor actor;
    ActorAnimation animation;
    uint8_t score;

} PlayerData;

PlayerData* player_init(PlayerData* data);

PlayerData* player_init(PlayerData* data)
{
    data->controller = (ControllerData){0};
    data->actor = (Actor){0};
    data->animation = (ActorAnimation){0};
    data->score = 0;
    return data;
}

#endif // PLAYERS_H