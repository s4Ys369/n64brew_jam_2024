#ifndef PLAYER_H
#define PLAYER_H


typedef struct {

    uint8_t id;
    uint8_t actor_id;
    bool died;
    ControllerData control;

} Player;

void player_init(Player* player, uint8_t id,uint8_t actor_id);

void player_init(Player* player, uint8_t id,uint8_t actor_id)
{
    player->id = id;
    player->actor_id = actor_id;
    player->died = false;

}

void player_setControlData(Player* player)
{
    joypad_poll();
    
    for (int i = 0; i < PLAYER_COUNT; i++) {
        if(player[i].died) continue;
        controllerData_getInputs(&player[i].control, i);
    } 
}

#endif