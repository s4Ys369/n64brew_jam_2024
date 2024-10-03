#include <libdragon.h>
#include <time.h>
#include "core.h"
#include "minigame.h"

#include "code/examplegame/examplegame.h"

typedef long long ticks;

int main()
{
    #if DEBUG
    	debug_init_isviewer();
    	debug_init_usblog();
    #endif
    
    // Initialize most subsystems
    dfs_init(DFS_DEFAULT_LOCATION);
    joypad_init();
    timer_init();
    srand(time(NULL));
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
    rdpq_init();

    // Enable RDP debugging
    #if DEBUG_RDP
        rdpq_debug_start();
        rdpq_debug_log(true);
        rspq_profile_start();
    #endif

    // Set the initial minigame
    minigame_play(&global_minigamedef_examplegame);

    // Program Loop
    while (1)
    {
        ticks oldtime = 0;
        ticks accumulator = 0;
        const ticks dt = TICKS_FROM_MS(DELTATIME*1000);

        // Initialize the minigame
        minigame_get_game()->funcPointer_init();
        
        // Handle the engine loop
        while (!minigame_get_ended())
        {
            ticks curtime = timer_ticks();
            ticks frametime = curtime - oldtime;
            
            // In order to prevent problems if the game slows down significantly, we will clamp the maximum timestep the simulation can take
            if (frametime > TICKS_FROM_MS(0.25f*1000))
                frametime = TICKS_FROM_MS(0.25f*1000);
            oldtime = curtime;
            
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
    }
}