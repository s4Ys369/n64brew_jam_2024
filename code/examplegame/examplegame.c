#include <libdragon.h>
#include "../../core.h"
#include "../../minigame.h"

const MinigameDef minigame_def = {
    .gamename = "Example Game",
    .developername = "Your Name",
    .description = "This is an example game.",
    .instructions = "Press A to win."
};

void minigame_init()
{
    debugf("Hello world, from examplegame initialization!\n");
}

void minigame_fixedloop(float deltatime)
{
    // TODO
    if (false)
        minigame_end();
}

void minigame_loop(float deltatime)
{
    // TODO
}

void minigame_cleanup()
{
    // TODO
}