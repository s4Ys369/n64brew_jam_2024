#ifndef ACTORSTATES_H
#define ACTORSTATES_H


#define STAND_IDLE 1
#define RUNNING 3
#define JUMP 6
#define FALLING 7
#define LANDING 8


// function prototypes

void actorState_setIdle (Actor *actor);
            
void actorState_setRunning (Actor *actor);

void actorState_setJump (Actor *actor);

void actorState_setFalling (Actor *actor);

void actor_setState (Actor *actor, uint8_t state);


void actorState_setIdle(Actor *actor)
{
    if (actor->state == STAND_IDLE) return;
    actor->previous_state = actor->state;
    actor->state = STAND_IDLE;
    actor->locomotion_state = STAND_IDLE;
    
}

void actorState_setRunning(Actor *actor)
{
    if (actor->state == RUNNING) return;
    actor->previous_state = actor->state;
    actor->state = RUNNING;
    actor->locomotion_state = RUNNING;
}

void actorState_setJump(Actor *actor) 
{
    if (actor->state == JUMP) return;
    
    actor->previous_state = actor->state;
    actor->state = JUMP;
    actor->grounded = 0;
    //actor->grounding_height = -FLT_MAX;
}

void actorState_setFalling (Actor *actor)
{
    if (actor->state == FALLING ) return;

    actor->previous_state = actor->state;
    actor->state = FALLING;
    //actor->grounding_height = -FLT_MAX;
}

void actor_setState(Actor *actor, uint8_t state) 
{
    switch(state) {
        
        case STAND_IDLE: {
            actorState_setIdle (actor);
            break;
        }
        case RUNNING: {
            actorState_setRunning (actor);
            break;
        }
        case JUMP: {
            actorState_setJump (actor);
            break;
        }
        case FALLING: {
            actorState_setFalling (actor);
            break;
        }   
    }
}

#endif