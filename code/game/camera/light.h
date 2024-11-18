#ifndef LIGHT_H
#define LIGHT_H


typedef struct{

	uint8_t ambient_color[4];
	uint8_t directional_color[4];
	T3DVec3 direction;
	
} LightData;


void light_set(LightData *light);
void light_setAmbient(LightData *light, uint8_t value);
void light_resetAmbient(LightData *light);


LightData light_create() {
    LightData light = {
        .ambient_color = {0x32, 0x20, 0x06, 0xFF},
        .directional_color = {0xE3, 0x3B, 0x29, 0xFF},
        .direction = {{1.0f, 1.0f, 1.0f}}
    };

    t3d_vec3_norm(&light.direction);

    return light;
}

/* set light
temporary function until i learn how the lights work  */
void light_set(LightData *light)
{
    t3d_light_set_ambient(light->ambient_color);
    t3d_light_set_directional(0, light->directional_color, &light->direction);
    t3d_light_set_directional(1, (uint8_t[4]){0xF5, 0xD8, 0x6E, 0xFF}, &(T3DVec3){{0,0,-1}});
    t3d_light_set_count(3);
}

void light_setAmbient(LightData *light, uint8_t value)
{
    for (size_t i = 0; i < 3; i++)
	{
		light->ambient_color[i] = value;
	}
    light_set(light);
}

void light_resetAmbient(LightData *light)
{
    light->ambient_color[0] = 0x32;
	light->ambient_color[1] = 0x20;
	light->ambient_color[2] = 0x06;
    light_set(light);
}

#endif