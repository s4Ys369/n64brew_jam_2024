/***************************************************************
                             menu.c
                               
This file contains the code for the basic menu
***************************************************************/

#include <libdragon.h>
#include <string.h>
#include "menu.h"


/*********************************
           Definitions
*********************************/

#define FONT_TEXT       1

/*==============================
    minigame_sort
    Sorts two names alphabetically
    @param  The first name
    @param  The second name
    @return -1 if a is less than b, 1 if a is greater than b, and 0 if they are equal
==============================*/

static int minigame_sort(const void *a, const void *b)
{
    int idx1 = *(int*)a, idx2 = *(int*)b;
    return strcasecmp(global_minigame_list[idx1].definition.gamename, global_minigame_list[idx2].definition.gamename);
}

/*==============================
    menu
    Show the minigame selection menu
    @return The internal name of the selected minigame
==============================*/

char* menu(void)
{
    const color_t BLACK = RGBA32(0x00,0x00,0x00,0xFF);
    const color_t ASH_GRAY = RGBA32(0xAD,0xBA,0xBD,0xFF);
    const color_t MAYA_BLUE = RGBA32(0x6C,0xBE,0xED,0xFF);
    const color_t GUN_METAL = RGBA32(0x31,0x39,0x3C,0xFF);
    const color_t REDWOOD = RGBA32(0xB2,0x3A,0x7A,0xFF);
    const color_t BREWFONT = RGBA32(242,209,155,0xFF);

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

    sprite_t *logo = sprite_load("rom:/n64brew.ia8.sprite");
    sprite_t *jam = sprite_load("rom:/jam.rgba32.sprite");
    
    rdpq_font_t *font = rdpq_font_load("rom:/squarewave.font64");
    rdpq_text_register_font(FONT_TEXT, font);
    rdpq_font_style(font, 0, &(rdpq_fontstyle_t){.color = MAYA_BLUE, .outline_color = GUN_METAL });

    int select = 0;
    float yselect = -1;
    float yselect_target = -1;

    int sorted_indices[global_minigame_count];
    for (int i = 0; i < global_minigame_count; i++) sorted_indices[i] = i;
    qsort(sorted_indices, global_minigame_count, sizeof(int), minigame_sort);

    while (1) {
        joypad_poll();

        joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        if (btn.d_up) select--;
        if (btn.d_down) select++;
        if (btn.a) break;

        if (select < 0) select = 0;
        if (select > global_minigame_count-1) select = global_minigame_count-1;

        surface_t *disp = display_get();

        rdpq_attach(disp, NULL);
        rdpq_clear(ASH_GRAY);

        rdpq_textparms_t textparms = {
            .width = 200, .tabstops = (int16_t[]){ 15 },
        };

        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_mode_combiner(RDPQ_COMBINER2(
            // (1-TEX0)*ENV + (TEX0*PRIM)
            (1,TEX0,ENV,0),             (0,0,0,TEX0),
            (TEX0_BUG,0,PRIM,COMBINED), (0,0,0,COMBINED)
        ));
        rdpq_set_prim_color(BREWFONT);
        rdpq_set_env_color(BLACK);
        rdpq_sprite_blit(logo, 35, 20, NULL);

        rdpq_set_mode_standard();
        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
        rdpq_sprite_blit(jam, 35+190, 10, NULL);

        int x0 = 80;
        int y0 = 20 + logo->height + 20;

        if (yselect_target >= 0) {
            if (yselect < 0) yselect = yselect_target;
            yselect = yselect * 0.9 + yselect_target * 0.1;
            rdpq_set_mode_standard();
            rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
            rdpq_set_prim_color(REDWOOD);
            rdpq_fill_rectangle(x0-10, yselect-12, x0+150, yselect+5);
        }

        rdpq_set_mode_standard();
        int ycur = y0;
        for (int i = 0; i < global_minigame_count; i++) {
            if (select == i) yselect_target = ycur;
            ycur += rdpq_text_printf(&textparms, FONT_TEXT, x0, ycur, "%d.\t%s\n", i+1, global_minigame_list[sorted_indices[i]].definition.gamename).advance_y;
        }
        
        rdpq_detach_show();
    }

    rspq_wait();
    sprite_free(jam);
    sprite_free(logo);
    rdpq_text_unregister_font(FONT_TEXT);
    rdpq_font_free(font);
    display_close();
    return global_minigame_list[sorted_indices[select]].internalname;
}
