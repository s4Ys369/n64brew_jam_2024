/*
* This file includes code from Tiny3D.
* Tiny3D is licensed under the MIT License.
*
* Original code by Max Bebök 
* Adapted by s4ys
* November 2024
*
* Description of changes or adaptations made:
* - Generate positions randomly within an AABB
* - Utilize `gradient_fire` in more of a random distribution
*
*
* Original source: https://github.com/HailToDodongo/tiny3d/tree/main/examples/18_particles
*/

#ifndef PARTICLES_H
#define PARTICLES_H

typedef struct
{
    uint32_t count;
    uint32_t bufSize;
    TPXParticle* buf;
    T3DMat4FP* mat;

} Particles;

Particles lavaBubbles;

void ptx_init(Particles *ptx)
{
    tpx_init((TPXInitParams){});
    ptx->count = 2000;
    ptx->bufSize = sizeof(TPXParticle) * (ptx->count + 2);
    ptx->buf = malloc_uncached(ptx->bufSize);
    ptx->mat = malloc_uncached(sizeof(T3DMat4FP));
}

void ptx_randomPos(Particles *ptx, AABB aabb, T3DViewport* vp)
{
    for (int i = 0; i < ptx->count; i++)
    {
        int p = i / 2;
        int8_t *ptxPos = (i % 2 == 0) ? ptx->buf[p].posA : ptx->buf[p].posB;

        // Assign random sizes
        ptx->buf[p].sizeA = 2 + (rand() % 5);
        ptx->buf[p].sizeB = 2 + (rand() % 5);

        // Random positions within the bounding box
        T3DVec3 randomPos;
        randomPos.v[0] = aabb.minCoordinates.x + ((float)rand() / 1000) * (aabb.maxCoordinates.x  - aabb.minCoordinates.x);
        randomPos.v[1] = aabb.minCoordinates.y + ((float)rand() / 1000) * (aabb.maxCoordinates.y  - aabb.minCoordinates.y);
        randomPos.v[2] = aabb.minCoordinates.z + ((float)rand() / 1000) * (aabb.maxCoordinates.z  - aabb.minCoordinates.z);

        // Calculate from view space
        T3DVec3 screenPos;
        t3d_viewport_calc_viewspace_pos(vp, &screenPos, &randomPos);

        ptxPos[0] = (int8_t)screenPos.v[0]; // Cast to match int8_t type
        ptxPos[1] = (int8_t)screenPos.v[1]; 
        ptxPos[2] = (int8_t)screenPos.v[2];
    }
}

// Fire color: white -> yellow/orange -> red -> black
void gradient_fire(uint8_t *color, float t)
{
    t = fminf(1.0f, fmaxf(0.0f, t));
    t = 0.8f - t;
    t *= t;

    if (t < 0.25f) { // Dark red to bright red
      color[0] = (uint8_t)(200 * (t / 0.25f)) + 55;
      color[1] = 0;
      color[2] = 0;
    } else if (t < 0.5f) { // Bright red to yellow
      color[0] = 255;
      color[1] = (uint8_t)(255 * ((t - 0.25f) / 0.25f));
      color[2] = 0;
    } else if (t < 0.75f) { // Yellow to white (optional, if you want a bright white center)
      color[0] = 255;
      color[1] = 255;
      color[2] = (uint8_t)(255 * ((t - 0.5f) / 0.25f));
    } else { // White to black
      color[0] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
      color[1] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
      color[2] = (uint8_t)(255 * (1.0f - (t - 0.75f) / 0.25f));
    }
}

void ptx_draw(T3DViewport* vp, Particles *ptx, float x, float y)
{

    static int frameCounter = 0;
    const int updateInterval = 3;

    // Prepare the RDPQ
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER1((PRIM,0,ENV,0), (0,0,0,1)));
    rdpq_set_env_color(ui_color(WHITE));

     
    AABB aabb = (AABB) {
        .minCoordinates = {-1000,-1000,-1000},
        .maxCoordinates = {1000,1000,1000}
    };

    if (frameCounter % updateInterval == 0)
    {
        for (int p = 0; p < ptx->count; p++)
        {
            gradient_fire(ptx->buf[p].colorA, (ptx->buf[p].posA[1] + 127) / 150.0f);
            gradient_fire(ptx->buf[p].colorB, (ptx->buf[p].posB[1] + 127) / 150.0f);
        }

        ptx_randomPos(ptx, aabb, vp);
    }
    frameCounter++;

    t3d_mat4fp_from_srt_euler(
        ptx->mat,
        (float[3]){50,50,50},
        (float[3]){0,0,0},
        (float[3]){0,0,-250}
    );

    tpx_state_from_t3d();

    tpx_matrix_push(ptx->mat);
        tpx_state_set_scale(x,y);
        tpx_particle_draw(ptx->buf, ptx->count);
    tpx_matrix_pop(1);
}

void ptx_cleanup(Particles *ptx)
{
    free_uncached(ptx->mat);
    free_uncached(ptx->buf);
    tpx_destroy();
}

#endif // PARTICLES_H