#include <libdragon.h>
#include "../../core.h"
#include "../../minigame.h"

Minigame global_minigamedef_examplegame = {
    "My Example Game",
    "My Team Name",
    "This is a brief, catchy description of the minigame.",
    "This is an explanation of how to play this minigame",
    "images/examplegame/icon",
    &examplegame_init,      // TODO: This code needs to go in a function because you can't initialize function pointers like this in a struct, especially since these functions will get DSO'd in
    &examplegame_fixedloop,
    &examplegame_loop,
    &examplegame_cleanup,
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