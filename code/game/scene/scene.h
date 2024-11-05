#ifndef SCENE_H
#define SCENE_H

typedef struct
{
	Camera camera[4];
	LightData light;

}Scene;


void scene_init(Scene *scene)
{
    for(uint32_t i = 0; i < ACTOR_COUNT; i++)
        scene->camera[i] = camera_create();
    scene->light = light_create();
}

#endif