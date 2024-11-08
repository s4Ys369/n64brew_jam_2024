#ifndef PLATFORM_H
#define PLATFORM_H

// HEXAGON TEST
#define NUM_HEXAGONS 30

typedef struct {
  Box* boxes;
  AABB aabb;
} PlatformCollider;

typedef struct {
  Vector3 position;
  PlatformCollider* collider;
  bool despawned;
} Platform;

Platform hexagons[NUM_HEXAGONS];

void platform_init(Platform* platform, T3DModel* model);
void platform_init_grid(Platform* platform, T3DModel* model, float z);
void platform_free(Platform* platform);

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

// Generate a hexagonal grid of positions for the platforms
void generate_hexagon_grid(Vector3* positions, float z)
{
  float x_offset = 255.0f;    // Horizontal distance between centers of adjacent columns
  float y_offset = 225.0f;    // Vertical distance between centers of adjacent rows
  float start_x = 0.0f;       // Starting X coordinate for the first row
  float start_y = 0.0f;       // Starting Y coordinate for the first row

  int rows[] = {3, 4, 5, 4, 3};  // Number of hexagons per row
  int hexagon_index = 0;

  for (int row_index = 0; row_index < 5; row_index++)
  {
    int hex_count = rows[row_index];
    float row_start_x = start_x - (hex_count - 1) * x_offset / 2.0f;

    for (int i = 0; i < hex_count; i++)
    {
      positions[hexagon_index].x = row_start_x + i * x_offset;
      positions[hexagon_index].y = start_y + row_index * y_offset;
      positions[hexagon_index].z = z;
      hexagon_index++;
    }
  }
}

// Generate a larger hexagonal grid of positions for the platforms
void generate_hexagon_grid2(Vector3* positions, float z)
{
  float x_offset = 255.0f;    // Horizontal distance between centers of adjacent columns
  float y_offset = 225.0f;    // Vertical distance between centers of adjacent rows
  float start_x = 0.0f;       // Starting X coordinate for the first row
  float start_y = 0.0f;       // Starting Y coordinate for the first row

  // Updated pattern for a larger hexagonal grid
  int rows[] = {3, 4, 5, 6, 5, 4, 3};  // Number of hexagons per row
  int hexagon_index = 0;

  for (int row_index = 0; row_index < 7; row_index++)
  {
    int hex_count = rows[row_index];
    float row_start_x = start_x - (hex_count - 1) * x_offset / 2.0f;

    for (int i = 0; i < hex_count; i++)
    {
      positions[hexagon_index].x = row_start_x + i * x_offset;
      positions[hexagon_index].y = start_y + row_index * y_offset;
      positions[hexagon_index].z = z;
      hexagon_index++;
    }
  }
}

// Initialize each platform in the hexagonal grid
void platform_init_grid(Platform* platforms, T3DModel* model, float z) {
  Vector3 hex_positions[NUM_HEXAGONS];
  generate_hexagon_grid2(hex_positions, z);

  for (int i = 0; i < NUM_HEXAGONS; i++)
  {
    Platform* platform = &platforms[i];
    platform->collider = (PlatformCollider*)malloc(sizeof(PlatformCollider));
    platform->collider->boxes = (Box*)malloc(3 * sizeof(Box));

    // Set platform position based on hexagonal layout
    platform->position = hex_positions[i];
    
    // Initialize model AABB for collision detection
    platform->collider->aabb.maxCoordinates = vector3_from_int16(model->aabbMax);
    platform->collider->aabb.minCoordinates = vector3_from_int16(model->aabbMin);
    
    // Initialize the three boxes for collision within each hexagon
    for (int j = 0; j < 3; j++)
    {
      platform->collider->boxes[j] = (Box) {
        .size = {175.0f, 275.0f, 30.0f},
        .center = platform->position,
        .rotation = { 
          // Set the rotations explicitly
          (j == 0) ? 180.0f : (j == 2) ? 180.0f : 0.0f, // X rotation for boxes[0] and boxes[2]
          (j == 0) ? -180.0f : (j == 2) ? -180.0f : 0.0f, // Y rotation for boxes[0] and boxes[2]
          (j == 0) ? 90.0f : (j == 1) ? 30.0f : 30.0f   // Z rotation for boxes[0], boxes[1], and boxes[2]
        }
      };
    }
    
    platform->despawned = false;
  }
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