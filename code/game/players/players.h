#ifndef PLAYERS_H
#define PLAYERS_H

#include <libdragon.h>
#include "../actor/actor.h"
#include "../actor/actor_animation.h"
#include "../control/controls.h"


typedef struct {

    Actor* actor;
    ActorAnimation* animation;
    uint8_t score;

} PlayerData;

PlayerData* player_init(PlayerData* data);

PlayerData* player_init(PlayerData* data)
{
    data->actor = (Actor *)malloc(sizeof(Actor));
    data->animation = (ActorAnimation *)malloc(sizeof(ActorAnimation));
    data->score = 0;
    return data;
}

#endif // PLAYERS_H