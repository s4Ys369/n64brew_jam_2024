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
    int color_idx;
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

float angle = 0.0f;
float rotationSpeed = 50.0f;
float axisX = 0.0f, axisY = 1.0f, axisZ = 0.0f; 

void generateRandomAxis() {
    axisX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    axisY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    axisZ = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

    float length = sqrtf(axisX * axisX + axisY * axisY + axisZ * axisZ);
    if (length > 0.0f) {
        axisX /= length;
        axisY /= length;
        axisZ /= length;
    }
}

Vertex random_vertex(float range_min, float range_max) {
    Vertex v;

    float radius = (range_max - range_min)/2;
    float theta = ((float)rand() / RAND_MAX) * 2.0f * 3.1415628f;  // Angolo azimutale [0, 2*PI]
    float phi = acosf(1.0f - 2.0f * ((float)rand() / RAND_MAX));  // Angolo polare [0, PI]

    v.x = radius * sinf(phi) * cosf(theta);
    v.y = radius * sinf(phi) * sinf(theta);
    v.z = radius * cosf(phi);

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

void color_polyhedron(void) {
    int are_faces_adjacent(Face *f1, Face *f2) {
        int shared_vertices = 0;
        if (f1->v1 == f2->v1 || f1->v1 == f2->v2 || f1->v1 == f2->v3) shared_vertices++;
        if (f1->v2 == f2->v1 || f1->v2 == f2->v2 || f1->v2 == f2->v3) shared_vertices++;
        if (f1->v3 == f2->v1 || f1->v3 == f2->v2 || f1->v3 == f2->v3) shared_vertices++;

        return (shared_vertices == 2);
    }    
    
    int is_valid_color(Face *faces, int face_index, int color_index, int num_faces) {
        for (int i = 0; i < num_faces; i++) {
            if (i == face_index) continue;
            if (are_faces_adjacent(&faces[face_index], &faces[i]) && faces[i].color_idx == color_index)
                return false;
        }
        return true;
    }


    for (int i = 0; i < num_faces; i++) {
        int idx = rand() % PALETTE_SIZE;
        for (int c = 0; c < PALETTE_SIZE; c++) {
            if (is_valid_color(faces, i, (idx+c)%PALETTE_SIZE, num_faces)) {
                faces[i].color_idx = (idx+c) % PALETTE_SIZE;
                break;
            }
        }
    }
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
        Color color = palette[faces[i].color_idx];
        glColor4f(color.r, color.g, color.b, 0.8f);

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
    color_polyhedron();

    if (poly) rspq_block_free(poly);
    rspq_block_begin();
        draw_polyhedron();
    poly = rspq_block_end();
}

void minigame_init()
{
    display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
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

    int w = display_get_width(), h = display_get_height();
    float near_plane = 1.0f;
    float far_plane = 50.0f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, near_plane, far_plane);

    int num_vertices = rand() % 10 + 5;
    generate_random_polyhedron(num_vertices, -1.0f, 1.0f);

    bkg = sprite_load("rom:/polyquiz/plaster2.ci4.sprite");
    font = rdpq_font_load("rom:/polyquiz/abaddon.font64");
    rdpq_text_register_font(FONT_TEXT, font);
    rdpq_font_style(font, 0, &(rdpq_fontstyle_t){
        .color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF), .outline_color = RGBA32(0x0, 0x0, 0x0, 0xFF),
    });
    rdpq_font_style(font, 1, &(rdpq_fontstyle_t){
        .color = RGBA32(palette[2].r*255, palette[2].g*255, palette[2].b*255, 0xFF),
        .outline_color = RGBA32(0x0, 0x0, 0x0, 0xFF),
    });
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
    angle += rotationSpeed * dt;
    if (angle > 360.0f) {
        angle -= 360.0f;

        // Cambia l'asse di rotazione ogni volta che l'angolo raggiunge 360 gradi
        generateRandomAxis();
    }
}

struct {
    int guess;
    bool confirmed;
    float confirm_time;
} player[4];

void minigame_loop(float dt)
{
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if (btn.start) {
        debugf("Minigame ended by player (faces=%d)\n", num_faces);
        minigame_end();
    }

    for (int i=0; i<4; i++) {
        if (player[i].confirmed) break;
        joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1+i);
        if (btn.d_up) player[i].guess++;
        if (btn.d_down) player[i].guess--;
        if (btn.d_left) player[i].guess -= 10;
        if (btn.d_right) player[i].guess += 10;
        if (player[i].guess < 0) player[i].guess = 0;
        if (player[i].guess > 30) player[i].guess = 30;
        if (btn.a) player[i].confirmed = true;
    }

    surface_t *disp = display_get();
    rdpq_attach(disp, NULL);

    rdpq_set_mode_copy(false);
    rdpq_sprite_upload(TILE0, bkg, &(rdpq_texparms_t){
        .s.repeats = REPEAT_INFINITE, .t.repeats = REPEAT_INFINITE,
    });
    rdpq_texture_rectangle(TILE0, 0, 0, display_get_width(), display_get_height(), 0, 0);

    gl_context_begin();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 4.0,  // Camera pos
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up vector

    glRotatef(angle, axisX, axisY, axisZ);

    rspq_block_run(poly);

    gl_context_end();

    rdpq_set_mode_standard();
    rdpq_text_printf(&(rdpq_textparms_t){
        .width = display_get_width(), .align = ALIGN_CENTER,
    }, FONT_TEXT, 0, 50, "Guess the number of faces!");

    for (int i=0; i<4; i++) {
        rdpq_textparms_t parms = {
            .style_id = player[i].confirmed ? 1 : 0,
        };
        rdpq_text_printf(&parms, FONT_TEXT, 100+i*100, 460, "%d", player[i].guess);
    }

    rdpq_detach_show();
}
