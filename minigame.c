#include <libdragon.h>
#include "core.h"
#include "minigame.h"

Minigame* global_minigame_current = NULL;
MinigameDef* global_minigame_list;


void minigame_loadall()
{
    // TODO: Search through the code directory and list the number of games, allocate a MinigameDef struct for each all
}

void minigame_play(char* name)
{
    //global_minigame_current = mini;
    // TODO: Load overlays and generate the Minigame struct
}

void minigame_end()
{
    // TODO
}

Minigame* minigame_get_game()
{
    return global_minigame_current;
}

bool minigame_get_ended()
{
    // TODO
    return false;
}