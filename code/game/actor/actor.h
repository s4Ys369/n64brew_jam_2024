#ifndef ACTOR_H
#define ACTOR_H


// structures

typedef struct {

	float idle_acceleration_rate;
	float walk_acceleration_rate;
	float run_acceleration_rate;
	float roll_acceleration_rate;
	float roll_acceleration_grip_rate;
	float jump_acceleration_rate;
	float aerial_control_rate;

	float walk_target_speed;
	float run_target_speed;
	float sprint_target_speed;
	float idle_to_roll_target_speed;
	float idle_to_roll_grip_target_speed;
	float walk_to_roll_target_speed;
	float run_to_roll_target_speed;
	float sprint_to_roll_target_speed;
	float jump_target_speed;
	
	float jump_timer_max;

}ActorSettings;


typedef struct {

	float stick_magnitude;
	float stick_x;
	float stick_y;
	float jump_time_held;
	bool jump_hold;
	bool jump_released;

}Actorinput;


typedef struct{

	T3DSkeleton main;
	T3DSkeleton blend;

} ActorArmature;

typedef struct {

	T3DAnim breathing_idle;
	T3DAnim running_left;
	T3DAnim jump_left;
	T3DAnim falling_left;
	T3DAnim land_left;

} AnimationSet;


typedef struct {
	
	uint8_t previous;
	uint8_t current;

	AnimationSet main;
	AnimationSet blend;

	uint8_t change_delay;
	float blending_ratio;
	float speed_rate;
	bool synced;

} ActorAnimation;


typedef struct {

	uint32_t id;
	rspq_block_t *dl;
	T3DMat4FP *modelMat;
	T3DModel *model;
	Vector3 scale;
	
	char model_path;
	ActorArmature armature;
	ActorAnimation animation;

	RigidBody body;

	float target_yaw;
	float horizontal_target_speed;
	Vector3 target_velocity;

	float horizontal_speed;
	bool grounded;
	float grounding_height;

	bool hasCollided; // Testing a collision boolean

	uint8_t locomotion_state;
	uint8_t previous_state;
	uint8_t state;

	ActorSettings settings;
	Actorinput input;

} Actor;


// function prototypes

Actor actor_create(uint32_t id, const char *model_path);

void actor_draw(Actor *actor);
void actor_delete(Actor *actor);


// function implemenations

Actor actor_create(uint32_t id, const char *model_path)
{
    Actor actor = {

        .id = id,
		.model = t3d_model_load(model_path),
        .modelMat = malloc_uncached(sizeof(T3DMat4FP)),

        .scale = {1.0f, 1.0f, 1.0f},
        
		.state = 1,
		.previous_state = 1,
		.locomotion_state = 1,

		.body = {
            .position = {0.0f, 0.0f, 0.0f},
            .velocity = {0.0f, 0.0f, 0.0f},
            .rotation = {0.0f, 0.0f, 0.0f},
        },
        
		.grounding_height = -2000.0f, // magic number
        
		.settings = {
			.idle_acceleration_rate = 9,
			.walk_acceleration_rate = 4,
			.run_acceleration_rate = 6,
			.roll_acceleration_rate = 20,
			.roll_acceleration_grip_rate = 2,
			.jump_acceleration_rate = 50,
			.aerial_control_rate = 2.5,
			.walk_target_speed = 200,
			.run_target_speed = 650,
			.sprint_target_speed = 900,
			.idle_to_roll_target_speed = 300,
			.idle_to_roll_grip_target_speed = 50,
			.walk_to_roll_target_speed = 400,
			.run_to_roll_target_speed = 780,
			.sprint_to_roll_target_speed = 980,
			.jump_target_speed = 600, 
			.jump_timer_max = 0.13
        },
    };

	actor.armature.main = t3d_skeleton_create(actor.model);
	actor.armature.blend = t3d_skeleton_clone(&actor.armature.main, false);

    rspq_block_begin();
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
    t3d_model_draw_skinned(actor.model, &actor.armature.main);
    actor.dl = rspq_block_end();

    t3d_mat4fp_identity(actor.modelMat);

    return actor;
}

void actor_draw(Actor *actor) 
{	
	for (uint8_t i = 0; i < ACTOR_COUNT; i++) {
				
		t3d_mat4fp_from_srt_euler(actor[i].modelMat,
			(float[3]){actor[i].scale.x, actor[i].scale.y, actor[i].scale.z},
			(float[3]){rad(actor[i].body.rotation.x), rad(actor[i].body.rotation.y), rad(actor[i].body.rotation.z)},
			(float[3]){actor[i].body.position.x, actor[i].body.position.y, actor[i].body.position.z}
		);
		t3d_matrix_set(actor[i].modelMat, true);
		rspq_block_run(actor[i].dl);
	};
}

void actor_delete(Actor *actor) 
{
	free_uncached(actor->modelMat);

	t3d_skeleton_destroy(&actor->armature.main);
	t3d_anim_destroy(&actor->animation.main.breathing_idle);
	t3d_anim_destroy(&actor->animation.main.running_left);
	t3d_anim_destroy(&actor->animation.main.falling_left);
	//t3d_anim_destroy(&actor->animation.main.jump_left);
	//t3d_anim_destroy(&actor->animation.main.land_left);
	
	t3d_skeleton_destroy(&actor->armature.blend);
	t3d_anim_destroy(&actor->animation.blend.breathing_idle);
	t3d_anim_destroy(&actor->animation.blend.running_left);
	t3d_anim_destroy(&actor->animation.blend.falling_left);
	//t3d_anim_destroy(&actor->animation.blend.jump_left);
	//t3d_anim_destroy(&actor->animation.blend.land_left);
	
	t3d_model_free(actor->model);
	rspq_block_free(actor->dl);
}


#endif