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
        .ambient_color = {80, 80, 100, 0xFF},
        .directional_color = {0xEE, 0xAA, 0xAA, 0xFF},
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
    t3d_light_set_count(2);
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
    light->ambient_color[0] = 80;
	light->ambient_color[1] = 80;
	light->ambient_color[2] = 100;
    light_set(light);
}

#endif