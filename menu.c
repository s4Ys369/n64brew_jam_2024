#include <libdragon.h>
#include <string.h>
#include "menu.h"

#define FONT_TEXT       1

static int minigame_sort(const void *a, const void *b)
{
    int idx1 = *(int*)a, idx2 = *(int*)b;
    return strcasecmp(global_minigame_list[idx1].definition.gamename, global_minigame_list[idx2].definition.gamename);
}

char* menu(void)
{
    const color_t ASH_GRAY = RGBA32(0xAD,0xBA,0xBD,0xFF);
    const color_t MAYA_BLUE = RGBA32(0x6C,0xBE,0xED,0xFF);
    const color_t GUN_METAL = RGBA32(0x31,0x39,0x3C,0xFF);
    const color_t REDWOOD = RGBA32(0xB2,0x3A,0x7A,0xFF);

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

    sprite_t *logo = sprite_load("rom:/logo.ci4.sprite");
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

        rdpq_set_mode_copy(true);
        rdpq_sprite_blit(logo, 160-logo->width/2, 0, NULL);

        int x0 = 80;
        int y0 = logo->height + 20;

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
            ycur += rdpq_text_printf(&textparms, FONT_TEXT, x0, ycur, "%d.\t%s", i+1, global_minigame_list[sorted_indices[i]].definition.gamename).advance_y;
        }
        
        rdpq_detach_show();
    }

    rspq_wait();
    sprite_free(logo);
    rdpq_font_free(font);
    display_close();
    return global_minigame_list[sorted_indices[select]].internalname;
}
