#ifndef UI_H
#define UI_H

#include "ui/ui_font.h"
#include "ui/ui_colors.h"
#include "ui/ui_sprite.h"

enum cStates
{
    C_UP,
    C_DOWN,
    C_LEFT,
    C_RIGHT
};

enum MENU_TEXT {
    TEXT_DIFF,
    TEXT_PLAYERS = +4,
    TEXT_BOTS,
    TEXT_CONTROLS,
    TEXT_RUMBLE,
    MENU_TEXT_COUNT
};

const char* uiMainMenuStrings[MENU_TEXT_COUNT] = {
    "Difficulty: ",
    "EASY",
    "MEDIUM",
    "HARD",
    "Players: ",
    "Bots: ",
    "       to Move",
    "Insert Rumble Pak now!"
};

float textPositions[2];
float panelPositions[4];

void ui_init(void);
void ui_print(bool fps);
void ui_cleanup(void);

// Loads, registers and sets a font with style
void ui_init(void)
{
    ui_fontRegister();
    ui_spriteLoad();
    textPositions[0] = SCREEN_WIDTH/2; // x
    textPositions[1] = SCREEN_HEIGHT/2; // y
    panelPositions[0] = SCREEN_WIDTH/2; // x0
    panelPositions[1] = SCREEN_HEIGHT/2; // y0
    panelPositions[2] = SCREEN_WIDTH; // x1
    panelPositions[3] = SCREEN_HEIGHT; // y1
}

// Optional RDPQ sync and set for text, to prevent bleeding if the autosync engine misses something.
void ui_syncText(void)
{
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_sync_tile();
}

void ui_fps(float frame_rate, float x, float y)
{
    heap_stats_t heap_stats;
    sys_get_heap_stats(&heap_stats);
    ui_syncText();
    rdpq_text_printf(&txt_debugParms, ID_DEBUG, x, y, "FPS %.2f Mem: %d KiB", frame_rate, heap_stats.used/1024);
}

void ui_printf(float x, float y, const char *txt, ...)
{
    ui_syncText();

    va_list args;
    va_start(args, txt);
    rdpq_text_vprintf(&txt_debugParms, ID_DEBUG, x, y, txt, args);
}

// Catch-all print function for Countdown, Score and Winner, with optional FPS print
void ui_print(bool fps)
{
    if (countDownTimer > 0.0f) {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_printf(&textparms, ID_DEFAULT, 0, 36, "Strawberry Byte\nPresents\n\nholes: Clone of Hole.io\n\n%d", (int)ceilf(countDownTimer));
    } else if (countDownTimer > -GO_DELAY) {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_print(&textparms, ID_DEFAULT, 0, 126, "GO!");
    } else if (isEnding && endTimer >= WIN_SHOW_DELAY) {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_printf(&textparms, ID_DEFAULT, 0, 116, "Player %d wins!\nScore: %u", winner+1, players[winner].score);
    } else {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_printf(&textparms, ID_DEFAULT, 0, 38, "P1: %u    P2: %u    P3: %u    P4: %u", players[0].score, players[1].score, players[2].score, players[3].score);
    }

    if(fps)
    {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_printf(&textparms, ID_DEFAULT, 0, 220, "FPS %.2f", display_get_fps());
    }
}

void ui_print_winner(int winner)
{
    ui_spriteDrawPanel(TILE1, sprite_gloss, T_BLACK, panelPositions[0]-64, panelPositions[1]-18, panelPositions[3]-30, panelPositions[1]+10, 0, 0, 64, 64);
    ui_syncText();
    if(winner !=5) // 5 signifies a Draw
    {
        rdpq_textparms_t winnerTextParms = txt_gameParms;
        winnerTextParms.style_id = STYLE_PLAYER + winner-1;
        rdpq_text_printf(&winnerTextParms, ID_DEFAULT, textPositions[0]-50, textPositions[1], "Player %d Wins", winner);
    } else {
        rdpq_text_print(&txt_gameParms, ID_DEFAULT, textPositions[0]-20, textPositions[1], "TIE!");
    }
}

void ui_playerScores(player_data* player)
{
    static float base[] = {32,32};
    float position[] = {base[0],base[1]};
    ui_syncText();
    rdpq_textparms_t playerTextParms = txt_gameParms;
    playerTextParms.style_id = STYLE_PLAYER + (player->plynum - 1);
    rdpq_text_printf(&playerTextParms, ID_DEFAULT, position[0] + 75 * (player->plynum - 1), position[1], "P%d : %u", player->plynum, player->score);
}

void ui_countdown(int secondsLeft)
{
    // Convert secondsLeft integer to a string
    char countdownText[2];
    snprintf(countdownText, sizeof(countdownText), "%d", secondsLeft);

    ui_syncText();
    rdpq_text_printf(&txt_titleParms, ID_TITLE, textPositions[0]-10, textPositions[1]-10, "%s", countdownText);

}


// Controller data is passed here for visual feedback for the button press.
void ui_main_menu(joypad_buttons_t* control, int diff)
{
    // Panels
    ui_spriteDrawPanel(TILE2, sprite_gloss, T_BLUE, panelPositions[0]-70, panelPositions[1]-80, panelPositions[2]-90, panelPositions[3]-116, 0, 0, 64, 64);
    ui_spriteDrawPanel(TILE3, sprite_tessalate, T_BLACK, panelPositions[0]-60, panelPositions[1]-75, panelPositions[2]-100, panelPositions[3]-126, 0, 0, 64, 64);

    // Buttons
    if(control->start)
    {
        ui_spriteDraw(TILE4, sprite_faceButtons0, 1, textPositions[0]+10, textPositions[1]-30);
    } else {
        ui_spriteDraw(TILE4, sprite_faceButtons0, 0, textPositions[0]+10, textPositions[1]-30);
    }
    ui_spriteDraw(TILE5, sprite_controlStick, 0, textPositions[0]-68, textPositions[1]+50);
    int stickX = 92;
    int stickY = 138+(spriteHeight*2);
    ui_spriteDraw(TILE5, sprite_controlStick, 1, stickX, stickY);

    // @TODO: Add D Pad

    int count = core_get_playercount();

    // Text
    ui_syncText();
    rdpq_text_print(&txt_titleParms, ID_TITLE, textPositions[0]-54, textPositions[1]-56, " holes\nA Clone of Hole.io");
    rdpq_text_print(&txt_gameParms, ID_DEFAULT, textPositions[0]-32, textPositions[1]-18, "Press");
    rdpq_text_printf(&txt_gameParms, ID_DEFAULT, textPositions[0]-68, textPositions[1]+20, 
        "%s %s\n"
        "%s %d\n"
        "%s %d\n"
        "%s\n\n"
        "%s\n",
        uiMainMenuStrings[TEXT_DIFF], uiMainMenuStrings[TEXT_DIFF+diff+1],
        uiMainMenuStrings[TEXT_PLAYERS], count,
        uiMainMenuStrings[TEXT_BOTS], MAXPLAYERS - count,
        uiMainMenuStrings[TEXT_CONTROLS],
        uiMainMenuStrings[TEXT_RUMBLE]
    );
}

void ui_pause(joypad_buttons_t* control)
{

    ui_spriteDrawPanel(TILE2, sprite_gloss, T_BLUE, 90, 60, 230, 144, 0, 0, 64, 64);
    ui_spriteDrawPanel(TILE3, sprite_tessalate, T_BLACK, 100, 65, 220, 134, 0, 0, 64, 64);

    if(control->start)
    {
        ui_spriteDraw(TILE4, sprite_faceButtons0, 1, 170, 110);
    } else {
        ui_spriteDraw(TILE4, sprite_faceButtons0, 0, 170, 110);
    }

    ui_spriteDraw(TILE5, sprite_dPadTriggers, 5, 160, 180);

    ui_syncText();
    rdpq_text_print(&txt_titleParms, ID_TITLE, 106, 96, "holes");
    rdpq_text_print(&txt_gameParms, ID_DEFAULT, 128, 122, "Press\n\n\n  PAUSED\n\nHold       to\nQuit Game");
}


void ui_intro(joypad_buttons_t* control)
{
    // Basic frame counter for timing
    static uint32_t introTimer = 0;
    introTimer++;

    // Animated text positions
    static float topTextPosition[] = {93.0f, 0.0f};
    topTextPosition[1] = topTextPosition[1] + 2.0f;
    if(topTextPosition[1] > 56.0f) topTextPosition[1] = 56.0f;

    if (introTimer < 100) {

/* STRAWBERRY SCREEN */

        // Panels
        ui_spriteDrawPanel(TILE1, sprite_strawberryTop, WHITE, 128, 80,196,112,0,0,32,16);
        if(introTimer >= 240)
        {
            ui_spriteDrawPanel(TILE2, sprite_strawberry1, WHITE,   128,112,196,144,0,0,32,16);
        } else {
            ui_spriteDrawPanel(TILE2, sprite_strawberry0, WHITE,   128,112,196,144,0,0,32,16);
        }

        // Buttons
        if(control->start)
        {
            ui_spriteDraw(TILE3, sprite_faceButtons0, 1, 132, 214);
        } else {
            ui_spriteDraw(TILE3, sprite_faceButtons0, 0, 132, 214);
        }

        // Text
        ui_syncText();
        rdpq_text_print(&txt_titleParms, ID_TITLE, 68, 56, "Strawberry Byte");
        if(introTimer >= 240) rdpq_text_print(&txt_titleParms, ID_TITLE, 110, 190, "Presents");
        rdpq_text_print(&txt_gameParms, ID_DEFAULT, 90, 226, "Press        to Skip Intro");

    } else {

        // Buttons
        if(control->start)
        {
            ui_spriteDraw(TILE3, sprite_faceButtons0, 1, 170, 66);
        } else {
            ui_spriteDraw(TILE3, sprite_faceButtons0, 0, 170, 66);
        }

        // Text
        ui_syncText();
        rdpq_text_print(&txt_titleParms, ID_TITLE, 106, 40, " holes\nA Clone of Hole.io");
        rdpq_text_print(&txt_gameParms, ID_DEFAULT, 128, 78, "Press");
        rdpq_text_print(&txt_titleParms, ID_DEFAULT, 32, 94, "CREDITS:");

        // @TODO: Probably should make this a rdpq_paragraph
        rdpq_text_print(&txt_gameParms, ID_DEBUG, 32, 114, 
            "Programming: s4ys\n"
            "Models: s4ys\n"
            "Strawberry Sprite by Sonika Rud\n"
            "UI Sprites by Kenney\n"
            "Music: 'The Morning After' by Soft One\n"
            "SFX obtained from Freesound");
    }
    
}

void ui_cleanup(void)
{
    ui_spriteCleanup();
    ui_fontUnregister();
    ui_fileCleanup();
}

#endif // UI_H