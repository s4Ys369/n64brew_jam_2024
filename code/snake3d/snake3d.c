#include <libdragon.h>
#include "../../minigame.h"
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

const MinigameDef minigame_def = {
    .gamename = "Snake3D",
    .developername = "HailToDodongo",
    .description = "This is a porting of one of the Tiny3D examples, to show how to "
                   "integrate Tiny3D in minigame",
    .instructions = "Press A to win."
};


/**
 * Example project showcasing the usage of the animation system.
 * This includes instancing animations, blending animations, and controlling playback.
 */

surface_t depthBuffer;
T3DViewport viewport;
rdpq_font_t *font;
T3DMat4FP* modelMatFP;
T3DMat4FP* mapMatFP;
rspq_block_t *dplSnake;
rspq_block_t *dplMap;
T3DAnim animAttack;
T3DAnim animWalk;
T3DAnim animIdle;
T3DSkeleton skelBlend;
T3DSkeleton skel;
T3DModel *model;
T3DModel *modelShadow;
T3DModel *modelMap;
T3DVec3 camPos;
T3DVec3 camTarget;
T3DVec3 lightDirVec;
T3DVec3 moveDir;
T3DVec3 playerPos;

void minigame_init(void)
{
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  depthBuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

  t3d_init((T3DInitParams){});
  font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, font);
  viewport = t3d_viewport_create();

  modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
  mapMatFP = malloc_uncached(sizeof(T3DMat4FP));
  t3d_mat4fp_from_srt_euler(mapMatFP, (float[3]){0.3f, 0.3f, 0.3f}, (float[3]){0, 0, 0}, (float[3]){0, 0, -10});

  camPos = (T3DVec3){{0, 45.0f, 80.0f}};
  camTarget = (T3DVec3){{0, 0,-10}};

  moveDir = (T3DVec3){{0,0,0}};
  playerPos = (T3DVec3){{0,0.15f,0}};

  lightDirVec = (T3DVec3){{1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);

  modelMap = t3d_model_load("rom:/snake3d/map.t3dm");
  modelShadow = t3d_model_load("rom:/snake3d/shadow.t3dm");

  // Model Credits: Quaternius (CC0) https://quaternius.com/packs/easyenemy.html
  model = t3d_model_load("rom:/snake3d/snake.t3dm");

  // First instantiate skeletons, they will be used to draw models in a specific pose
  // And serve as the target for animations to modify
  skel = t3d_skeleton_create(model);
  skelBlend = t3d_skeleton_clone(&skel, false); // optimized for blending, has no matrices

  // Now create animation instances (by name), the data in 'model' is fixed,
  // whereas 'anim' contains all the runtime data.
  // Note that tiny3d internally keeps no track of animations, it's up to the user to manage and play them.
  animIdle = t3d_anim_create(model, "Snake_Idle");
  t3d_anim_attach(&animIdle, &skel); // tells the animation which skeleton to modify

  animWalk = t3d_anim_create(model, "Snake_Walk");
  t3d_anim_attach(&animWalk, &skelBlend);

  // multiple animations can attach to the same skeleton, this will NOT perform any blending
  // rather the last animation that updates "wins", this can be useful if multiple animations touch different bones
  animAttack = t3d_anim_create(model, "Snake_Attack");
  t3d_anim_set_looping(&animAttack, false); // don't loop this animation
  t3d_anim_set_playing(&animAttack, false); // start in a paused state
  t3d_anim_attach(&animAttack, &skel);

  rspq_block_begin();
    t3d_matrix_push(modelMatFP);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw_skinned(model, &skel); // as in the last example, draw skinned with the main skeleton

    rdpq_set_prim_color(RGBA32(0, 0, 0, 120));
    t3d_model_draw(modelShadow);
    t3d_matrix_pop(1);
  dplSnake = rspq_block_end();

  rspq_block_begin();
    t3d_matrix_push(mapMatFP);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw(modelMap);
    t3d_matrix_pop(1);
  dplMap = rspq_block_end();
}

void minigame_loop(float deltaTime)
{
    rspq_syncpoint_t syncPoint = 0;

    uint8_t colorAmbient[4] = {0xAA, 0xAA, 0xAA, 0xFF};
    uint8_t colorDir[4]     = {0xFF, 0xAA, 0xAA, 0xFF};

    float rotY = 0.0f;
    float currSpeed = 0.0f;
    float animBlend = 0.0f;
    bool isAttack = false;

    joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    T3DVec3 newDir = {{
       (float)joypad.stick_x * 0.05f, 0,
      -(float)joypad.stick_y * 0.05f
    }};
    float speed = sqrtf(t3d_vec3_len2(&newDir));

    if(btn.start) minigame_end();

    // Player Attack
    if((btn.a || btn.b) && !animAttack.isPlaying) {
      t3d_anim_set_playing(&animAttack, true);
      t3d_anim_set_time(&animAttack, 0.0f);
      isAttack = true;
    }

    // Player movement
    if(speed > 0.15f && !isAttack) {
      newDir.v[0] /= speed;
      newDir.v[2] /= speed;
      moveDir = newDir;

      float newAngle = atan2f(moveDir.v[0], moveDir.v[2]);
      rotY = t3d_lerp_angle(rotY, newAngle, 0.25f);
      currSpeed = t3d_lerp(currSpeed, speed * 0.15f, 0.15f);
    } else {
      currSpeed *= 0.8f;
    }

    // use blend based on speed for smooth transitions
    animBlend = currSpeed / 0.51f;
    if(animBlend > 1.0f)animBlend = 1.0f;

    // move player...
    playerPos.v[0] += moveDir.v[0] * currSpeed;
    playerPos.v[2] += moveDir.v[2] * currSpeed;
    // ...and limit position inside the box
    const float BOX_SIZE = 140.0f;
    if(playerPos.v[0] < -BOX_SIZE)playerPos.v[0] = -BOX_SIZE;
    if(playerPos.v[0] >  BOX_SIZE)playerPos.v[0] =  BOX_SIZE;
    if(playerPos.v[2] < -BOX_SIZE)playerPos.v[2] = -BOX_SIZE;
    if(playerPos.v[2] >  BOX_SIZE)playerPos.v[2] =  BOX_SIZE;

    // position the camera behind the player
    camTarget = playerPos;
    camTarget.v[2] -= 20;
    camPos.v[0] = camTarget.v[0];
    camPos.v[1] = camTarget.v[1] + 45;
    camPos.v[2] = camTarget.v[2] + 65;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 150.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // Update the animation and modify the skeleton, this will however NOT recalculate the matrices
    t3d_anim_update(&animIdle, deltaTime);
    t3d_anim_set_speed(&animWalk, animBlend + 0.15f);
    t3d_anim_update(&animWalk, deltaTime);

    if(isAttack) {
      t3d_anim_update(&animAttack, deltaTime); // attack animation now overrides the idle one
      if(!animAttack.isPlaying)isAttack = false;
    }

    // We now blend the walk animation with the idle/attack one
    t3d_skeleton_blend(&skel, &skel, &skelBlend, animBlend);

    if(syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame

    // Now recalc. the matrices, this will cause any model referencing them to use the new pose
    t3d_skeleton_update(&skel);

    // Update player matrix
    t3d_mat4fp_from_srt_euler(modelMatFP,
      (float[3]){0.125f, 0.125f, 0.125f},
      (float[3]){0.0f, -rotY, 0},
      playerPos.v
    );

    // ======== Draw (3D) ======== //
    rdpq_attach(display_get(), &depthBuffer);
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(224, 180, 96, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);

    rspq_block_run(dplMap);
    rspq_block_run(dplSnake);

    syncPoint = rspq_syncpoint_new();

    // ======== Draw (UI) ======== //
    float posX = 16;
    float posY = 24;

    rdpq_sync_pipe();
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "[A] Attack: %d", isAttack);

    posY = 216;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Speed: %.4f", currSpeed); posY += 10;
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, posX, posY, "Blend: %.4f", animBlend); posY += 10;

    rdpq_detach_show();
}

void minigame_cleanup(void)
{
  rspq_block_free(dplSnake);
  rspq_block_free(dplMap);

  t3d_skeleton_destroy(&skel);
  t3d_skeleton_destroy(&skelBlend);

  t3d_anim_destroy(&animIdle);
  t3d_anim_destroy(&animWalk);
  t3d_anim_destroy(&animAttack);

  t3d_model_free(model);
  t3d_model_free(modelMap);
  t3d_model_free(modelShadow);

  free_uncached(mapMatFP);
  free_uncached(modelMatFP);

  rdpq_text_unregister_font(FONT_BUILTIN_DEBUG_MONO);
  rdpq_font_free(font);
  t3d_destroy();

  surface_free(&depthBuffer);
  display_close();
}
