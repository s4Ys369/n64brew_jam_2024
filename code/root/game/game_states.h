#ifndef GAME_STATES_H
#define GAME_STATES_H

#define INTRO 0
#define MAIN_MENU 1
#define GAMEPLAY 2
#define PAUSE 3
#define GAME_OVER 4

extern rspq_syncpoint_t syncPoint;

// function prototypes

void gameState_setIntro();
void gameState_setMainMenu();
void gameState_setGameplay();
void gameState_setPause();
void gameState_setGameOver();


// function implementations

void gameState_setIntro()
{
    // code for the intro state
}

void gameState_setMainMenu()
{

}

void gameState_setGameplay()
{

}

void gameState_setPause()
{
    // code for the pause state
}

void gameState_setGameOver()
{
    // code for the game over state
}






#endif