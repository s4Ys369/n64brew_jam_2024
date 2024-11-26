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
    ptx->count = 6;
    ptx->bufSize = sizeof(TPXParticle) * (ptx->count + 2);
    ptx->buf = malloc_uncached(ptx->bufSize);
    ptx->mat = malloc_uncached(sizeof(T3DMat4FP));
}

// Alpha gradient
void gradient_alpha(uint8_t *color, float t)
{
    t = fminf(1.0f, fmaxf(0.0f, t));
    t = 0.1f - t;
    t *= t;


    color[0] = (uint8_t)(255);
    color[1] = (uint8_t)(255);
    color[2] = (uint8_t)(255);
    color[3] = (uint8_t)(200 * (1.0f - (t - 0.5f) / 0.5f));
}

void ptx_randomPos(Particles *ptx)
{
    for (int i = 0; i < ptx->count; i++)
    {
        int p = i / 2;
        int8_t *ptxPos = (i % 2 == 0) ? ptx->buf[p].posA : ptx->buf[p].posB;

        // Assign random sizes
        ptx->buf[p].sizeA = 20 + (rand() % 10);
        ptx->buf[p].sizeB = 20 + (rand() % 10);

        // Random positions
        T3DVec3 pos = {{
            (i * 1 + rand()) % 128 - 64,
            (i * 3 + rand()) % 128 - 64,
            (i * 4 + rand()) % 128 - 64
        }};

        t3d_vec3_norm(&pos);
        float len = rand() % 40;
        pos.v[0] *= len;
        pos.v[1] *= len;
        pos.v[2] *= len;

        ptxPos[0] = (rand() % 256) - 128;
        ptxPos[1] = (rand() % 256) - 128;
        ptxPos[2] = (rand() % 256) - 128;


        gradient_alpha(ptx->buf[p].colorA, (ptxPos[0] + 127) * 0.0012f);
        gradient_alpha(ptx->buf[p].colorB, (ptxPos[0] + 127) * 0.0012f);
    }
}

void ptx_draw(Platform* platform, Particles *ptx)
{

    static int frameCounter = 0;
    const int updateInterval = 3;

    // Prepare the RDPQ
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_set_mode_standard();
    rdpq_mode_zbuf(true, false);
    rdpq_mode_zoverride(true, 0, 0);
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);


    if (platform->position.z <= 20.0f && platform->position.z >= -20.0f)
    {
        if (frameCounter % updateInterval == 0)
        {
            ptx_randomPos(ptx);
        }
        frameCounter++;

        tpx_state_from_t3d();

        tpx_matrix_push_pos(1);
        tpx_matrix_set(ptx->mat, true);
            tpx_state_set_scale(1,1);
            t3d_mat4fp_from_srt_euler(
                ptx->mat,
                (float[3]){1,1,1},
                (float[3]){0,0,0},
                (float[3]){platform->position.x,platform->position.y,0}
            );
            tpx_particle_draw(ptx->buf, ptx->count);
        tpx_matrix_pop(1);
    }
}

void ptx_cleanup(Particles *ptx)
{
    free_uncached(ptx->mat);
    free_uncached(ptx->buf);
    tpx_destroy();
}

#endif // PARTICLES_H