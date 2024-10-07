#include <libdragon.h>
#include "core.h"
#include "config.h"

typedef struct {
    PlyNum number;
    joypad_port_t port;
} Player;

static Player   global_core_players[JOYPAD_PORT_COUNT];
static uint32_t global_core_playercount;

static bool global_core_playeriswinner[MAXPLAYERS];

static AiDiff global_core_aidifficulty = AI_DIFFICULTY;
static double global_core_subtick = 0;

void core_set_subtick(double subtick)
{
    global_core_subtick = subtick;
}

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
                lastcont = j++;
                break;
            }
        }
        assertf(found, "Unable to find an available controller for player %d\n", i+1);
    }
    global_core_playercount = playercount;
}

void core_set_winner(PlyNum ply)
{
    global_core_playeriswinner[ply] = true;
}

AiDiff core_get_aidifficulty()
{
    return global_core_aidifficulty;
}

double core_get_subtick()
{
    return global_core_subtick;
}

uint32_t core_get_playercount()
{
    return global_core_playercount;
}

joypad_port_t core_get_playercontroller(PlyNum ply)
{
    return global_core_players[ply].port;
}