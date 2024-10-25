#ifndef UI_SPRITE_H
#define UI_SPRITE_H

#include "ui_file.h"

#ifdef __cplusplus
extern "C" {
#endif

// Constant dimensions for sprites
const int spriteWidth = 16, spriteHeight = 16;

// Static pointers to sprites representing different controller elements
static sprite_t *sprite_controlStick;
static sprite_t *sprite_dPadTriggers;
static sprite_t *sprite_cButtons0;
static sprite_t *sprite_cButtons1;
static sprite_t *sprite_faceButtons0;
static sprite_t *sprite_faceButtons1;

// Static pointers to sprites for panels
static sprite_t *sprite_border;
static sprite_t *sprite_gloss;
static sprite_t *sprite_gradient;
static sprite_t *sprite_bubbleGrid;
static sprite_t *sprite_tessalate;
static sprite_t *sprite_star;

// Surfaces for rendering UI elements
surface_t surf_UIpanels;
surface_t surf_UIsprites;

/* Declarations */

void ui_spriteLoad(void);
inline void ui_syncSprite(int color);
void ui_spriteDraw(rdpq_tile_t tile, sprite_t *sprite, int idx, int x, int y);
void ui_spriteDrawPanel(rdpq_tile_t tile, sprite_t *sprite, int color, int x0, int y0, int x1, int y1, int s, int t, int s1, int t1);
void ui_spriteCleanup(void);

/* Definitions */

// Loads and assigns sprites to their corresponding pointers based on file paths set by ui_fileSprites.
void ui_spriteLoad(void)
{
    ui_fileSprites();

    // Load IA format sprites (grayscale with alpha for UI overlays).
    sprite_border = sprite_load(uiSpritePanelFileName[0]);
    sprite_gloss = sprite_load(uiSpritePanelFileName[1]);
    sprite_gradient = sprite_load(uiSpritePanelFileName[2]);
    sprite_bubbleGrid = sprite_load(uiSpritePanelFileName[3]);
    sprite_tessalate = sprite_load(uiSpritePanelFileName[4]);
    sprite_star = sprite_load(uiSpritePanelFileName[5]);
    sprite_controlStick = sprite_load(uiSpriteButtonFileName[0]);
    sprite_dPadTriggers = sprite_load(uiSpriteButtonFileName[1]);

    // Load RGBA32 format sprites (full color with transparency for UI buttons).
    sprite_cButtons0 = sprite_load(uiSpriteButtonFileName[2]);
    sprite_cButtons1 = sprite_load(uiSpriteButtonFileName[3]);
    sprite_faceButtons0 = sprite_load(uiSpriteButtonFileName[4]);
    sprite_faceButtons1 = sprite_load(uiSpriteButtonFileName[5]);
}

// Optional RDPQ sync and set for sprites. Similar to ui_syncText, but sets the combiner for textures and allows for primitive color to added.
inline void ui_syncSprite(int color)
{
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(ui_color(color));
    rdpq_sync_tile();
}

// Draws a simple 16x16 sprite.
void ui_spriteDraw(rdpq_tile_t tile, sprite_t *sprite, int idx, int x, int y)
{
    int s = 0, t = 0;
    int idxCopy = idx;


    if(idx > 4)
    {
        idx = idx % 4;
        s = spriteWidth * idx;
    } else {
        s = spriteWidth * idx;
    }

    t = (idxCopy / 4) * spriteHeight;

    ui_syncSprite(WHITE);

    surf_UIsprites = sprite_get_pixels(sprite);

    rdpq_tex_upload_sub(tile, &surf_UIsprites, NULL, s, t, s+spriteWidth, t+spriteHeight);
    rdpq_texture_rectangle(tile, x, y, x+spriteWidth, y+spriteHeight, s, t);
}

// Draws a scalable sprite with added primitive color.
void ui_spriteDrawPanel(rdpq_tile_t tile, sprite_t *sprite, int color, int x0, int y0, int x1, int y1, int s, int t, int s1, int t1)
{

    ui_syncSprite(color);

    surf_UIpanels = sprite_get_pixels(sprite);

    rdpq_tex_upload(tile, &surf_UIpanels, NULL);
    rdpq_texture_rectangle_scaled(tile, x0, y0, x1, y1, s, t, s1, t1);


}

// Frees static pointers to sprites.
void ui_spriteCleanup(void)
{
    sprite_free(sprite_controlStick);
    sprite_free(sprite_dPadTriggers);
    sprite_free(sprite_cButtons0);
    sprite_free(sprite_cButtons1);
    sprite_free(sprite_faceButtons0);
    sprite_free(sprite_faceButtons1);
    sprite_free(sprite_border);
    sprite_free(sprite_gloss);
    sprite_free(sprite_gradient);
    sprite_free(sprite_bubbleGrid);
    sprite_free(sprite_tessalate);
    sprite_free(sprite_star);
}

#ifdef __cplusplus
}
#endif

#endif // UI_SPRITE_H