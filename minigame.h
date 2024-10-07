#ifndef GAMEJAM2024_MINIGAME_H
#define GAMEJAM2024_MINIGAME_H

    #include <stdbool.h>

    typedef struct {
        char* gamename;
        char* developername;
        char* description;
        char* instructions;
    } MinigameDef;

    void minigame_end();


    // Internal stuff. Don't use this

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
    extern size_t global_minigame_count;

    void      minigame_loadall();
    void      minigame_play(char* name);
    void      minigame_cleanup();
    Minigame* minigame_get_game();
    bool      minigame_get_ended();

#endif 