#include <libdragon.h>
#include "core.h"
#include "minigame.h"

Minigame* global_minigame_current = NULL;

void minigame_play(Minigame* mini)
{
    global_minigame_current = mini;
    // TODO: Load overlays
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