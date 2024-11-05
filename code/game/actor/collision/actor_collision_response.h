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
    if (contact->slope >= 1.0f && contact->slope < 50.0f && contact->ground_distance > 0.1f)
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

    if (contact->slope >= 0 && contact->slope < 50) {
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

void actorCollision_collideWithPlayground(Actor* actor) {
    if (actor->body.position.x > 1870) actor->body.position.x = 1875;
    if (actor->body.position.x < -1870) actor->body.position.x = -1875;
    if (actor->body.position.y > 1870) actor->body.position.y = 1875;
    if (actor->body.position.y < -1870) actor->body.position.y = -1875;
    if (actor->body.position.z < -2000.0f) actor->body.position.z = -2000.0f;
}

// HEXAGON TEST
bool actorCollision_checkCapsuleHexagonGround(Actor* actor, ActorContactData* contact, ActorCollider* collider, const Triangle* hexagon)
{

    // Step 1: Check if either end of the capsule is within any of the hexagon's triangles
    for (int i = 0; i < 6; i++)
    {
        // Check the start point of the capsule
        if (triangle_containsPoint(&hexagon[i], &collider->body.start))
        {
            // Step 2: Check if the player's z position matches the hexagon's z position
            if (fabsf(actor->body.position.z - hexagon[i].vertA.z) < TOLERANCE)
            {
                // Step 3: Set ground response
                actorCollision_setGroundResponse(actor, contact, collider);
                return true; // Capsule is within hexagon and ground response has been set
            }
        }

        // Check the end point of the capsule if length is greater than zero
        if (collider->body.length > 0 && triangle_containsPoint(&hexagon[i], &collider->body.end))
        {
            if (fabsf(actor->body.position.z - hexagon[i].vertA.z) < TOLERANCE)
            {
                actorCollision_setGroundResponse(actor, contact, collider);
                return true;
            }
        }
    }
    
    // Step 4: Check if the distance from the hexagon's edges intersects with the capsule's radius
    for (int i = 0; i < 6; i++)
    {
        if (capsule_intersectsEdge(&collider->body, &hexagon[i].edge.start, &hexagon[i].edge.end))
        {
            if (fabsf(actor->body.position.z - hexagon[i].edge.start.z) < TOLERANCE)
            {
                actorCollision_setGroundResponse(actor, contact, collider);
                return true;
            }
        }
    }

    return false; // Capsule is not within the hexagon or is at a different z level
}

#endif