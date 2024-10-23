#ifndef PLAYERS_H
#define PLAYERS_H

#include <libdragon.h>
#include "../actor/actor.h"
#include "../actor/actor_animation.h"
#include "../control/controls.h"


typedef struct {
    uint32_t port;
    Actor* actor;
    ActorAnimation* animation;
    uint8_t score;

} PlayerData;

PlayerData* player_init(uint32_t num, PlayerData* data);

PlayerData* player_init(uint32_t num, PlayerData* data)
{
    data->port = core_get_playercontroller(num);
    data->actor = (Actor *)malloc(sizeof(Actor));
    data->animation = (ActorAnimation *)malloc(sizeof(ActorAnimation));
    data->score = 0;
    return data;
}

#endif // PLAYERS_H