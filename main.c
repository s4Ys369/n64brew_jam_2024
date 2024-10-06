#include <libdragon.h>
#include <time.h>
#include <unistd.h>
#include "core.h"
#include "minigame.h"

typedef long long ticks;

int main()
{
    #if DEBUG
    	debug_init_isviewer();
    	debug_init_usblog();
    #endif
    
    // Initialize most subsystems
    dfs_init(DFS_DEFAULT_LOCATION);
    debug_init_usblog();
    debug_init_isviewer();
    joypad_init();
    timer_init();
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
    rdpq_init();
    minigame_loadall();

    // Enable RDP debugging
    #if DEBUG_RDP
        rdpq_debug_start();
        rdpq_debug_log(true);
        rspq_profile_start();
    #endif

    // Initialize the random number generator
    uint32_t seed;
    getentropy(&seed, sizeof(seed));
    srand(seed);

    // Set the initial minigame
    minigame_play("examplegame");

    // Program Loop
    while (1)
    {
        float accumulator = 0;
        const float dt = DELTATIME;

        // Initialize the minigame
        minigame_get_game()->funcPointer_init();
        
        // Handle the engine loop
        while (!minigame_get_ended())
        {
            float frametime = display_get_delta_time();
            
            // In order to prevent problems if the game slows down significantly, we will clamp the maximum timestep the simulation can take
            if (frametime > 0.25f)
                frametime = 0.25f;
            
            // Perform the update in discrete steps (ticks)
            accumulator += frametime;
            while (accumulator >= dt)
            {
                minigame_get_game()->funcPointer_fixedloop(dt);
                accumulator -= dt;
            }
            
            // Read controler data
            joypad_poll();
            
            // Perform the unfixed loop
            core_set_subtick(((double)accumulator)/((double)dt));
            minigame_get_game()->funcPointer_loop(frametime);
        }
        
        // End the current level
        minigame_get_game()->funcPointer_cleanup();
        minigame_cleanup();
    }
}