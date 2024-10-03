#ifndef GAMEJAM2024_CORE_H
#define GAMEJAM2024_CORE_H

    #define TICKRATE 30
    #define DELTATIME  (1.0f/(double)TICKRATE)

    #define PLAYERCOLOR_1 0xFF0000
    #define PLAYERCOLOR_2 0x00FF00
    #define PLAYERCOLOR_3 0x0000FF
    #define PLAYERCOLOR_4 0xFFFF00

    typedef enum {
        PLAYER_1,
        PLAYER_2,
        PLAYER_3,
        PLAYER_4,
    } PlyNum;

    typedef enum {
        DIFF_EASY,
        DIFF_MEDIUM,
        DIFF_HARD,
    } AiDiff;

    uint32_t      core_get_playercount();
    joypad_port_t core_get_playercontroller();
    AiDiff        core_get_aidifficulty();
    double        core_get_subtick();
    
    void          core_set_winner(PlyNum ply);

    // These functions will be disabled when shipping the final game and only exist for minigame testing reasons
    void  core_test_set_aidifficulty(AiDiff level);

    // These functions shouldn't be used unless you really know what you're doing
    void core_set_subtick(double subtick);

#endif