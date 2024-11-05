#ifndef UI_H
#define UI_H

#include "ui_font.h"
#include "ui_colors.h"
#include "ui_sprite.h"

#ifdef __cplusplus
extern "C" {
#endif

// @TODO: Recreate button sprite tiling to remove these insanity checks.
#define aPressed 2
#define aIdle 3
#define aHeld 2

#define bPressed 0
#define bIdle 1
#define bHeld 3

#define cIdle 0
enum cStates
{
    C_UP,
    C_DOWN,
    C_LEFT,
    C_RIGHT
};

// @TODO: Unhardcode position, as it effects ui_printf as well.
T3DVec3 fpsPos = {{32.0f,32.0f,1.0f}};

/* Declarations */

void ui_init(void);
inline void ui_syncText(void);
void ui_fps(void);
void ui_printf(const char *txt, ...);
void ui_main_menu(ControllerData* control);
void ui_input_display(ControllerData* control);
void ui_textbox(void);
void ui_cleanup(void);

/* Definitons */

void ui_init(void)
{
    ui_fontRegister();
    ui_spriteLoad();
}

// Optional RDPQ sync and set for text, to prevent bleeding if the autosync engine misses something.
inline void ui_syncText(void)
{
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_sync_tile();
}

void ui_fps(void)
{
    heap_stats_t heap_stats;
    sys_get_heap_stats(&heap_stats);
    ui_syncText();
    rdpq_text_printf(&txt_debugParms, ID_DEBUG, fpsPos.v[0], fpsPos.v[1], "FPS %.2f Mem: %d KiB", display_get_fps(), heap_stats.used/1024);
}

void ui_printf(const char *txt, ...)
{
    ui_syncText();

    va_list args;
    va_start(args, txt);
    rdpq_text_vprintf(&txt_debugParms, ID_DEBUG, fpsPos.v[0], fpsPos.v[1] + 20, txt, args);
}

// Controller data is passed here for visual feedback for the button press.
void ui_main_menu(ControllerData* control)
{
    ui_spriteDrawPanel(TILE1, sprite_gradient, T_RED, 96, 64, 224, 140, 0, 0, 128, 64);
    ui_spriteDrawPanel(TILE2, sprite_tessalate, T_BLACK, 112, 74, 208, 131, 0, 0, 64, 64);

    ui_syncText();
    rdpq_text_print(&txt_titleParms, ID_TITLE, 136, 96, "TITLE");
    rdpq_text_print(&txt_gameParms, ID_DEFAULT, 128, 120, "Press");

    ui_spriteDrawPanel(TILE4, sprite_star, YELLOW, 100, 74, 132, 106, 0, 0, 64, 64);
    ui_spriteDrawPanel(TILE4, sprite_star, YELLOW, 187, 74, 219, 106, 0, 0, 64, 64);

    if(control->pressed.start || control->held.start)
    {
        ui_spriteDraw(TILE3, sprite_faceButtons0, 1, 170, 108);
    } else {
        ui_spriteDraw(TILE3, sprite_faceButtons0, 0, 170, 108);
    }
}

// Time to crash test the RDP
void ui_input_display(ControllerData* control)
{
    int s = 24;
    int t = 164;

    // First row: Triggers
    if(control->pressed.l || control->held.l)
        ui_spriteDraw(TILE0, sprite_dPadTriggers, 6, s, t);
    if(control->pressed.z || control->held.z)
        ui_spriteDraw(TILE1, sprite_dPadTriggers, 5, s+spriteWidth, t);
    if(control->pressed.r || control->held.r)
        ui_spriteDraw(TILE2, sprite_dPadTriggers, 7, s+(spriteWidth*2), t);

    // Second row: Face Buttons
    if(control->pressed.a || control->held.a)
    {
        ui_spriteDraw(TILE3, sprite_faceButtons1, aHeld, s, t+spriteHeight);
    } else {
        ui_spriteDraw(TILE3, sprite_faceButtons0, aIdle, s, t+spriteHeight);
    }

    if(control->pressed.b || control->held.b)
    {
        ui_spriteDraw(TILE4, sprite_faceButtons1, bHeld, s+spriteHeight, t+spriteHeight);
    } else {
        ui_spriteDraw(TILE4, sprite_faceButtons1, bIdle, s+spriteHeight, t+spriteHeight);
    }

    if(control->pressed.start || control->held.start)
    {
        ui_spriteDraw(TILE5, sprite_faceButtons0, 1, s+(spriteHeight*2), t+spriteHeight);
    } else {
        ui_spriteDraw(TILE5, sprite_faceButtons0, 0, s+(spriteHeight*2), t+spriteHeight);
    }

    // Third row: Sticks
    ui_spriteDraw(TILE6, sprite_controlStick, 0, s, t+(spriteHeight*2));
    int stickX = s+(control->input.stick_x/15);
    int stickY = t+(spriteHeight*2)-(control->input.stick_y/15);
    ui_spriteDraw(TILE6, sprite_controlStick, 1, stickX, stickY);
    if(control->pressed.c_up || control->held.c_up)
    {
        ui_spriteDraw(TILE7, sprite_cButtons1, C_UP, s+(spriteHeight*2), t+(spriteHeight*2));
    } else if(control->pressed.c_down || control->held.c_down) {
        ui_spriteDraw(TILE7, sprite_cButtons1, C_DOWN, s+(spriteHeight*2), t+(spriteHeight*2));
    } else if(control->pressed.c_left || control->held.c_left) {
        ui_spriteDraw(TILE7, sprite_cButtons1, C_LEFT, s+(spriteHeight*2), t+(spriteHeight*2));
    } else if(control->pressed.c_right || control->held.c_right) {
        ui_spriteDraw(TILE7, sprite_cButtons1, C_RIGHT, s+(spriteHeight*2), t+(spriteHeight*2));
    } else {
        ui_spriteDraw(TILE7, sprite_cButtons0, 0, s+(spriteHeight*2), t+(spriteHeight*2));
    }

}

// @TODO: Needs integration of text parsing from upstream, currently does nothing.
void ui_textbox(void)
{

}

void ui_cleanup(void)
{
    ui_spriteCleanup();
    ui_fontUnregister();
    ui_fileCleanup();
}

#ifdef __cplusplus
}
#endif

#endif // UI_H