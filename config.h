#ifndef GAMEJAM2024_CONFIG_H
#define GAMEJAM2024_CONFIG_H

    #include "core.h"

    /* ==================================================================================================================
        Don't use these macros as getter functions, use stuff like core_get_aidifficulty() and core_get_playercount() 
    ================================================================================================================== */

    // Skip asking the number of players and assume PLAYER_COUNT
    #define SKIP_PLAYERSELECTION  1

    // The number of human players
    #define PLAYER_COUNT  4

    // Skip asking the AI difficulty and assume AI_DIFFICULTY
    #define SKIP_DIFFICULTYSELECTION  1

    // The difficulty of the AI. 
    #define AI_DIFFICULTY  DIFF_MEDIUM

    // Skip the minigame selection and boot straight into MINIGAME_TO_TEST
    #define SKIP_MINIGAMESELECTION  1

    // The current minigame you want to test
    #define MINIGAME_TO_TEST  "game"

#endif
