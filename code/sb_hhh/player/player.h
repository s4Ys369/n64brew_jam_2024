#ifndef PLAYER_H
#define PLAYER_H


typedef struct {

    Vector3 position;
    uint8_t id;
    uint8_t actor_id;
    bool died;
    bool isHuman;
    bool deathCounted;
    ControllerData control;

} Player;

void player_init(Player* player, uint8_t id,uint8_t actor_id);

void player_init(Player* player, uint8_t id,uint8_t actor_id)
{
    player->position = (Vector3){0,0,0};
    player->id = id;
    player->actor_id = actor_id;
    player->died = false;
    player->isHuman = false;
    player->deathCounted = false;

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
      player->position.z + 200.0f
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

void player_drawShadow(Vector3 position, T3DViewport* viewport)
{

    Vector3 billboardPos = (Vector3){
      position.x,
      position.y,
      275 // Lock at top height of the platforms
    };

    T3DVec3 billboardPosConvert = Vector3_to_T3DVec3(billboardPos);

    T3DVec3 billboardScreenPos;
    t3d_viewport_calc_viewspace_pos(viewport, &billboardScreenPos, &billboardPosConvert);

    int x = floorf(billboardScreenPos.v[0]);
    int y = floorf(billboardScreenPos.v[1]);
    int offset = 3; // Size (n * n) in pixels

    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(RGBA32(0,0,0,100)); // Turn down alpha to hid when over lava
    rdpq_fill_rectangle(x-offset,y-offset,x+offset,y+offset);
}
#endif