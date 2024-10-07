#ifndef GAMEJAM2024_CONFIG_H
#define GAMEJAM2024_CONFIG_H

    #include "core.h"

    // The number of human players
    #define PLAYER_COUNT  1

    // Skip the menu and boot straight into MINIGAME_TO_TEST
    #define SKIP_MENU  0

    // The current minigame you want to test
    #define MINIGAME_TO_TEST  "examplegame"

    // The difficulty of the AI. 
    // Don't use this macro as a getter function, use core_get_aidifficulty()
    #define AI_DIFFICULTY  DIFF_MEDIUM

#endif