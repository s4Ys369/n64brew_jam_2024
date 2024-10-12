/***************************************************************
                         examplegame.c
                               
An example minigame to demonstrate how to use the template for
the game jam.
***************************************************************/

#include <libdragon.h>
#include "../../core.h"
#include "../../minigame.h"

#define FONT_TEXT       1


/*********************************
             Globals
*********************************/

// You need this function defined somewhere in your project
// so that the minigame manager can work
const MinigameDef minigame_def = {
    .gamename = "Example Game",
    .developername = "Your Name",
    .description = "This is an example game.",
    .instructions = "Press A to win."
};

rdpq_font_t *font;

/*==============================
    minigame_init
    The minigame initialization function
==============================*/

void minigame_init()
{
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_VAR);
    rdpq_text_register_font(FONT_TEXT, font);
}


/*==============================
    minigame_fixedloop
    Code that is called every loop, at a fixed delta time.
    Use this function for stuff where a fixed delta time is 
    important, like physics.
    @param  The fixed delta time for this tick
==============================*/

void minigame_fixedloop(float deltatime)
{
    // Check for A presses from players
    bool a_pressed = false;
    for (uint32_t i=0; i<core_get_playercount(); i++)
    {
        joypad_buttons_t btn = joypad_get_buttons_pressed(core_get_playercontroller(i));
        if (btn.a) {
            a_pressed = true;
            core_set_winner(i);
        }
    }

    // Make the AI press a button at random
    for (uint32_t i=core_get_playercount(); i<MAXPLAYERS; i++)
    {
        int aipress = rand() % (300/(1+core_get_aidifficulty())); // Speed dependent on AI difficulty
        if (aipress == 1) {
            a_pressed = true;
            core_set_winner(i);
        }
    }

    // Handle Winner
    if (a_pressed)
        minigame_end();
}


/*==============================
    minigame_loop
    Code that is called every loop.
    @param  The delta time for this tick
==============================*/

void minigame_loop(float deltatime)
{
    // Render the instructions
    rdpq_attach(display_get(), NULL);
    rdpq_text_printf(NULL, FONT_TEXT, 30, 100, "Press A to win.");
    rdpq_detach_show();
}


/*==============================
    minigame_cleanup
    Clean up any memory used by your game just before it ends.
==============================*/

void minigame_cleanup()
{
    display_close();
    rdpq_text_unregister_font(FONT_TEXT);
    rdpq_font_free(font);
}