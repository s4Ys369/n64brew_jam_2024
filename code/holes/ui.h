#ifndef UI_H
#define UI_H

#define FONT_TEXT           1
#define TEXT_COLOR          0xF5B201FF
#define TEXT_OUTLINE        0x30521AFF

rdpq_font_t *font;

void ui_init(void);
void ui_print(bool fps);
void ui_cleanup(void);

// Loads, registers and sets a font with style
void ui_init(void)
{
    font = rdpq_font_load("rom:/holes/TitanOne-Regular.font64");
    rdpq_text_register_font(FONT_TEXT, font);
    rdpq_font_style(font, 0, &(rdpq_fontstyle_t){.color = color_from_packed32(TEXT_COLOR) });
}

// Catch-all print function for Countdown, Score and Winner, with optional FPS print
void ui_print(bool fps)
{
    if (countDownTimer > 0.0f) {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_printf(&textparms, FONT_TEXT, 0, 36, "Strawberry Byte\nPresents\n\nholes: Clone of Hole.io\n\n%d", (int)ceilf(countDownTimer));
    } else if (countDownTimer > -GO_DELAY) {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_print(&textparms, FONT_TEXT, 0, 126, "GO!");
    } else if (isEnding && endTimer >= WIN_SHOW_DELAY) {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_printf(&textparms, FONT_TEXT, 0, 116, "Player %d wins!\nScore: %u", winner+1, players[winner].score);
    } else {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_printf(&textparms, FONT_TEXT, 0, 38, "P1: %u    P2: %u    P3: %u    P4: %u", players[0].score, players[1].score, players[2].score, players[3].score);
    }

    if(fps)
    {
        rdpq_textparms_t textparms = { .align = ALIGN_CENTER, .width = 320, .disable_aa_fix=true};
        rdpq_text_printf(&textparms, FONT_TEXT, 0, 220, "FPS %.2f", display_get_fps());
    }
}

// Unregisters and frees font
void ui_cleanup(void)
{
    rdpq_text_unregister_font(FONT_TEXT);
    rdpq_font_free(font);
}

#endif // UI_H