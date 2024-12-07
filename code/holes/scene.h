#ifndef SCENE_H
#define SCENE_H

enum SCENE_ID
{
    INTRO,
    GAMEPLAY,
    PAUSE,
    ENDING,
    NUM_SCENES
};

extern scene_data scenes[NUM_SCENES];

void intro_fixedLoop(game_data *game, float deltaTime);
void intro_loop(game_data *game, float deltaTime);
void gameplay_fixedLoop(game_data *game, float deltaTime);
void gameplay_loop(game_data *game, float deltaTime);
void pause_fixedLoop(game_data *game, float deltaTime);
void pause_loop(game_data *game, float deltaTime);
void ending_fixedLoop(game_data *game, float deltaTime);
void ending_loop(game_data *game, float deltaTime);

void intro_fixedLoop(game_data *game, float deltaTime) {}
void intro_loop(game_data *game, float deltaTime) {}

void gameplay_fixedLoop(game_data *game, float deltaTime)
{

    ////////// PLAYERS
    bool controlbefore = player_has_control(game, &players[0]);
    for (size_t i = 0; i < MAXPLAYERS; i++)
    {
        for (int j = 0; j < NUM_OBJ_TYPES; j++)
        {
            player_fixedloop(game, &players[i], &objects[j], deltaTime, core_get_playercontroller(i), i < game->playerCount);
        }
    }
    //////////

    sound_setChannels();
    if (game->countDownTimer > -GO_DELAY)
    {
        float prevCountDown = game->countDownTimer;
        game->countDownTimer -= deltaTime;
        if ((int)prevCountDown != (int)game->countDownTimer && game->countDownTimer >= 0)
            sound_wavPlay(SFX_COUNTDOWN, false);
    }
    if (!controlbefore && player_has_control(game, &players[0]))
        sound_wavPlay(SFX_START, false);

    if (!game->isEnding)
    {
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

        if (alivePlayers == 1)
        {
            game->isEnding = true;
            game->winner = lastPlayer;
            if (game->playerCount != 4)
                sound_xmStop();
            sound_wavPlay(SFX_STOP, false);
        }
    }
    else
    {
        float prevEndTime = game->endTimer;
        game->endTimer += deltaTime;
        if ((int)prevEndTime != (int)game->endTimer && (int)game->endTimer == WIN_SHOW_DELAY)
            sound_wavPlay(SFX_WINNER, false);
        if (game->endTimer > WIN_DELAY)
        {
            core_set_winner(game->winner);
            minigame_end();
        }
    }
}

void gameplay_loop(game_data *game, float deltaTime)
{

    viewport_set(viewport, game->playerCount, cam);

    for (size_t i = 0; i < game->playerCount; i++)
    {

        for (int j = 0; j < NUM_OBJ_TYPES; j++)
        {
            for (size_t p = 0; p < MAXPLAYERS; p++)
            {
                object_updateBatch(&objects[j], &viewport[i], &players[p]);
            }
        }

        cam[i].position = (T3DVec3){{players[i].playerPos.x, players[i].playerPos.y + 250.0f, players[i].playerPos.z + 100.0f}};
        cam[i].target = players[i].playerPos;
    }

    for (size_t p = 0; p < MAXPLAYERS; p++)
    {
        player_loop(game, &players[p], deltaTime, core_get_playercontroller(p), p < game->playerCount);
    }

    render_scene(game, &scenes[GAMEPLAY]);
}

void scene_init(scene_data *scene)
{
    uint8_t tempAmb[4] = {54, 40, 47, 0xFF};
    uint8_t tempDir[4] = {0xFF, 0xAA, 0xAA, 0xFF};
    for (size_t s = 0; s < NUM_SCENES; s++)
    {
        scene[s].ID = s;

        switch (s)
        {
        case INTRO:
            for (int i = 0; i < 4; i++)
            {
                scene[s].colorAmbient[i] = tempAmb[i];
                scene[s].colorDir[i] = tempDir[i];
            }
            scene[s].lightDirVec = (T3DVec3){{0.0f, 1.0f, 1.0f}};
            scene[s].loop = intro_loop;
            scene[s].fixedLoop = intro_fixedLoop;
            break;
        case GAMEPLAY:
            for (int i = 0; i < 4; i++)
            {
                scene[s].colorAmbient[i] = tempAmb[i];
                scene[s].colorDir[i] = tempDir[i];
            }
            scene[s].lightDirVec = (T3DVec3){{0.0f, 1.0f, 1.0f}};
            scene[s].loop = gameplay_loop;
            scene[s].fixedLoop = gameplay_fixedLoop;
            break;
        case PAUSE:
            for (int i = 0; i < 4; i++)
            {
                scene[s].colorAmbient[i] = tempAmb[i];
                scene[s].colorDir[i] = tempDir[i];
            }
            scene[s].lightDirVec = (T3DVec3){{0.0f, 1.0f, 1.0f}};
            scene[s].loop = pause_loop;
            scene[s].fixedLoop = pause_fixedLoop;
            break;
        case ENDING:
            for (int i = 0; i < 4; i++)
            {
                scene[s].colorAmbient[i] = tempAmb[i];
                scene[s].colorDir[i] = tempDir[i];
            }
            scene[s].lightDirVec = (T3DVec3){{0.0f, 1.0f, 1.0f}};
            scene[s].loop = ending_loop;
            scene[s].fixedLoop = ending_fixedLoop;
            break;
        }

        t3d_vec3_norm(&scene[s].lightDirVec);
    }
}

void pause_fixedLoop(game_data *game, float deltaTime) {}
void pause_loop(game_data *game, float deltaTime) {}
void ending_fixedLoop(game_data *game, float deltaTime) {}
void ending_loop(game_data *game, float deltaTime) {}

#endif // SCENE_H