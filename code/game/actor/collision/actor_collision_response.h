#ifndef ACTOR_COLLISION_RESPONSE_H
#define ACTOR_COLLISION_RESPONSE_H     


void actorCollision_pushTowardsNormal(Actor* actor, ActorContactData* contact)
{
    // Calculate the necessary displacement vector in the direction of the contact normal
    Vector3 displacement_vector = vector3_returnScaled(&contact->data.normal, -contact->data.penetration);

    // Apply the displacement to the actor's position
    vector3_subtract(&actor->body.position, &displacement_vector);
}

// lighter solution to use together with the push towards normal function. gives almost same results, for now i will use the correct algorithm
void actorCollision_projectAcceleration(Actor* actor, ActorContactData* contact)
{
    float t = vector3_returnDotProduct(&actor->body.acceleration, &contact->data.normal);
    vector3_addScaledVector(&actor->body.acceleration, &contact->data.normal, -t);
}

void actorCollision_projectVelocity(Actor* actor, ActorContactData* contact)
{
    float t = vector3_returnDotProduct(&actor->body.velocity, &contact->data.normal);
    vector3_addScaledVector(&actor->body.velocity, &contact->data.normal, -t);
}

void actorCollision_solvePenetration(Actor* actor, ActorContactData* contact, ActorCollider* collider)
{
    // Normalize the actor's velocity vector
    Vector3 velocity_normal = vector3_returnNormalized(&actor->body.velocity);

    // Calculate the intersection of the ray (contact point + velocity normal) with the plane
    float denominator = vector3_returnDotProduct(&velocity_normal, &contact->data.normal);
    float numerator = contact->displacement + collider->body.radius - vector3_returnDotProduct(&contact->data.point, &contact->data.normal);

    float t;
    if (fabsf(denominator) > 0.0001f) t = numerator / denominator;
    else return;

    Vector3 axis_closest_at_contact = contact->data.point;
    vector3_addScaledVector(&axis_closest_at_contact, &velocity_normal, t);

    Vector3 displacement_vector = axis_closest_at_contact;
    vector3_subtract(&displacement_vector, &contact->axis_closest_to_point);

    contact->velocity_penetration = displacement_vector;
    vector3_invert(&contact->velocity_penetration);

    vector3_add(&actor->body.position, &displacement_vector);
}

void actorCollision_collideAndSlide(Actor* actor, ActorContactData* contact)
{
    float t = vector3_returnDotProduct(&contact->velocity_penetration, &contact->data.normal);
    Vector3 projection = contact->velocity_penetration;
    vector3_addScaledVector(&projection, &contact->data.normal, -t);
    
    vector3_add(&actor->body.position, &projection);
}

void actorCollision_setGroundResponse(Actor* actor, ActorContactData* contact, ActorCollider* collider)
{
    actor->grounded = true;
    actor->body.acceleration.z = 0;
    actor->body.velocity.z = 0;
    actor->grounding_height = actor->body.position.z;
    actor->state = actor->previous_state;

    // Lower the ground height slightly when on a slope
    if (contact->slope > 10.0f && contact->slope < 50.0f && contact->ground_distance > 0.1f)
    {
        float slope_offset = 0.1f * contact->slope;
        actor->grounding_height -= slope_offset;
        if(actor->body.velocity.x != 0) actor->body.position.z = actor->grounding_height;
        
    } 
}


void actorCollision_setCeilingResponse(Actor* actor, ActorContactData* contact)
{   
    if (actor->body.velocity.z > 0){
    vector3_scale(&actor->body.velocity, 1 - (contact->angle_of_incidence * 0.01));           // angle of incidence can be up to 90 degrees
    actor->body.velocity = vector3_reflect(&actor->body.velocity, &contact->data.normal);
    actor->body.velocity.z = 0.0f;
    }
    else {
        actor->body.velocity.x = 0.0f;
        actor->body.velocity.y = 0.0f;
    }

    // Possible fix for Sloped Ceilings forcing Player downwards
    if(!(actor->grounded))
    {
        actor->state = FALLING;
        actor->hasCollided = false;
    }
}

void actorCollision_setResponse(Actor* actor, ActorContactData* contact, ActorCollider* collider)
{
    actorContactData_setAngleOfIncidence(contact, &actor->body.velocity);
    actorCollision_solvePenetration(actor, contact, collider);

    if (contact->slope > 0 && contact->slope < 50) {
        actorCollision_setGroundResponse(actor, contact, collider);
        actorCollision_collideAndSlide(actor, contact);
    }
    else if (contact->slope > 95 && actor->grounded == false) {
        actorCollision_collideAndSlide(actor, contact);
        actorCollision_setCeilingResponse(actor, contact);    
    }
    else actorCollision_collideAndSlide(actor, contact);

    actorCollider_setVertical(collider, &actor->body.position);
}


// HEXAGON TEST
void actorCollision_updateBoxes(Actor* actor, ActorContactData* actor_contact, ActorCollider* actor_collider, Box box_collider[], size_t numBoxes)
{
    actorContactData_clear(actor_contact);
	actorCollider_setVertical(actor_collider, &actor->body.position);

	// Check if the actor is neither jumping nor falling
	if (actor->body.position.z != -200.0f // magic number
		&& actor->state != JUMP
		&& actor->state != FALLING
        && actor->hasCollided == false) {
			
		actor->state = FALLING;
		actor->grounded = false;
		actor->grounding_height = -200.0f; // magic number
        
	}

	

	for (size_t i = 0; i < numBoxes; ++i)
    {
		if (actorCollision_contactBox(actor_collider, &box_collider[i]))
        {
			actorCollision_contactBoxSetData(actor_contact, actor_collider, &box_collider[i]);
			actorCollision_setGroundResponse(actor, actor_contact, actor_collider);
        
			actor->hasCollided = true; // Set to true only if collision occurs
			actor->grounded = true;    // Set grounded if we have a collision

			actor->state = STAND_IDLE;
			break; // Exit after the first collision
		} else {
			actor->hasCollided = false;
		}
	}

	// Call setState after processing collision responses
	actor_setState(actor, actor->state);

	if(actor->body.position.z <= -50.0f) actor->body.position = (Vector3){0.0f, 0.0f, 300.0f};
}

#endif