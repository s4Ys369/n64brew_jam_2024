#ifndef ACTOR_ANIMATION_H
#define ACTOR_ANIMATION_H


// structures

typedef struct {

	T3DAnim idle_left;
	T3DAnim running_left;
	T3DAnim jump_left;
	T3DAnim falling_left;
	T3DAnim landing_left;

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


// function implemenations

ActorAnimation actorAnimation_create(const Actor* actor)
{
	ActorAnimation animation;
	animation.previous = STAND_IDLE;
	animation.current = STAND_IDLE;
	animation.blending_ratio = 0.0f;
	return animation;
}

void animationSet_init(const Actor* actor, AnimationSet* set)
{
	set->idle_left = t3d_anim_create(actor->model, "idle-breathing-left");
	set->running_left = t3d_anim_create(actor->model, "running-10-left");
	//set->jump_left = t3d_anim_create(actor->model, "jump-left");
	//set->falling_left = t3d_anim_create(actor->model, "falling-idle-left");
	//set->landing_left = t3d_anim_create(actor->model, "landing-left");
}

void actorAnimation_init(const Actor* actor, ActorAnimation* animation)
{
	animationSet_init(actor, &animation->main);
	animationSet_init(actor, &animation->blend);
	// attach main
	t3d_anim_attach(&animation->main.idle_left, &actor->armature.main);

	// attach blend
	t3d_anim_attach(&animation->blend.running_left, &actor->armature.blend);
}

	//t3d_anim_set_speed(&actor_animation.blend.walking_left, actor->armature.blending_ratio + 0.15f);		

void actorAnimation_setStandIdle(Actor* actor, ActorAnimation* animation, const float frame_time, rspq_syncpoint_t* syncpoint)
{
	if (animation->previous == RUNNING || animation->current == RUNNING) {

		animation->blending_ratio = actor->horizontal_speed / actor->settings.run_target_speed;
		if(animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;

		t3d_anim_update(&animation->main.idle_left, frame_time);
		t3d_anim_update(&animation->blend.running_left, frame_time);
		t3d_skeleton_blend(&actor->armature.main, &actor->armature.main, &actor->armature.blend, animation->blending_ratio);
	}
	else t3d_anim_update(&animation->main.idle_left, frame_time);
}

void actorAnimation_setRunning(Actor* actor, ActorAnimation* animation, const float frame_time, rspq_syncpoint_t* syncpoint)
{
	if (animation->previous == STAND_IDLE || animation->current == STAND_IDLE) {

		animation->blending_ratio = actor->horizontal_speed / 320;
		if (animation->blending_ratio > 1.0f) animation->blending_ratio = 1.0f;
		if (animation->current == STAND_IDLE) t3d_anim_set_time(&animation->blend.running_left, 0.0f);

		t3d_anim_update(&animation->main.idle_left, frame_time);

		t3d_anim_set_speed(&animation->blend.running_left, animation->blending_ratio);
		t3d_anim_update(&animation->blend.running_left, frame_time);
		
		t3d_skeleton_blend(&actor->armature.main, &actor->armature.main, &actor->armature.blend, animation->blending_ratio);
	}
	else
		t3d_anim_update(&animation->main.running_left, frame_time);
}

void actorAnimation_setCurrent(Actor* actor, ActorAnimation* animation, const uint8_t state, const float frame_time, rspq_syncpoint_t* syncpoint)
{
	switch(animation->current) {

		case STAND_IDLE: {
			actorAnimation_setStandIdle(actor, animation, frame_time, syncpoint);
			break;
		}
		case RUNNING: {
			actorAnimation_setRunning(actor, animation, frame_time, syncpoint);
			break;
		}
		case JUMP: {
			break;
		}
		case FALLING: {
			break;
		}
		case LANDING: {
			break;
		}
	}
}

void actor_setAnimation(Actor* actor, ActorAnimation* animation, const float frame_time, rspq_syncpoint_t* syncpoint)
{
    switch(actor->state) {

        case STAND_IDLE: {	
			actorAnimation_setStandIdle(actor, animation, frame_time, syncpoint);
			if (animation->current != STAND_IDLE) {
				animation->previous = animation->current;
				animation->current = STAND_IDLE;
			}
            break;
        }

        case RUNNING: {
			actorAnimation_setRunning(actor, animation, frame_time, syncpoint);
			if (animation->current != RUNNING) {
				animation->previous = animation->current;
				animation->current = RUNNING;
			}
            break;
        }

        case JUMP: {
            break;
        }
        case FALLING: {
            break;
        }
		case LANDING: {
			break;
		}
    }
	
	if(syncpoint)rspq_syncpoint_wait(*syncpoint);
	t3d_skeleton_update(&actor->armature.main);
}


#endif