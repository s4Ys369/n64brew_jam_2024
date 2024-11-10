#ifndef ROOM_H
#define ROOM_H

// Hook/callback to modify tile settings set by t3d_model_draw
void tile_scroll(void* userData, rdpq_texparms_t *tileParams, rdpq_tile_t tile) 
{
  float offset = *(float*)userData;
  if(tile == TILE0) {
    tileParams->s.translate = offset * 0.5f;
    tileParams->t.translate = offset * 0.8f;

    tileParams->s.translate = fm_fmodf(tileParams->s.translate, 32.0f);
    tileParams->t.translate = fm_fmodf(tileParams->t.translate, 32.0f);
  }
}


T3DMat4 modelMat;
t3d_mat4_identity(&modelMat);
t3d_mat4_scale(&modelMat, 0.12f, 0.12f, 0.12f);

T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));
t3d_mat4_to_fixed(modelMatFP, &modelMat);


uint8_t colorAmbient[4] = {0xF0, 0xF0, 0xF0, 0xFF};

T3DModel *modelRoom = t3d_model_load("rom:/model.t3dm");
T3DModel *modelLava = t3d_model_load("rom:/lava.t3dm");

rspq_block_begin();
t3d_model_draw(modelRoom);
rspq_block_t *dplRoom = rspq_block_end();

rspq_block_begin();
t3d_model_draw(modelLava);
rspq_block_t *dplLava = rspq_block_end();

bool scrollEnabled = true;
bool transformEnabled = true;

float tileOffset = 0.0f;
float transformOffset = 0.0f;



#endif