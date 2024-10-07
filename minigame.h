#ifndef GAMEJAM2024_MINIGAME_H
#define GAMEJAM2024_MINIGAME_H

    /***************************************************************
                       Public Minigame Functions
    ***************************************************************/

    // You need to have one of these structs defined globally for the minigame manager to detect it
    typedef struct {
        char* gamename;
        char* developername;
        char* description;
        char* instructions;
    } MinigameDef;


    /*==============================
        minigame_end
        Use this to mark your minigame as finished
    ==============================*/
    void minigame_end();

    
    /***************************************************************
                      Internal Minigame Functions
                  Do not use anything below this line
    ***************************************************************/

    #include <stdbool.h>

    typedef struct {
        char* internalname;
        MinigameDef definition;
        void* handle;
        void (*funcPointer_init)(void);
        void (*funcPointer_loop)(float deltatime);
        void (*funcPointer_fixedloop)(float deltatime);
        void (*funcPointer_cleanup)(void);
    } Minigame;

    extern Minigame* global_minigame_list;
    extern size_t    global_minigame_count;

    void      minigame_loadall();
    void      minigame_play(char* name);
    void      minigame_cleanup();
    Minigame* minigame_get_game();
    bool      minigame_get_ended();

#endif 