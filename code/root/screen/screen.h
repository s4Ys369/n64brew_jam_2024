#ifndef SCREEN_H
#define SCREEN_H


typedef struct {
	
	surface_t depthBuffer; 
	T3DViewport gameplay_viewport;
	T3DViewport main_menu_viewport;
	T3DViewport current_viewport;

} Screen;

typedef struct Scissor {
	int32_t v[4];
} Scissor;

enum ScissorIDs {
	SCISSOR_DEFAULT,
	SCISSOR_MAIN_MENU,
	SCISSOR_COUNT
};

Scissor scissors[SCISSOR_COUNT] = {
	{{  0,  0, 320, 240}},
	{{ 90, 64, 140, 140}},
};


void screen_initDisplay(Screen* screen);
void screen_clearDisplay(Screen* screen);


void screen_initDisplay(Screen* screen)
{
	display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
	screen->depthBuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());
}

void screen_clearDisplay(Screen* screen)
{
	rdpq_attach(display_get(), &screen->depthBuffer);
	t3d_screen_clear_color(RGBA32(154, 181, 198, 0xFF));
	t3d_screen_clear_depth();
}

void screen_initT3dViewport(T3DViewport vp, uint8_t scissorIdx)
{
	vp = t3d_viewport_create();
	Scissor sx = scissors[scissorIdx];
	t3d_viewport_set_area(&vp, sx.v[0], sx.v[1], sx.v[2], sx.v[3]);
}	

void screen_clearT3dViewport(T3DViewport vp)
{
	t3d_frame_start();
	t3d_viewport_attach(&vp);
}


#endif