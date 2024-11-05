#ifndef SCREEN_H
#define SCREEN_H


typedef struct {
	
	surface_t depthBuffer; 
	T3DViewport gameplay_viewport[4];

} Screen;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240


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

void screen_clearT3dViewport(T3DViewport* vp)
{
	t3d_frame_start();
	t3d_viewport_attach(vp);
}

void screen_initT3dViewport(Screen* screen)
{
	switch (ACTOR_COUNT)
	{
		case 1:
			screen->gameplay_viewport[0] = t3d_viewport_create();
			t3d_viewport_set_area(&screen->gameplay_viewport[0],   0,               0,              SCREEN_WIDTH,      SCREEN_HEIGHT);
			break;
		case 2:
			screen->gameplay_viewport[0] = t3d_viewport_create();
			screen->gameplay_viewport[1] = t3d_viewport_create();
			t3d_viewport_set_area(&screen->gameplay_viewport[0], 0,             0,               SCREEN_WIDTH,   SCREEN_HEIGHT/2);
			t3d_viewport_set_area(&screen->gameplay_viewport[1], 0,             SCREEN_HEIGHT/2, SCREEN_WIDTH,   SCREEN_HEIGHT/2);
			break;
		case 3:
			screen->gameplay_viewport[0] = t3d_viewport_create();
			screen->gameplay_viewport[1] = t3d_viewport_create();
			screen->gameplay_viewport[2] = t3d_viewport_create();
			t3d_viewport_set_area(&screen->gameplay_viewport[0], 0,              0,               SCREEN_WIDTH,     SCREEN_HEIGHT/2);
			t3d_viewport_set_area(&screen->gameplay_viewport[1], 0,              SCREEN_HEIGHT/2, SCREEN_WIDTH/2,   SCREEN_HEIGHT/2-2);
			t3d_viewport_set_area(&screen->gameplay_viewport[2], SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2-2, SCREEN_HEIGHT/2-2);
			break;
		case 4:
			screen->gameplay_viewport[0] = t3d_viewport_create();
			screen->gameplay_viewport[1] = t3d_viewport_create();
			screen->gameplay_viewport[2] = t3d_viewport_create();
			screen->gameplay_viewport[3] = t3d_viewport_create();
			t3d_viewport_set_area(&screen->gameplay_viewport[0], 0,              0,               SCREEN_WIDTH/2,   SCREEN_HEIGHT/2);
			t3d_viewport_set_area(&screen->gameplay_viewport[1], SCREEN_WIDTH/2, 0,               SCREEN_WIDTH/2-2, SCREEN_HEIGHT/2);
			t3d_viewport_set_area(&screen->gameplay_viewport[2], 0,              SCREEN_HEIGHT/2, SCREEN_WIDTH/2,   SCREEN_HEIGHT/2-2);
			t3d_viewport_set_area(&screen->gameplay_viewport[3], SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2-2, SCREEN_HEIGHT/2-2);
			break;
	}
}


#endif