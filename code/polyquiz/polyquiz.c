#include <libdragon.h>
#include "../../minigame.h"
#include "../../core.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>

#define MAX_VERTICES 100
#define MAX_FACES 200

const MinigameDef minigame_def = {
    .gamename = "Polyquiz",
    .developername = "Rasky",
    .description = "Can you guess how many faces a polyhedron has?",
    .instructions = "Be the first to guess the right number of faces, or get close!",
};

typedef struct {
    float r, g, b;
} Color;

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    int v1, v2, v3;
    Color color;
} Face;

Color palette[] = {
    {0.894f, 0.102f, 0.110f},  // Rosso brillante
    {0.216f, 0.494f, 0.722f},  // Blu brillante
    {0.302f, 0.686f, 0.290f},  // Verde brillante
    {0.596f, 0.306f, 0.639f},  // Viola
    {1.000f, 0.498f, 0.000f},  // Arancione
    {1.000f, 1.000f, 0.200f},  // Giallo
    {0.651f, 0.337f, 0.157f},  // Marrone
    {0.968f, 0.505f, 0.749f},  // Rosa
};
#define PALETTE_SIZE (sizeof(palette) / sizeof(palette[0]))

Color random_color_from_palette() {
    int index = rand() % PALETTE_SIZE;
    return palette[index];
}

Vertex vertices[MAX_VERTICES];
Face faces[MAX_FACES];
int num_vertices = 0;
int num_faces = 0;
rspq_block_t *poly = NULL;
sprite_t *bkg = NULL;
rdpq_font_t *font = NULL;
#define FONT_TEXT 1

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
float rotationSpeed = 20.0f;

Vertex random_vertex(float range_min, float range_max) {
    Vertex v;
    v.x = range_min + (float)rand() / RAND_MAX * (range_max - range_min);
    v.y = range_min + (float)rand() / RAND_MAX * (range_max - range_min);
    v.z = range_min + (float)rand() / RAND_MAX * (range_max - range_min);
    return v;
}

Vertex cross_product(Vertex v1, Vertex v2) {
    Vertex result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

Vertex subtract(Vertex v1, Vertex v2) {
    Vertex result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

float dot_product(Vertex v1, Vertex v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

int is_convex(Vertex p, Vertex q, Vertex r, Vertex s) {
    Vertex pq = subtract(q, p);
    Vertex pr = subtract(r, p);
    Vertex ps = subtract(s, p);
    Vertex n = cross_product(pq, pr);
    return dot_product(n, ps) >= 0;
}

// Gift Wrapping (Jarvis March) for 3D convex hull
void compute_convex_hull() {
    assertf(num_vertices >= 4, "Too few points to create a polyhedron");

    num_faces = 0;
    
    // Staring point: find the leftmost point
    int start = 0;
    for (int i = 1; i < num_vertices; i++) {
        if (vertices[i].x < vertices[start].x) {
            start = i;
        }
    }

    int p = start;
    do {
        for (int i = 0; i < num_vertices; i++) {
            for (int j = i + 1; j < num_vertices; j++) {
                if (i != p && j != p) {
                    int found = 1;
                    for (int k = 0; k < num_vertices; k++) {
                        if (k != p && k != i && k != j && !is_convex(vertices[p], vertices[i], vertices[j], vertices[k])) {
                            found = 0;
                            break;
                        }
                    }
                    if (found) {
                        faces[num_faces].v1 = p;
                        faces[num_faces].v2 = i;
                        faces[num_faces].v3 = j;
                        faces[num_faces].color = random_color_from_palette();
                        num_faces++;
                    }
                }
            }
        }
        p++;
        if (p == num_vertices) {
            p = 0;
        }
    } while (p != start && num_faces < MAX_FACES);
}

void draw_polyhedron(void)
{
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < num_faces; i++) {
        Vertex v1 = vertices[faces[i].v1];
        Vertex v2 = vertices[faces[i].v2];
        Vertex v3 = vertices[faces[i].v3];

        Vertex normal;
        normal.x = (v2.y - v1.y) * (v3.z - v1.z) - (v2.z - v1.z) * (v3.y - v1.y);
        normal.y = (v2.z - v1.z) * (v3.x - v1.x) - (v2.x - v1.x) * (v3.z - v1.z);
        normal.z = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);

        glNormal3f(normal.x, normal.y, normal.z);
        glColor4f(faces[i].color.r, faces[i].color.g, faces[i].color.b, 0.8f);

        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
}

void generate_random_polyhedron(int num_vertices_input, float range_min, float range_max) {
    num_vertices = num_vertices_input;
    for (int i = 0; i < num_vertices; i++) {
        vertices[i] = random_vertex(range_min, range_max);
    }
    compute_convex_hull();

    if (poly) rspq_block_free(poly);
    rspq_block_begin();
        draw_polyhedron();
    poly = rspq_block_end();
}

void minigame_init()
{
    display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    gl_init();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Colore di sfondo

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_dir[] = { -1.0f, -1.0f, -1.0f, 0.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    float aspect_ratio = (float)display_get_width() / (float)display_get_height();
    float near_plane = 1.0f;
    float far_plane = 50.0f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-near_plane*aspect_ratio, near_plane*aspect_ratio, -near_plane, near_plane, near_plane, far_plane);

    int num_vertices = rand() % 10 + 5;
    generate_random_polyhedron(num_vertices, -1.0f, 1.0f);

    bkg = sprite_load("rom:/polyquiz/plaster2.ci4.sprite");
    font = rdpq_font_load("rom:/polyquiz/abaddon.font64");
    rdpq_text_register_font(FONT_TEXT, font);
}

void minigame_cleanup()
{
    rdpq_text_unregister_font(FONT_TEXT);
    rdpq_font_free(font);
    sprite_free(bkg);
    if (poly) rspq_block_free(poly);
    gl_close();
    display_close();
}

void minigame_fixedloop(float dt)
{
    angleX += rotationSpeed * dt;
    angleY += rotationSpeed * dt;
    angleZ += rotationSpeed * dt;
    if (angleX > 360.0f) angleX -= 360.0f;
    if (angleY > 360.0f) angleY -= 360.0f;
    if (angleZ > 360.0f) angleZ -= 360.0f;
}

void minigame_loop(float dt)
{
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if (btn.start) minigame_end();

    surface_t *disp = display_get();
    rdpq_attach(disp, NULL);

    rdpq_set_mode_copy(false);
    rdpq_sprite_upload(TILE0, bkg, &(rdpq_texparms_t){
        .s.repeats = REPEAT_INFINITE, .t.repeats = REPEAT_INFINITE,
    });
    rdpq_texture_rectangle(TILE0, 0, 0, display_get_width(), display_get_height(), 0, 0);

    gl_context_begin();

    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 3.0,  // Camera pos
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up vector

    glScalef(1.2f, 1.2f, 1.2f);
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    glRotatef(angleZ, 0.0f, 0.0f, 1.0f);

    rspq_block_run(poly);

    gl_context_end();

    rdpq_set_mode_standard();
    rdpq_text_printf(&(rdpq_textparms_t){
        .width = display_get_width(), .align = ALIGN_CENTER,
    }, FONT_TEXT, 0, 50, "Guess the number of faces!");

    rdpq_detach_show();
}
