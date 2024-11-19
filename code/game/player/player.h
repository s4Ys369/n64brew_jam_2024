#ifndef PLAYER_H
#define PLAYER_H


typedef struct {

    Vector3 position;
    uint8_t id;
    uint8_t actor_id;
    bool died;
    ControllerData control;

} Player;

void player_init(Player* player, uint8_t id,uint8_t actor_id);

void player_init(Player* player, uint8_t id,uint8_t actor_id)
{
    player->position = (Vector3){0,0,0};
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

// Modified from `player_draw_billboard` in Snake3D example
Vector3 player_getBillboard(Player* player, T3DViewport* viewport)
{
    Vector3 result = (Vector3){0,0,0};
    if (player->died) return result;

    Vector3 billboardPos = (Vector3){
      player->position.x,
      player->position.y,
      player->position.z + 250.0f
    };

    T3DVec3 billboardPosConvert = Vector3_to_T3DVec3(billboardPos);

    T3DVec3 billboardScreenPos;
    t3d_viewport_calc_viewspace_pos(viewport, &billboardScreenPos, &billboardPosConvert);

    int x = floorf(billboardScreenPos.v[0]);
    int y = floorf(billboardScreenPos.v[2]);
    int z = floorf(billboardScreenPos.v[1]);

    result = (Vector3){x,y,z};
    return result;
}

#endif