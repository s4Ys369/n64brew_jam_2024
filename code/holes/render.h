#ifndef RENDER_H
#define RENDER_H

void render_scene(game_data *game, scene_data *scene);

void render_scene(game_data *game, scene_data *scene)
{
    // Attach display and color clear
    rdpq_attach(display_get(), depthBuffer);
    rdpq_clear(RGBA32(0, 0, 0, 255));

    // For each human player
    for (size_t i = 0; i < game->playerCount; i++)
    {

        // If not alive, do nothing
        if (!players[i].isAlive)
            continue;

        // Reset render mode and attach player's viewport
        t3d_frame_start();
        t3d_viewport_attach(&viewport[i]);

        // Set lights
        t3d_light_set_ambient(scene->colorAmbient);
        t3d_light_set_directional(0, scene->colorDir, &scene->lightDirVec);
        t3d_light_set_count(1);

        /** Render Layout
         * - Map
         * - Players
         * - Objects
         * - UI
         */

        t3d_matrix_push_pos(1);

        // Disable Z buffer compare for first layer
        rdpq_mode_zbuf(false, true);
        rspq_block_run(map.block);

        // reset Z buffer testing for second layer
        rdpq_mode_zbuf(false, true);
        for (size_t p = 0; p < MAXPLAYERS; p++)
        {
            player_draw(&players[p]);
        }

        // Disable Z buffer write for last layer
        rdpq_mode_zbuf(true, false);
        for (int j = 0; j < NUM_OBJ_TYPES; j++)
        {
            object_cull(&objects[j], &viewport[i], game->playerCount);
        }

        object_drawBatch(&objects[OBJ_HYDRANT]);
        for (size_t o = 0; o < NUM_OBJECTS; o++)
        {
            if (spray[o] && !objects[OBJ_HYDRANT].objects[o].hide)
                hydrant_water_spray(objects[OBJ_HYDRANT].objects[o].position, &viewport[i]);
        }

        // Reset render mode for 3D
        t3d_frame_start();

        object_drawBatch(&objects[OBJ_BUILDING]);
        object_drawBatch(&objects[OBJ_CAR]);

        t3d_matrix_pop(1);
    }

    game->syncPoint = rspq_syncpoint_new();

    // ======== Draw (2D) ======== //
    rdpq_sync_tile();
    rdpq_sync_pipe(); // Hardware crashes otherwise

    viewport_drawScissor();

    ui_print(game, true);

    rdpq_detach_show();
}

#endif // RENDER_H