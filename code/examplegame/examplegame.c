#include <libdragon.h>
#include "../../core.h"
#include "../../minigame.h"

const MinigameDef minigame_def = {
    .gamename = "Example Game",
    .developername = "Your Name",
    .description = "This is an example game.",
    .instructions = "Press A to win."
};

void examplegame_init()
{
    // TODO
}

void examplegame_fixedloop(float deltatime)
{
    // TODO
    if (false)
        minigame_end();
}

void examplegame_loop(float deltatime)
{
    // TODO
}

void examplegame_cleanup()
{
    // TODO
}