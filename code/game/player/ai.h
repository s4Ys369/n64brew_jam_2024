#ifndef AI_H
#define AI_H


void ai_generateControlData(ControllerData *control, Actor *actor, Platform* platforms, size_t platform_count, float camera_angle);

// Helper function to rotate input by camera angle
void ai_updateCam(ControllerData *control, float camera_angle)
{
    // Convert angle to radians
    float angle_rad = camera_angle * (T3D_PI / 180.0f);
    int8_t original_x = control->input.stick_x;
    int8_t original_y = control->input.stick_y;

    // Rotate stick_x and stick_y based on camera angle
    control->input.stick_x = (int8_t)(original_x * fm_cosf(angle_rad) - original_y * fm_sinf(angle_rad));
    control->input.stick_y = (int8_t)(original_x * fm_sinf(angle_rad) + original_y * fm_cosf(angle_rad));
}

// Function to find the nearest platform at a safe height (position.z > 0)
Platform* find_nearest_safe_platform(Actor* actor, Platform* platforms, size_t platform_count) {
    Platform* nearest_platform = NULL;
    float min_distance = FLT_MAX; // Large initial value

    for (size_t i = 0; i < platform_count; ++i)
    {
        Platform* platform = &platforms[i];

        // Skip platforms not at a safe height
        if (platform->position.z <= 0) continue;

        // Calculate the vector difference to the platform
        Vector3 diff = vector3_difference(&platform->position, &actor->body.position);

        // Calculate the inverse distance
        float inverse_distance = vector3_magnitude(&diff);
        if (inverse_distance <= 0) continue; // Avoid division by zero

        // Convert inverse distance to actual distance
        float distance = 1 / inverse_distance;

        // Check if this platform is the nearest valid one
        if (distance < min_distance)
        {
            min_distance = distance;
            nearest_platform = platform;
        }
    }

    return nearest_platform;
}

// AI controller data generation function
void ai_generateControlData(ControllerData *control, Actor *actor, Platform* platforms, size_t platform_count, float camera_angle)
{
    // Initialize control data to zero for each frame
    memset(control, 0, sizeof(ControllerData));

    // Find the nearest safe platform
    Platform* target_platform = find_nearest_safe_platform(actor, platforms, platform_count);
    if (target_platform == NULL) return; // No valid platform found, do nothing

    // Calculate direction towards the target platform
    Vector3 direction_to_target = {
        target_platform->position.x - actor->body.position.x,
        target_platform->position.y - actor->body.position.y,
        0 // Only move in x-y plane, no need for z movement
    };
    vector3_normalize(&direction_to_target);

    // Move towards the target platform
    control->input.stick_x = (int8_t)(direction_to_target.x * 127); // Max joystick range
    control->input.stick_y = (int8_t)(direction_to_target.y * 127);

    // Optional: Add slight randomness to mimic human input
    control->input.stick_x += rand() % 5 - 2;
    control->input.stick_y += rand() % 5 - 2;

    // Calculate horizontal speed as the magnitude of the x and y velocity components
    float horizontal_speed = sqrtf(actor->body.velocity.x * actor->body.velocity.x + actor->body.velocity.y * actor->body.velocity.y);

    // Check if the actor should jump when horizontal speed is greater than 1.0
    if (horizontal_speed > 1.0f && actor->state != JUMP && actor->state != FALLING) control->input.btn.a = 1;


    // Adjust for camera angle if needed
    ai_updateCam(control, camera_angle);
}


#endif // AI_H