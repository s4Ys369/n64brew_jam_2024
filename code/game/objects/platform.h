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
  bool contact;
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
  platform->contact = false;
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

    // Offset model AABB's center to actual position
    vector3_add(&platform->collider->aabb.maxCoordinates, &platform->position);
    vector3_add(&platform->collider->aabb.minCoordinates, &platform->position);
    
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
    platform->contact = false;
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

// VISUAL COLLISION FEEDBACK
color_t platformColor[NUM_HEXAGONS];

// Quick and dirty unsigned 8-bit integer linear interpolation 
uint8_t u8_lerp(uint8_t start, uint8_t end, float t)
{
  return (uint8_t)(start + t * (end - start));
}

// Use sine wave for to oscillate between white and red
void color_oscillateRed(uint8_t *r, uint8_t *g, uint8_t *b, float time)
{
  float t = (fm_sinf(time) + 1) / 2; // Map sine wave from [-1, 1] to [0, 1]

  uint8_t start = 200;
  uint8_t end = 20;

  *r = start;
	*g = u8_lerp(start, end, t);
	*b = u8_lerp(start, end, t);
}

color_t color_get(float speed)
{
	static uint8_t r, g, b;
	static float time = 0.0f;

	color_oscillateRed(&r, &g, &b, time);
	time += speed;
	return RGBA32(r,g,b,255);
}

bool colors_are_equal(color_t c1, color_t c2)
{
  return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a);
}

void platform_collideCheck(Platform* platform, Actor* actor)
{
  float offset = 15.0f; // Size of the actor's collider

  // If actor is within AABB
  if (actor->body.position.x >= platform->collider->aabb.minCoordinates.x - offset &&
      actor->body.position.x <= platform->collider->aabb.maxCoordinates.x + offset &&
      actor->body.position.y >= platform->collider->aabb.minCoordinates.y - offset &&
      actor->body.position.y <= platform->collider->aabb.maxCoordinates.y + offset &&
      actor->grounded)
  {
    platform->contact = true;
  }
}

void platform_getColor(Platform* platform)
{
  for (int i = 0; i < NUM_HEXAGONS; i++)
	{
				
		if(platform[i].contact)
		{
			platformColor[i] = color_get(0.2f);
		} 
		else if (!colors_are_equal(platformColor[i], ui_color(WHITE)))
		{
			platformColor[i] = ui_color(WHITE);
		}
	}
}

#define blinkTime 3 // Number of seconds to blink

uint32_t blinkFrames = 30 * blinkTime;
uint32_t counter[NUM_HEXAGONS];
void platform_despawn(int counterIdx, Platform* platform)
{
  counter[counterIdx]++;
  if(counter[counterIdx] >= blinkFrames)
  {
    if(platform->contact)
    {
      platform->despawned = true;

      // @TODO: super hacky, not sure how remove collision from list, so just make it really small and far away
      for (int i = 0; i < 3; i++)
      {
        platform->collider->boxes[i].size = (Vector3){0.001f,0.001f,0.001f};
        platform->collider->boxes[i].center.z = -100.0f;
      }
    }
    counter[counterIdx] = 0;
  }

}

#endif // PLATFORM_H