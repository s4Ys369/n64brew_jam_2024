#ifndef PLATFORM_H
#define PLATFORM_H

// HEXAGON TEST
typedef struct {
  Box* boxes;
  AABB aabb;
} PlatformCollider;


typedef struct {
  Vector3 position;
  PlatformCollider* collider;
  bool despawned;
} Platform;

Platform hexagon;

void platform_init(Platform* platform, T3DModel* model)
{
  // Allocate PlatformCollider
  platform->collider = (PlatformCollider*)malloc(sizeof(PlatformCollider));

  // Allocate space for 3 boxes within the PlatformCollider
  platform->collider->boxes = (Box*)malloc(3 * sizeof(Box));

  // Initialize the three boxes with specified attributes
  platform->collider->boxes[0] = (Box) {
    .size = {150.0f, 258.0f, 25.0f},
    .center = {0.0f, 0.0f, 0.0f},
    .rotation = {180.0f, -180.0f, 90.0f}
  };

  platform->collider->boxes[1] = (Box) {
    .size = {150.0f, 258.0f, 25.0f},
    .center = {0.0f, 0.0f, 0.0f},
    .rotation = {0.0f, 0.0f, 30.0f}
  };

  platform->collider->boxes[2] = (Box) {
    .size = {150.0f, 258.0f, 25.0f},
    .center = {0.0f, 0.0f, 0.0f},
    .rotation = {180.0f, -180.0f, 30.0f}
  };

  // Initialize other Platform fields
  platform->position = (Vector3){0.0f, 0.0f, 0.0f};
  platform->collider->aabb.maxCoordinates = vector3_from_int16(model->aabbMax);
  platform->collider->aabb.minCoordinates = vector3_from_int16(model->aabbMin);
  platform->despawned = false;
}

void platform_free(Platform* platform)
{
  if (platform && platform->collider)
  {
    free(platform->collider->boxes);  // Free the box array
    free(platform->collider);         // Free the collider itself
    platform->collider = NULL;
  }
}

#endif // PLATFORM_H