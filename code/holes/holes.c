#include <libdragon.h>
#include "../../minigame.h"
#include "../../core.h"
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

const MinigameDef minigame_def = {
    .gamename = "holes",
    .developername = "s4ys",
    .description = "Clone of Hole.io",
    .instructions = "Try to devour as much as possible!"
};

#define FONT_TEXT           1
#define FONT_BILLBOARD      2
#define TEXT_COLOR          0x6CBB3CFF
#define TEXT_OUTLINE        0x30521AFF

#define HITBOX_RADIUS       40.f

#define ATTACK_OFFSET       10.f
#define ATTACK_RADIUS       5.f

#define ATTACK_TIME_START   0.333f
#define ATTACK_TIME_END     0.4f

#define COUNTDOWN_DELAY     3.0f
#define GO_DELAY            1.0f
#define WIN_DELAY           5.0f
#define WIN_SHOW_DELAY      2.0f

#define BILLBOARD_YOFFSET   15.0f

/**
 * Simple clone of Hole.io
 * Basically just pushing myself to make something in a shorter time.
 */

surface_t *depthBuffer;
T3DViewport viewport[MAXPLAYERS];
rdpq_font_t *font;
rdpq_font_t *fontBillboard;
T3DMat4FP* mapMatFP;
rspq_block_t *dplMap;
T3DModel *model;
T3DModel *modelCar;
T3DModel *modelBuilding;
T3DModel *modelHydrant;
T3DModel *modelMap;
T3DVec3 camPos[MAXPLAYERS];
T3DVec3 camTarget[MAXPLAYERS];
T3DVec3 lightDirVec;
xm64player_t music;

////////// OBJECTS
enum OBJ_TYPES
{
  OBJ_CAR,
  OBJ_BUILDING,
  OBJ_HYDRANT,
  NUM_OBJ_TYPES
};

typedef struct
{
  uint8_t ID;
  T3DObject *model;
  T3DMat4FP* mtxFP;
  T3DVec3 position;
  T3DVec3 scale;
  float yaw;
  bool visible;
  bool hide;
  rspq_block_t *modelBlock;
} object_data;

#define NUM_OBJECTS 9

typedef struct
{
  uint8_t type;
  uint8_t scoreValue;
  T3DModel *model;
  float collisionRadius;
  object_data objects[NUM_OBJECTS];
} object_type;

object_type objects[NUM_OBJ_TYPES];
//////////

typedef struct
{
  PlyNum plynum;
  T3DMat4FP* modelMatFP;
  rspq_block_t *dplHole;
  T3DVec3 moveDir;
  T3DVec3 playerPos;
  T3DVec3 scale;
  float rotY;
  float currSpeed;
  bool isAttack;
  bool isAlive;
  float attackTimer;
  PlyNum ai_target;
  int ai_reactionspeed;
  uint8_t score;
} player_data;

player_data players[MAXPLAYERS];

float countDownTimer;
bool isEnding;
float endTimer;
PlyNum winner;

wav64_t sfx_start;
wav64_t sfx_countdown;
wav64_t sfx_stop;
wav64_t sfx_winner;

rspq_syncpoint_t syncPoint;


////////// VIEWPORTS

#define SCREEN_WIDTH display_get_width()
#define SCREEN_HEIGHT display_get_height()

void viewport_create(T3DViewport* vp)
{
  switch (core_get_playercount())
	{
		case 1:
			vp[0] = t3d_viewport_create();
			t3d_viewport_set_area(&vp[0],   0,               0,           SCREEN_WIDTH,      SCREEN_HEIGHT);
			break;
		case 2:
			vp[0] = t3d_viewport_create();
			vp[1] = t3d_viewport_create();
			t3d_viewport_set_area(&vp[0], 0,             0,               SCREEN_WIDTH,   SCREEN_HEIGHT/2);
			t3d_viewport_set_area(&vp[1], 0,             SCREEN_HEIGHT/2, SCREEN_WIDTH,   SCREEN_HEIGHT/2);
			break;
		case 3:
			vp[0] = t3d_viewport_create();
			vp[1] = t3d_viewport_create();
			vp[2] = t3d_viewport_create();
			t3d_viewport_set_area(&vp[0], 0,              0,               SCREEN_WIDTH,     SCREEN_HEIGHT/2);
			t3d_viewport_set_area(&vp[1], 0,              SCREEN_HEIGHT/2, SCREEN_WIDTH/2,   SCREEN_HEIGHT/2-2);
			t3d_viewport_set_area(&vp[2], SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2-2, SCREEN_HEIGHT/2-2);
			break;
		case 4:
			vp[0] = t3d_viewport_create();
			vp[1] = t3d_viewport_create();
			vp[2] = t3d_viewport_create();
			vp[3] = t3d_viewport_create();
			t3d_viewport_set_area(&vp[0], 0,              0,               SCREEN_WIDTH/2,   SCREEN_HEIGHT/2);
			t3d_viewport_set_area(&vp[1], SCREEN_WIDTH/2, 0,               SCREEN_WIDTH/2-2, SCREEN_HEIGHT/2);
			t3d_viewport_set_area(&vp[2], 0,              SCREEN_HEIGHT/2, SCREEN_WIDTH/2,   SCREEN_HEIGHT/2-2);
			t3d_viewport_set_area(&vp[3], SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2-2, SCREEN_HEIGHT/2-2);
			break;
	}
}

void viewport_drawScissor()
{
  int sizeX = SCREEN_WIDTH;
	int sizeY = SCREEN_HEIGHT;
	rdpq_set_scissor(0, 0, sizeX, sizeY);
	rdpq_set_mode_standard();
	rdpq_set_mode_fill(RGBA32(0,0,0,255));

	// draw thick lines between the screens
	switch (core_get_playercount())
  {
    case 1:
      break;
    case 2:
      rdpq_fill_rectangle(0, sizeY/2-1, sizeX, sizeY/2+1);
      break;
    case 3:
      rdpq_fill_rectangle(0, sizeY/2-1, sizeX, sizeY/2+1);
      rdpq_fill_rectangle(sizeX/2-1, sizeY/2, sizeX/2+1, sizeY);
      break;
    case 4:
      rdpq_fill_rectangle(0, sizeY/2-1, sizeX, sizeY/2+1);
      rdpq_fill_rectangle(sizeX/2-1, 0, sizeX/2+1, sizeY);
      break;
  }
}
//////////


////////// COLLISION

float vec2_dist_squared(T3DVec3* pos0, T3DVec3* pos1)
{
  // Calculate squared distance between two positions' X and Z coordinates
  float dx = pos1->v[0] - pos0->v[0];
  float dy = pos1->v[2] - pos0->v[2];
  return  dx*dx + dy*dy;
}

bool check_collision(T3DVec3* pos0, float radii0, T3DVec3* pos1, float radii1)
{

  float distSq = vec2_dist_squared(pos0,pos1);

  assert(radii0 >= 0.0f && radii1 >= 0.0f);

  if(radii0 != 0)
  {
    // Assume checking if radii overlap
    float radiiSum = radii0 + radii1;
    float radiiSumSq = radiiSum*radiiSum;

    return distSq <= radiiSumSq;
  } else {

    // Assume checking if pos0 is within pos1's radius
    return distSq <= radii1 * radii1;
  }


}
//////////

////////// OBJECTS


#define GRID_SIZE 144 // BOX_SIZE plus 4 to be neatly divide by 12
#define CELL_SIZE 96
#define NUM_CELLS (GRID_SIZE * 2 / CELL_SIZE)
#define MAX_GRID_POINTS (NUM_CELLS * NUM_CELLS)

T3DVec3 gridPos[MAX_GRID_POINTS];
size_t gridPointCount = 0;

void generate_grid()
{
  gridPointCount = 0;

  // Calculate half the grid size
  int halfGridSize = GRID_SIZE;

  for (int i = 0; i < NUM_CELLS; i++) {
    for (int j = 0; j < NUM_CELLS; j++)
    {
      // Calculate cell center coordinates
      int x = -halfGridSize + j * CELL_SIZE + CELL_SIZE / 2;
      int z = -halfGridSize + i * CELL_SIZE + CELL_SIZE / 2;

      // Store the grid cell center position
      gridPos[gridPointCount++] = (T3DVec3){{(float)x, 10.0f, (float)z}};
    }
  }
}

void object_init(object_data *object, uint8_t objectType, uint8_t ID, T3DVec3 position)
{
  object->ID = ID;
  object->mtxFP = malloc_uncached(sizeof(T3DMat4FP));
  object->position = position;

  switch(objectType) {
    case OBJ_CAR:
      object->scale = (T3DVec3){{0.125f, 0.125f, 0.125f}};
      object->position.v[0] = fmaxf(-GRID_SIZE, fminf(GRID_SIZE, object->position.v[0] + (rand() % 20)));
      object->position.v[2] = fmaxf(-GRID_SIZE, fminf(GRID_SIZE, object->position.v[2] + 40));
      object->yaw = T3D_DEG_TO_RAD(90.0f);
      break;
    case OBJ_BUILDING:
      object->scale = (T3DVec3){{0.3f, 0.3f, 0.3f}};
      object->yaw = 0;
      break;
    case OBJ_HYDRANT:
      object->scale = (T3DVec3){{0.05f, 0.05f, 0.05f}};
      object->position.v[0] = fmaxf(-GRID_SIZE, fminf(GRID_SIZE, object->position.v[0] + 25));
      object->position.v[2] = fmaxf(-GRID_SIZE, fminf(GRID_SIZE, object->position.v[2] + 25));
      object->yaw = 0;
      break;
  }
  

  object->visible = true;
  object->hide = false;
}

void object_initBatch(object_type* batch, uint8_t objectType)
{

  batch->type = objectType;

  // Assign model to objects
  switch(batch->type)
  {
    case OBJ_CAR:
      batch->scoreValue = 2;
      batch->model = modelCar;
      batch->collisionRadius = 0.25f;
      break;
    case OBJ_BUILDING:
      batch->scoreValue = 4;
      batch->model = modelBuilding;
      batch->collisionRadius = 0.5f;
      break;
    case OBJ_HYDRANT:
      batch->scoreValue = 1;
      batch->model = modelHydrant;
      batch->collisionRadius = 0.125f;
      break;
  }

  // Initialize batch objects
  generate_grid();
  for (size_t i = 0; i < NUM_OBJECTS; i++)
  {
    object_init(&batch->objects[i], batch->type, i, gridPos[i]);
  }

  // Create model block
  
    for (size_t i = 0; i < NUM_OBJECTS; i++)
    {
      rspq_block_begin();

        t3d_matrix_set(batch->objects[i].mtxFP, true);
        t3d_model_draw(batch->model);

      batch->objects[i].modelBlock = rspq_block_end();
    }
  

}

void object_updateBatch(object_type* batch, T3DViewport* vp, player_data* player)
{
  for (size_t i = 0; i < NUM_OBJECTS; i++)
  {
    if(!batch->objects[i].visible) continue; // just skip the object update if not visible

    if(check_collision(&batch->objects[i].position, batch->collisionRadius, &player->playerPos, HITBOX_RADIUS*player->scale.x))
    {
      batch->objects[i].position.v[1] -= 0.4f/batch->collisionRadius;
      while(batch->objects[i].position.v[1] <= -80.0f * batch->collisionRadius)
      {
        player->score += batch->scoreValue;
        batch->objects[i].visible = false;
        break;
      }
    }

    t3d_mat4fp_from_srt_euler(
      batch->objects[i].mtxFP,
      batch->objects[i].scale.v,
      (float[3]){0,batch->objects[i].yaw,0},
      batch->objects[i].position.v
    );

  }
}

void object_drawBatch(object_type* batch)
{
  for (size_t i = 0; i < NUM_OBJECTS; i++)
  {
    if(batch->objects[i].visible && !batch->objects[i].hide) rspq_block_run(batch->objects[i].modelBlock);
  }
}

void object_destroyBatch(object_type* batch)
{
  for (size_t i = 0; i < NUM_OBJECTS; i++)
  {
    free_uncached(batch->objects[i].mtxFP);
    rspq_block_free(batch->objects[i].modelBlock);
  }

  t3d_model_free(batch->model);

}

//////////

void player_init(player_data *player, color_t color, T3DVec3 position, float rotation)
{
  player->modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  player->moveDir = (T3DVec3){{0,0,0}};
  player->playerPos = position;
  player->scale = (T3DVec3){{0.125f,0.125f,0.125f}};
  player->score = 0;

  rspq_block_begin();
    t3d_matrix_set(player->modelMatFP, true);
    rdpq_set_prim_color(color);
    t3d_model_draw(model);
  player->dplHole = rspq_block_end();

  player->rotY = rotation;
  player->currSpeed = 0.0f;
  player->isAttack = true;
  player->isAlive = true;
  player->ai_target = rand()%NUM_OBJECTS;
  player->ai_reactionspeed = (2-core_get_aidifficulty())*5 + rand()%((3-core_get_aidifficulty())*3);
}

void minigame_init(void)
{
  const color_t colors[] = {
    PLAYERCOLOR_1,
    PLAYERCOLOR_2,
    PLAYERCOLOR_3,
    PLAYERCOLOR_4,
  };

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  depthBuffer = display_get_zbuf();

  t3d_init((T3DInitParams){});

  font = rdpq_font_load("rom:/snake3d/m6x11plus.font64");
  rdpq_text_register_font(FONT_TEXT, font);
  rdpq_font_style(font, 0, &(rdpq_fontstyle_t){.color = color_from_packed32(TEXT_COLOR) });

  fontBillboard = rdpq_font_load("rom:/squarewave.font64");
  rdpq_text_register_font(FONT_BILLBOARD, fontBillboard);
  for (size_t i = 0; i < MAXPLAYERS; i++)
  {
    rdpq_font_style(fontBillboard, i, &(rdpq_fontstyle_t){ .color = colors[i] });
  }

  viewport_create(viewport);

  mapMatFP = malloc_uncached(sizeof(T3DMat4FP));
  t3d_mat4fp_from_srt_euler(mapMatFP, (float[3]){0.3f, 0.3f, 0.3f}, (float[3]){0, 0, 0}, (float[3]){0, 0, -10});

  lightDirVec = (T3DVec3){{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  modelMap = t3d_model_load("rom:/holes/map.t3dm");
  modelCar = t3d_model_load("rom:/holes/car.t3dm");
  modelBuilding = t3d_model_load("rom:/holes/building.t3dm");
  modelHydrant = t3d_model_load("rom:/holes/hydrant.t3dm");
  model = t3d_model_load("rom:/holes/hole.t3dm");

  rspq_block_begin();
    t3d_matrix_set(mapMatFP, true);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw(modelMap);
  dplMap = rspq_block_end();

  T3DVec3 start_positions[] = {
    (T3DVec3){{-50,5,-50}},
    (T3DVec3){{50,5,-50}},
    (T3DVec3){{50,5,50}},
    (T3DVec3){{-50,5,50}},
  };

  float start_rotations[] = {
    M_PI/2,
    0,
    3*M_PI/2,
    M_PI
  };

  for (size_t i = 0; i < MAXPLAYERS; i++)
  {
    player_init(&players[i], colors[i], start_positions[i], start_rotations[i]);
    players[i].plynum = i;
    camPos[i] = (T3DVec3){{players[i].playerPos.x, players[i].playerPos.y+150.0f, players[i].playerPos.z+100.0f}};
    camTarget[i] = players[i].playerPos;
  }

  for (int i = 0; i < NUM_OBJ_TYPES; i++)
  {
    object_initBatch(&objects[i], i);
  }

  countDownTimer = COUNTDOWN_DELAY;

  syncPoint = 0;
  wav64_open(&sfx_start, "rom:/core/Start.wav64");
  wav64_open(&sfx_countdown, "rom:/core/Countdown.wav64");
  wav64_open(&sfx_stop, "rom:/core/Stop.wav64");
  wav64_open(&sfx_winner, "rom:/core/Winner.wav64");
  xm64player_open(&music, "rom:/snake3d/bottled_bubbles.xm64");
  xm64player_play(&music, 0);
  mixer_ch_set_vol(31, 0.5f, 0.5f);

  if(core_get_playercount() > 1)
  {
    display_set_fps_limit(display_get_refresh_rate()*0.5f);
  } else {
    display_set_fps_limit(0);
  }
}


void player_do_damage(player_data *player)
{
  if (!player->isAlive) {
    // Prevent edge cases
    return;
  }

  for (size_t i = 0; i < MAXPLAYERS; i++)
  {
    player_data *other_player = &players[i];
    if (other_player == player || !other_player->isAlive || other_player->scale.x > player->scale.x) continue;

    if (check_collision(&other_player->playerPos, 0, &player->playerPos, HITBOX_RADIUS*player->scale.x))
    {
      other_player->isAlive = false;
      player->score += 5.0f;
    }
  }

  player->isAttack = false;
}

bool player_has_control(player_data *player)
{
  return player->isAlive && countDownTimer < 0.0f;
}

void player_fixedloop(player_data *player, object_type* objects, float deltaTime, joypad_port_t port, bool is_human)
{
  float speed = 0.0f;
  T3DVec3 newDir = {0};

  if (player_has_control(player)) {
    if (is_human) {
      joypad_inputs_t joypad = joypad_get_inputs(port);

      // @TODO: add D Pad support
      newDir.v[0] = (float)joypad.stick_x * 0.05f;
      newDir.v[2] = -(float)joypad.stick_y * 0.05f;
      speed = sqrtf(t3d_vec3_len2(&newDir));
    } else {
      if(objects->collisionRadius <= player->scale.x) 
      {
        object_data* target = &objects->objects[player->ai_target];
        if (target->visible) { // Check for a valid target
          // Move towards the direction of the target
          float dist, norm;
          newDir.v[0] = (target->position.v[0] - player->playerPos.v[0]);
          newDir.v[2] = (target->position.v[2] - player->playerPos.v[2]);
          dist = sqrtf(newDir.v[0]*newDir.v[0] + newDir.v[2]*newDir.v[2]);
          if(dist==0) dist = 1;
          norm = 1/dist;
          newDir.v[0] *= norm + (0.05f * core_get_aidifficulty());
          newDir.v[2] *= norm + (0.05f * core_get_aidifficulty());
          speed = 200;

        } else {
          player->ai_target = rand()%NUM_OBJECTS; // (Attempt) to aquire a new target this frame
        }
      }
    }
  }

  // Player movement
  if(speed > 0.15f) {
    newDir.v[0] /= speed;
    newDir.v[2] /= speed;
    player->moveDir = newDir;

    float newAngle = atan2f(player->moveDir.v[0], player->moveDir.v[2]);
    player->rotY = t3d_lerp_angle(player->rotY, newAngle, 0.5f);
    player->currSpeed = t3d_lerp(player->currSpeed, speed * 0.09f, 0.15f);
  }

  // move player...
  player->playerPos.v[0] += player->moveDir.v[0] * player->currSpeed;
  player->playerPos.v[2] += player->moveDir.v[2] * player->currSpeed;
  // ...and limit position inside the box
  const float BOX_SIZE = 140.0f;
  if(player->playerPos.v[0] < -BOX_SIZE)player->playerPos.v[0] = -BOX_SIZE;
  if(player->playerPos.v[0] >  BOX_SIZE)player->playerPos.v[0] =  BOX_SIZE;
  if(player->playerPos.v[2] < -BOX_SIZE)player->playerPos.v[2] = -BOX_SIZE;
  if(player->playerPos.v[2] >  BOX_SIZE)player->playerPos.v[2] =  BOX_SIZE;

  player_do_damage(player);

  // Scaling based on score
  if(player->score >= 2 && player->score < 6)
  {
    player->scale.v[0] = 0.25f;
    player->scale.v[2] = 0.25f;
  } else if (player->score >= 6 && player->score < 10) {
    player->scale.v[0] = 0.5f;
    player->scale.v[2] = 0.5f;
  } else if (player->score >= 10) {
    player->scale.v[0] = 1.0f;
    player->scale.v[2] = 1.0f;
  }
  
}

void player_loop(player_data *player, float deltaTime, joypad_port_t port, bool is_human)
{
  if (is_human && player_has_control(player))
  {
    joypad_buttons_t btn = joypad_get_buttons_pressed(port);

    if (btn.start) minigame_end();

  }

  if(syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame

  // Update player matrix
  t3d_mat4fp_from_srt_euler(player->modelMatFP,
    player->scale.v,
    (float[3]){0.0f, -player->rotY, 0},
    player->playerPos.v
  );
}

void player_draw(player_data *player)
{
  if (player->isAlive) {
    rspq_block_run(player->dplHole);
  }
}

void player_draw_billboard(player_data *player, PlyNum playerNum)
{
  if (!player->isAlive) return;

  T3DVec3 billboardPos = (T3DVec3){{
    player->playerPos.v[0],
    player->playerPos.v[1] + BILLBOARD_YOFFSET,
    player->playerPos.v[2]
  }};

  T3DVec3 billboardScreenPos;
  t3d_viewport_calc_viewspace_pos(&viewport[playerNum], &billboardScreenPos, &billboardPos);

  int x = floorf(billboardScreenPos.v[0]);
  int y = floorf(billboardScreenPos.v[1]);

  rdpq_sync_pipe(); // Hardware crashes otherwise
  rdpq_sync_tile(); // Hardware crashes otherwise

  rdpq_text_printf(&(rdpq_textparms_t){ .style_id = playerNum }, FONT_BILLBOARD, x-5, y-16, "P%d", playerNum+1);
}

void minigame_fixedloop(float deltaTime)
{
  bool controlbefore = player_has_control(&players[0]);
  uint32_t playercount = core_get_playercount();
  for (size_t i = 0; i < MAXPLAYERS; i++)
  {
    for (int j = 0; j < NUM_OBJ_TYPES; j++)
    {
      player_fixedloop(&players[i], &objects[j], deltaTime, core_get_playercontroller(i), i < playercount);
    }
  }

  if (countDownTimer > -GO_DELAY)
  {
    float prevCountDown = countDownTimer;
    countDownTimer -= deltaTime;
    if ((int)prevCountDown != (int)countDownTimer && countDownTimer >= 0)
      wav64_play(&sfx_countdown, 31);
  }
  if (!controlbefore && player_has_control(&players[0]))
    wav64_play(&sfx_start, 31);

  if (!isEnding) {
    // Determine if a player has won
    uint32_t alivePlayers = 0;
    PlyNum lastPlayer = 0;
    for (size_t i = 0; i < MAXPLAYERS; i++)
    {
      if (players[i].isAlive)
      {
        alivePlayers++;
        lastPlayer = i;
      }
    }
    
    if (alivePlayers == 1) {
      isEnding = true;
      winner = lastPlayer;
      wav64_play(&sfx_stop, 31);
    }
  } else {
    float prevEndTime = endTimer;
    endTimer += deltaTime;
    if ((int)prevEndTime != (int)endTimer && (int)endTimer == WIN_SHOW_DELAY)
        wav64_play(&sfx_winner, 31);
    if (endTimer > WIN_DELAY) {
      core_set_winner(winner);
      minigame_end();
    }
  }
}

void minigame_loop(float deltaTime)
{
  uint8_t colorAmbient[4] = {0xAA, 0xAA, 0xAA, 0xFF};
  uint8_t colorDir[4]     = {0xFF, 0xAA, 0xAA, 0xFF};

  uint32_t playercount = core_get_playercount();
  for (size_t i = 0; i < playercount; i++)
  {
    switch (playercount)
    {
      case 1:
      case 4:
        t3d_viewport_set_projection(&viewport[i], T3D_DEG_TO_RAD(30.0f), 5.0f, 300.0f);
        break;
      case 2:
        t3d_viewport_set_projection(&viewport[i], T3D_DEG_TO_RAD(20.0f), 5.0f, 300.0f);
        break;
      case 3:
        if(i > 0)
        {
          t3d_viewport_set_projection(&viewport[i], T3D_DEG_TO_RAD(30.0f), 5.0f, 300.0f);
        } else {
          t3d_viewport_set_projection(&viewport[i], T3D_DEG_TO_RAD(20.0f), 5.0f, 300.0f);
        }
        break;
    }
    
    t3d_viewport_look_at(&viewport[i], &camPos[i], &camTarget[i], &(T3DVec3){{0,1,0}});

    for (int j = 0; j < NUM_OBJ_TYPES; j++)
    {
      for (size_t p = 0; p < MAXPLAYERS; p++)
      {
        object_updateBatch(&objects[j], &viewport[i], &players[p]);
      }
    }

    camPos[i] = (T3DVec3){{players[i].playerPos.x, players[i].playerPos.y+150.0f, players[i].playerPos.z+100.0f}};
    camTarget[i] = players[i].playerPos;
  }

  for (size_t p = 0; p < MAXPLAYERS; p++)
  {
    player_loop(&players[p], deltaTime, core_get_playercontroller(p), p < playercount);
  }
  

  // ======== Draw (3D) ======== //

  // @TODO: Splitscreen?
  rdpq_attach(display_get(), depthBuffer);

  for (size_t i = 0; i < playercount; i++)
  {
    t3d_frame_start();
    t3d_viewport_attach(&viewport[i]);

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    t3d_matrix_push_pos(1);

    rdpq_mode_zbuf(false, true);
    rspq_block_run(dplMap);

    rdpq_mode_zbuf(false, false);
    for (size_t p = 0; p < MAXPLAYERS; p++)
    {
      player_draw(&players[p]);
    }


    rdpq_mode_zbuf(true, false);
    for (int j = 0; j < NUM_OBJ_TYPES; j++)
    {
      
      for (int o = 0; o < NUM_OBJECTS; o++)
      {
        if(t3d_frustum_vs_sphere(&viewport[i].viewFrustum, &gridPos[o], 50.0f * objects[j].collisionRadius))
        {
          objects[j].objects[o].hide = false;
        } else {
          objects[j].objects[o].hide = true;
        }
      }

      object_drawBatch(&objects[j]);
    }

    t3d_matrix_pop(1);

    player_draw_billboard(&players[i], i);
  }

  syncPoint = rspq_syncpoint_new();

  rdpq_sync_tile();
  rdpq_sync_pipe(); // Hardware crashes otherwise

  viewport_drawScissor();

  // @TODO: Print Score
  if (countDownTimer > 0.0f) {
    rdpq_text_printf(NULL, FONT_TEXT, 155, 100, "%d", (int)ceilf(countDownTimer));
  } else if (countDownTimer > -GO_DELAY) {
    rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, };
    rdpq_text_print(&textparms, FONT_TEXT, 0, 100, "GO!");
  } else if (isEnding && endTimer >= WIN_SHOW_DELAY) {
    rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, };
    rdpq_text_printf(&textparms, FONT_TEXT, 0, 100, "Player %d wins!", winner+1);
  }

  rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, };
  rdpq_text_printf(&textparms, FONT_BILLBOARD, 0, 210, "FPS %.3f", display_get_fps());

  rdpq_detach_show();
}

void player_cleanup(player_data *player)
{
  rspq_block_free(player->dplHole);
  free_uncached(player->modelMatFP);
}

void minigame_cleanup(void)
{

  display_set_fps_limit(0);

  for (size_t i = 0; i < MAXPLAYERS; i++)
  {
    player_cleanup(&players[i]);
  }

  for (int i = 0; i < NUM_OBJ_TYPES; i++)
  {
    object_destroyBatch(&objects[i]);
  }

  wav64_close(&sfx_start);
  wav64_close(&sfx_countdown);
  wav64_close(&sfx_stop);
  wav64_close(&sfx_winner);
  xm64player_stop(&music);
  xm64player_close(&music);
  rspq_block_free(dplMap);

  t3d_model_free(model);
  t3d_model_free(modelMap);

  free_uncached(mapMatFP);

  rdpq_text_unregister_font(FONT_BILLBOARD);
  rdpq_font_free(fontBillboard);
  rdpq_text_unregister_font(FONT_TEXT);
  rdpq_font_free(font);
  t3d_destroy();

  display_close();
}
