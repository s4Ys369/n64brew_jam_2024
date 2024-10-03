#ifndef GAMEJAM2024_MINIGAME_H
#define GAMEJAM2024_MINIGAME_H

    typedef struct {
        char* gamename;
        char* developername;
        char* description;
        char* instructions;
        char* iconpath
        void (*funcPointer_init)(void);
        void (*funcPointer_loop)(float deltatime);
        void (*funcPointer_fixedloop)(float deltatime);
        void (*funcPointer_cleanup)(void);
    } Minigame;

    void      minigame_play(Minigame* mini);
    void      minigame_end();

    Minigame* minigame_get_game();
    bool      minigame_get_ended();

#endif 