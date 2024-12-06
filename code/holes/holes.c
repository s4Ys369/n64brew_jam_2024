#include <libdragon.h>
#include "../../minigame.h"
#include "../../core.h"
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>


/**
* Simple clone of Hole.io
* Basically just pushing myself to make something in a shorter time.
*/

const MinigameDef minigame_def = {
  .gamename = "holes",
  .developername = "Strawberry Byte: s4ys",
  .description = "Clone of Hole.io",
  .instructions = "Try to devour as much as possible!"
};

// Global subsystems
#include "util.h"
#include "sound.h"

// Global variables
float countDownTimer;
bool isEnding;
float endTimer;
PlyNum winner;
rspq_syncpoint_t syncPoint;

// Locals
T3DModel *model;
T3DModel *modelCar;
T3DModel *modelBuilding;
T3DModel *modelHydrant;
T3DVec3 lightDirVec;

////////// MAP
#include "map.h"
map_data map;

////////// CAMERA
camera_data cam[MAXPLAYERS];

////////// OBJECTS
#include "objects.h"
object_type objects[NUM_OBJ_TYPES];
T3DVec3 gridPos[MAX_GRID_POINTS];
size_t gridPointCount = 0;
T3DObject* buildings[2];
bool spray[NUM_OBJECTS] = {false};
bool stop[NUM_OBJECTS] = {false};

////////// PLAYERS
#include "player.h"
player_data players[MAXPLAYERS];

////////// TEXT
#include "ui.h"

////////// VIEWPORTS
#include "screen.h"
surface_t *depthBuffer;
T3DViewport viewport[MAXPLAYERS];


void minigame_init(void)
{

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  depthBuffer = display_get_zbuf();

  t3d_init((T3DInitParams){});

  ui_init();

  viewport_create(viewport);

  lightDirVec = (T3DVec3){{0.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  map_init(&map);

////////// OBJECTS
  modelCar = t3d_model_load("rom:/holes/car.t3dm");
  modelBuilding = t3d_model_load("rom:/holes/building.t3dm");
  modelHydrant = t3d_model_load("rom:/holes/hydrant.t3dm");

  for (int i = 0; i < NUM_OBJ_TYPES; i++)
  {
    object_initBatch(&objects[i], i);
  }

////////// PLAYERS
  model = t3d_model_load("rom:/holes/hole.t3dm");

  T3DVec3 start_positions[] = {
    (T3DVec3){{-50,5,-52}},
    (T3DVec3){{52,5,-52}},
    (T3DVec3){{-50,5,40}},
    (T3DVec3){{52,5,40}},
  };

  float start_rotations[] = {
    M_PI/2,
    0,
    3*M_PI/2,
    M_PI
  };

  const color_t colors[] = {
    PLAYERCOLOR_1,
    PLAYERCOLOR_2,
    PLAYERCOLOR_3,
    PLAYERCOLOR_4,
  };

  for (size_t i = 0; i < MAXPLAYERS; i++)
  {
    player_init(&players[i], colors[i], start_positions[i], start_rotations[i]);
    players[i].plynum = i;
    cam[i].position = (T3DVec3){{players[i].playerPos.x, players[i].playerPos.y+150.0f, players[i].playerPos.z+100.0f}};
    cam[i].target = players[i].playerPos;
  }
////////// 

  countDownTimer = COUNTDOWN_DELAY;

  syncPoint = 0;

  // Decide whether to limit FPS and/or disable background music
  switch(core_get_playercount())
  {
    case 1:
      sound_load(true);
      break;
    case 2:
    case 3:
      sound_load(true);
      display_set_fps_limit(display_get_refresh_rate()*0.5f);
      break;
    case 4:
      sound_load(false);
      display_set_fps_limit(display_get_refresh_rate()*0.5f);
      break;
  }

}

void minigame_fixedloop(float deltaTime)
{

////////// PLAYERS
  bool controlbefore = player_has_control(&players[0]);
  uint32_t playercount = core_get_playercount();
  for (size_t i = 0; i < MAXPLAYERS; i++)
  {
    for (int j = 0; j < NUM_OBJ_TYPES; j++)
    {
      player_fixedloop(&players[i], &objects[j], deltaTime, core_get_playercontroller(i), i < playercount);
    }
  }
//////////

  sound_setChannels();
  if (countDownTimer > -GO_DELAY)
  {
    float prevCountDown = countDownTimer;
    countDownTimer -= deltaTime;
    if ((int)prevCountDown != (int)countDownTimer && countDownTimer >= 0)
      sound_wavPlay(SFX_COUNTDOWN, false);
  }
  if (!controlbefore && player_has_control(&players[0]))
    sound_wavPlay(SFX_START, false);

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
      sound_xmStop();
      sound_wavClose(SFX_BUILDING);
      sound_wavPlay(SFX_STOP, false);
    }
  } else {
    float prevEndTime = endTimer;
    endTimer += deltaTime;
    if ((int)prevEndTime != (int)endTimer && (int)endTimer == WIN_SHOW_DELAY)
        sound_wavPlay(SFX_WINNER, false);
    if (endTimer > WIN_DELAY) {
      core_set_winner(winner);
      minigame_end();
    }
  }
}

void minigame_loop(float deltaTime)
{
  uint8_t colorAmbient[4] = {54, 40, 47, 0xFF};
  uint8_t colorDir[4]     = {0xFF, 0xAA, 0xAA, 0xFF};

  uint32_t playercount = core_get_playercount();

  viewport_set(viewport, playercount, cam);

  for (size_t i = 0; i < playercount; i++)
  {

    for (int j = 0; j < NUM_OBJ_TYPES; j++)
    {
      for (size_t p = 0; p < MAXPLAYERS; p++)
      {
        object_updateBatch(&objects[j], &viewport[i], &players[p]);
      }
    }

    cam[i].position = (T3DVec3){{players[i].playerPos.x, players[i].playerPos.y+250.0f, players[i].playerPos.z+100.0f}};
    cam[i].target = players[i].playerPos;
  }

  for (size_t p = 0; p < MAXPLAYERS; p++)
  {
    player_loop(&players[p], deltaTime, core_get_playercontroller(p), p < playercount);
  }
  

  // ======== Draw (3D) ======== //

  rdpq_attach(display_get(), depthBuffer);
  rdpq_clear(RGBA32(0,0,0,255));

  for (size_t i = 0; i < playercount; i++)
  {
    t3d_frame_start();
    t3d_viewport_attach(&viewport[i]);

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    t3d_matrix_push_pos(1);

    rdpq_mode_zbuf(false, true);
    rspq_block_run(map.block);

    for (size_t p = 0; p < MAXPLAYERS; p++)
    {
      player_draw(&players[p]);
    }

    rdpq_mode_zbuf(true, false);

    for (int j = 0; j < NUM_OBJ_TYPES; j++)
    {
      object_cull(&objects[j], &viewport[i], playercount);
    }

    object_drawBatch(&objects[OBJ_HYDRANT]);
    for (size_t o = 0; o < NUM_OBJECTS; o++)
    {
      if(spray[o] && !objects[OBJ_HYDRANT].objects[o].hide) hydrant_water_spray(objects[OBJ_HYDRANT].objects[o].position, &viewport[i]);
    }

    t3d_frame_start();
    object_drawBatch(&objects[OBJ_BUILDING]);
    object_drawBatch(&objects[OBJ_CAR]);

    t3d_matrix_pop(1);

  }

  syncPoint = rspq_syncpoint_new();

  // ======== Draw (2D) ======== //
  rdpq_sync_tile();
  rdpq_sync_pipe(); // Hardware crashes otherwise

  viewport_drawScissor();

  ui_print(true);

  rdpq_detach_show();

}

void minigame_cleanup(void)
{

  sound_cleanup();

  display_set_fps_limit(0);

  for (size_t i = 0; i < MAXPLAYERS; i++)
  {
    player_cleanup(&players[i]);
  }

  for (int i = 0; i < NUM_OBJ_TYPES; i++)
  {
    object_destroyBatch(&objects[i]);
  }

  t3d_model_free(model);

  map_cleanup(&map);

  ui_cleanup();

  t3d_destroy();

  display_close();
}
