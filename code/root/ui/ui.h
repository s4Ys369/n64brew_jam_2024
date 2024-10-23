#ifndef UI_H
#define UI_H

#include <libdragon.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ColorNames
{
    // Standard ROYGBIV
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    INDIGO,
    VIOLET,
    // RGB 0 (full black) & 1 (full white)
    BLACK,
    WHITE,
    // RGB 1 * (n*.25f)
    LIGHT_GREY, // n = 3
    GREY, // n = 2
    DARK_GREY, // n = 1
    // Transparent Colors
    TRANSPARENT,
    T_RED,
    T_ORANGE,
    T_YELLOW,
    T_GREEN,
    T_BLUE,
    T_INDIGO,
    T_VIOLET,
    T_BLACK,
    T_WHITE,
    T_GREY,
    // Darker Variants
    DARK_RED,
    DARK_GREEN,
    // N64 Logo Colors
    N_RED,
    N_YELLOW,
    N_GREEN,
    N_BLUE,
    COLOR_COUNT
};

const uint32_t COLORS[COLOR_COUNT] = 
{
    0xD90000FF, // RED
    0xFF6822FF, // ORANGE
    0xFFDA21FF, // YELLOW
    0x33DD00FF, // GREEN
    0x1133CCFF, // BLUE
    0x220066FF, // INDIGO
    0x330044FF, // VIOLET
    0x000000FF, // BLACK
    0xFFFFFFFF, // WHITE
    0xC0C0C0FF, // LIGHT_GREY  
    0x808080FF, // GREY        
    0x404040FF, // DARK_GREY   
    0x0000007F, // TRANSPARENT 
    0xD90000C8, // T_RED       
    0xFF6822C8, // T_ORANGE    
    0xFFDA21C8, // T_YELLOW    
    0x33DD00C8, // T_GREEN     
    0x1133CCC8, // T_BLUE      
    0x220066C8, // T_INDIGO    
    0x330044C8, // T_VIOLET    
    0x1F1F1FC8, // T_BLACK     
    0xFFFFFFC8, // T_WHITE     
    0xC0C0C0C8, // T_GREY      
    0x820000FF, // DARK_RED    
    0x006400FF, // DARK_GREEN  
    0xE10916FF, // N_RED       
    0xF5B201FF, // N_YELLOW    
    0x319900FF, // N_GREEN     
    0x01009AFF, // N_BLUE      
};

#define FONT_ID_GAME 2
enum FONT_IDS 
{
    ID_RESERVED,
    ID_DEBUG,
    ID_DEFAULT,
    ID_TITLE,
    ID_COUNT
};
enum FONT_STYLES 
{
    STYLE_DEBUG,
    STYLE_DEFAULT,
    STYLE_TITLE,
    STYLE_BRIGHT,
    STYLE_GREEN,
    STYLE_COUNT
};

T3DVec3 fpsPos = {{32.0f,32.0f,1.0f}};
rdpq_textparms_t txt_debugParms;
rdpq_textparms_t txt_titleParms;
rdpq_textparms_t txt_gameParms;
rdpq_fontstyle_t txt_debug_fontStyle;
rdpq_fontstyle_t txt_title_fontStyle;
rdpq_fontstyle_t txt_game_fontStyle;
rdpq_fontstyle_t txt_bright_fontStyle;
rdpq_fontstyle_t txt_green_fontStyle;


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


static sprite_t *sprite_controlStick;
static sprite_t *sprite_dPadTriggers;
static sprite_t *sprite_cButtons0;
static sprite_t *sprite_cButtons1;
static sprite_t *sprite_faceButtons0;
static sprite_t *sprite_faceButtons1;
const int spriteWidth = 16, spriteHeight = 16;

static sprite_t *sprite_border;
static sprite_t *sprite_gloss;
static sprite_t *sprite_gradient;
static sprite_t *sprite_bubbleGrid;
static sprite_t *sprite_tessalate;
static sprite_t *sprite_star;

surface_t surf_UIpanels;
surface_t surf_UIsprites;

const char* basePath = "rom:/root/ui/";
char fullPath[256];

const char* ui_get_file(const char* fn)
{
    
    sprintf(fullPath, "%s%s", basePath, fn);

    return fullPath;
}

void ui_load_sprites(void)
{
    // IA
    sprite_border = sprite_load(ui_get_file("panels/border.ia4.sprite"));
    sprite_gloss = sprite_load(ui_get_file("panels/gloss.ia4.sprite"));
    sprite_gradient = sprite_load(ui_get_file("panels/gradient.ia4.sprite"));
    sprite_bubbleGrid = sprite_load(ui_get_file("panels/pattern_bubble_grid.ia4.sprite"));
    sprite_tessalate = sprite_load(ui_get_file("panels/pattern_tessalate.ia4.sprite"));
    sprite_star = sprite_load(ui_get_file("panels/star.ia8.sprite"));
    sprite_controlStick = sprite_load(ui_get_file("buttons/control_stick.ia8.sprite"));
    sprite_dPadTriggers = sprite_load(ui_get_file("buttons/d_pad_triggers.ia8.sprite"));

    // RGBA32
    sprite_cButtons0 = sprite_load(ui_get_file("buttons/c_buttons0.rgba32.sprite"));
    sprite_cButtons1 = sprite_load(ui_get_file("buttons/c_buttons1.rgba32.sprite"));
    sprite_faceButtons0 = sprite_load(ui_get_file("buttons/face_buttons0.rgba32.sprite"));
    sprite_faceButtons1 = sprite_load(ui_get_file("buttons/face_buttons1.rgba32.sprite"));
}

void ui_draw_sprite(rdpq_tile_t tile, sprite_t *sprite, int idx, int posX, int posY)
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

    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1);
    rdpq_mode_filter(FILTER_BILINEAR);
    rdpq_sync_tile();

    surf_UIsprites = sprite_get_pixels(sprite);

    rdpq_tex_upload_sub(tile, &surf_UIsprites, NULL, s, t, s+spriteWidth, t+spriteHeight);
    rdpq_texture_rectangle(tile, posX, posY, posX+spriteWidth, posY+spriteHeight, s, t);
}

void ui_draw_panel(rdpq_tile_t tile, sprite_t *sprite, int color, int x0, int y0, int x1, int y1, int s, int t, int s1, int t1)
{
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_TEX_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(color_from_packed32(COLORS[color]));
    rdpq_sync_tile();

    surf_UIpanels = sprite_get_pixels(sprite);

    rdpq_tex_upload(tile, &surf_UIpanels, NULL);
    rdpq_texture_rectangle_scaled(tile, x0, y0, x1, y1, s, t, s1, t1);
}

void ui_register_fonts(void)
{
    // Load font64 and register to fonts starting at index 2, 0 reserved by the SDK, 1 should be reserved debug output
    rdpq_font_t *font[ID_COUNT];
    font[ID_DEBUG] = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_VAR);;
    font[ID_DEFAULT] = rdpq_font_load(ui_get_file("fonts/TitanOne-Regular.font64"));
    font[ID_TITLE] = rdpq_font_load(ui_get_file("fonts/chunkysans.font64"));

    // Create and register font styles
    txt_debug_fontStyle.color = color_from_packed32(COLORS[YELLOW]);
    txt_debug_fontStyle.outline_color = color_from_packed32(COLORS[BLACK]);

    txt_game_fontStyle.color = color_from_packed32(COLORS[WHITE]);
    txt_debug_fontStyle.outline_color = color_from_packed32(COLORS[T_BLACK]);

    txt_title_fontStyle.color = color_from_packed32(COLORS[GREEN]);
    txt_title_fontStyle.outline_color = color_from_packed32(COLORS[DARK_GREEN]);

    txt_bright_fontStyle.color = color_from_packed32(COLORS[YELLOW]);
    txt_debug_fontStyle.outline_color = color_from_packed32(COLORS[T_BLACK]);

    txt_green_fontStyle.color = color_from_packed32(COLORS[GREEN]);
    txt_title_fontStyle.outline_color = color_from_packed32(COLORS[DARK_GREEN]);

    for (int i = 1; i < ID_COUNT; i++)
    {
        rdpq_text_register_font(i, font[i]);
        rdpq_font_style(
            font[i], 
            STYLE_DEFAULT, 
            &txt_game_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_TITLE, 
            &txt_title_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_BRIGHT, 
            &txt_bright_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_GREEN, 
            &txt_green_fontStyle
        );
    }

    rdpq_font_style(
        font[ID_DEBUG], 
        STYLE_DEBUG, 
        &txt_debug_fontStyle
    );

    txt_debugParms = (rdpq_textparms_t){.style_id = STYLE_DEBUG};
    txt_titleParms = (rdpq_textparms_t){.style_id = STYLE_TITLE};
    txt_gameParms = (rdpq_textparms_t){.style_id = STYLE_BRIGHT};

}


void ui_init(void)
{
    ui_register_fonts();
    ui_load_sprites();
}

void ui_fps(void)
{
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_text_printf(&txt_debugParms, ID_DEBUG, fpsPos.v[0], fpsPos.v[1], "FPS %.2f", display_get_fps());
}

void ui_printf(const char *txt, ...)
{
    va_list args;
    va_start(args, txt);
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_text_printf(&txt_debugParms, ID_DEBUG, fpsPos.v[0], fpsPos.v[1] + 20, txt, args);
}

void ui_main_menu(ControllerData* control)
{
    ui_draw_panel(TILE1, sprite_gradient, T_RED, 96, 64, 224, 140, 0, 0, 128, 64);
    ui_draw_panel(TILE2, sprite_tessalate, T_BLACK, 112, 74, 208, 131, 0, 0, 64, 64);
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_text_print(&txt_titleParms, ID_TITLE, 136, 96, "TITLE");
    rdpq_text_print(&txt_gameParms, ID_DEFAULT, 128, 120, "Press");
    ui_draw_panel(TILE4, sprite_star, YELLOW, 100, 74, 132, 106, 0, 0, 64, 64);
    ui_draw_panel(TILE4, sprite_star, YELLOW, 187, 74, 219, 106, 0, 0, 64, 64);
    if(control->pressed.a || control->held.a)
    {
        ui_draw_sprite(TILE3, sprite_faceButtons1, aHeld, 170, 108);
    } else {
        ui_draw_sprite(TILE3, sprite_faceButtons0, aIdle, 170, 108);
    }
}

void ui_textbox(void)
{

}

// Time to crash test the RDP
void ui_input_display(ControllerData* control)
{
    int s = 24;
    int t = 164;
    controllerData_getInputs(control);
    rspq_syncpoint_t syncPoint0 = 0;

    // First row: Triggers
    ui_draw_sprite(TILE0, sprite_dPadTriggers, 6, s, t);
    if(control->pressed.z || control->held.z)
        ui_draw_sprite(TILE1, sprite_dPadTriggers, 5, s+spriteWidth, t);
    if(control->pressed.r || control->held.r)
        ui_draw_sprite(TILE2, sprite_dPadTriggers, 7, s+(spriteWidth*2), t);

    if(syncPoint0)rspq_syncpoint_wait(syncPoint0);
    rspq_syncpoint_t syncPoint1 = rspq_syncpoint_new();

    // Second row: Face Buttons
    if(control->pressed.a || control->held.a)
    {
        ui_draw_sprite(TILE3, sprite_faceButtons1, aHeld, s, t+spriteHeight);
    } else {
        ui_draw_sprite(TILE3, sprite_faceButtons0, aIdle, s, t+spriteHeight);
    }

    if(control->pressed.b || control->held.b)
    {
        ui_draw_sprite(TILE4, sprite_faceButtons1, bHeld, s+spriteHeight, t+spriteHeight);
    } else {
        ui_draw_sprite(TILE4, sprite_faceButtons1, bIdle, s+spriteHeight, t+spriteHeight);
    }

    if(control->pressed.start || control->held.start)
    {
        ui_draw_sprite(TILE5, sprite_faceButtons0, 1, s+(spriteHeight*2), t+spriteHeight);
    } else {
        ui_draw_sprite(TILE5, sprite_faceButtons0, 0, s+(spriteHeight*2), t+spriteHeight);
    }

    if(syncPoint1)rspq_syncpoint_wait(syncPoint1);
    rspq_syncpoint_t syncPoint2 = rspq_syncpoint_new();

    // Third row: Sticks
    ui_draw_sprite(TILE6, sprite_controlStick, 0, s, t+(spriteHeight*2));
    int stickX = s+(control->input.stick_x/15);
    int stickY = t+(spriteHeight*2)-(control->input.stick_y/15);
    ui_draw_sprite(TILE6, sprite_controlStick, 1, stickX, stickY);
    if(control->pressed.c_up || control->held.c_up)
    {
        ui_draw_sprite(TILE7, sprite_cButtons1, C_UP, s+(spriteHeight*2), t+(spriteHeight*2));
    } else if(control->pressed.c_down || control->held.c_down) {
        ui_draw_sprite(TILE7, sprite_cButtons1, C_DOWN, s+(spriteHeight*2), t+(spriteHeight*2));
    } else if(control->pressed.c_left || control->held.c_left) {
        ui_draw_sprite(TILE7, sprite_cButtons1, C_LEFT, s+(spriteHeight*2), t+(spriteHeight*2));
    } else if(control->pressed.c_right || control->held.c_right) {
        ui_draw_sprite(TILE7, sprite_cButtons1, C_RIGHT, s+(spriteHeight*2), t+(spriteHeight*2));
    } else {
        ui_draw_sprite(TILE7, sprite_cButtons0, 0, s+(spriteHeight*2), t+(spriteHeight*2));
    }

    if(syncPoint2)rspq_syncpoint_wait(syncPoint2);
    syncPoint0 = rspq_syncpoint_new();
}

// Step 4/5: call this AFTER your game logic ends each frame
void ui_update(void)
{

} 

// Step 5/5: render out the UI at the very end
void ui_draw(void)
{
    ui_fps();
    ui_update();
}

#ifdef __cplusplus
}
#endif

#endif // UI_H