/***************************************************************
                             core.c
                               
The file contains Core minigame code, which handles stuff like
player, AI, and game loop information.
***************************************************************/

#include <libdragon.h>
#include "core.h"
#include "config.h"


/*********************************
            Structures
*********************************/

typedef struct {
    PlyNum number;
    joypad_port_t port;
} Player;


/*********************************
             Globals
*********************************/

// Player info
static Player   global_core_players[JOYPAD_PORT_COUNT];
static uint32_t global_core_playercount;
static AiDiff   global_core_aidifficulty = AI_DIFFICULTY;

// Minigame info
static bool global_core_playeriswinner[MAXPLAYERS];

// Core info
static double global_core_subtick = 0;


/*==============================
    core_get_subtick
    Gets the current subtick. Use this to help smooth
    movements in your draw loop
    @return The current subtick
==============================*/

void core_set_subtick(double subtick)
{
    global_core_subtick = subtick;
}


/*==============================
    core_set_playercount
    Sets the number of human players
    @param  The number of players
==============================*/

void core_set_playercount(uint32_t playercount)
{
    int lastcont = 0;

    // Attempt to find the first N left-most available controllers
    for (int i=0; i<playercount; i++)
    {
        bool found = false;
        for (int j=lastcont; j<JOYPAD_PORT_COUNT; j++)
        {
            if (joypad_is_connected(j))
            {
                global_core_players[i].port = j;
                found = true;
                lastcont = ++j;
                break;
            }
        }
        assertf(found, "Unable to find an available controller for player %d\n", i+1);
    }
    global_core_playercount = playercount;
}

/*==============================
    core_set_aidifficulty
    Sets the AI difficulty
    @param  The AI difficulty
==============================*/

void core_set_aidifficulty(AiDiff difficulty)
{
    global_core_aidifficulty = difficulty;
}


/*==============================
    core_set_winner
    Set the winner of the minigame. You can call this
    multiple times to set multiple winners.
    @param  The winning player
==============================*/

void core_set_winner(PlyNum ply)
{
    global_core_playeriswinner[ply] = true;
}


/*==============================
    core_get_aidifficulty
    Gets the current AI difficulty
    @return The AI difficulty
==============================*/

AiDiff core_get_aidifficulty()
{
    return global_core_aidifficulty;
}


/*==============================
    core_get_subtick
    Gets the subtick of the current frame
    @return The frame's subtick
==============================*/

double core_get_subtick()
{
    return global_core_subtick;
}


/*==============================
    core_get_playercount
    Get the number of human players
    @return The number of players
==============================*/

uint32_t core_get_playercount()
{
    return global_core_playercount;
}


/*==============================
    core_get_playercontroller
    Get the controller port of this player.
    Because player 1's controller might not be plugged 
    into port number 1.
    @param  The player we want
    @return The controller port
==============================*/

joypad_port_t core_get_playercontroller(PlyNum ply)
{
    return global_core_players[ply].port;
}


/*==============================
    core_reset_winners
    Resets the winners
==============================*/

void core_reset_winners()
{
    for (int i=0; i<MAXPLAYERS; i++)
        global_core_playeriswinner[i] = false;
}