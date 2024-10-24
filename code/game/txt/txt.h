#ifndef TXT_H
#define TXT_H

// Text file parser written for use with the bullet exporter

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SHAPES 20
#define MAX_TRIGGERS 20

typedef struct {
    char shape[10];
    char name[50];
    float pos[3];
    float dim[3];
    float rot[3];
} ShapeInfo;

typedef struct {
    char trigger[10];
    char name[50];
    float pos[3];
    float rot[3];
} TriggerInfo;

typedef struct {
    ShapeInfo shapes[MAX_SHAPES];
    TriggerInfo triggers[MAX_TRIGGERS];
    int shapeCount;
    int triggerCount;
} LevelData;

void txt_parseShapes(const char* filename, ShapeInfo* info, int* shapeCount);
void txt_parseTriggers(const char* filename, TriggerInfo* info, int* triggerCount);

void txt_parseShapes(const char* filename, ShapeInfo* shapes, int* shapeCount) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        debugf("Could not open file %s\n", filename);
        return;
    }
    debugf("File %s opened successfully\n", filename);

    char line[256];
    static int index = 0;
    while (fgets(line, sizeof(line), file)) {
        debugf("Reading line: %s", line);  // Log each line
        if (strncmp(line, "tag:", 4) != 0) {
            if (strncmp(line, "shape:", 6) == 0) {
                sscanf(line + 6, "%29s", shapes[index].shape);
                debugf("Parsed shape: %s\n", shapes[index].shape);  // Log parsed shape
            } else if (strncmp(line, "name:", 5) == 0) {
                sscanf(line + 5, "%49s", shapes[index].name);
            } else if (strncmp(line, "pos:", 4) == 0) {
                sscanf(line + 4, "%f %f %f", &shapes[index].pos[0], &shapes[index].pos[1], &shapes[index].pos[2]);
            } else if (strncmp(line, "dim:", 4) == 0) {
                sscanf(line + 4, "%f %f %f", &shapes[index].dim[0], &shapes[index].dim[1], &shapes[index].dim[2]);
            } else if (strncmp(line, "rot:", 4) == 0) {
                sscanf(line + 4, "%f %f %f", &shapes[index].rot[0], &shapes[index].rot[1], &shapes[index].rot[2]);
                index++;  // Only increment after a full shape is parsed
            }
        }
    }
    debugf("Total shapes parsed: %d\n", index);

    *shapeCount = index;
    fclose(file);
}

void txt_parseTriggers(const char* filename, TriggerInfo* triggers, int* triggerCount) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        debugf("Could not open file %s\n", filename);
        return;
    }
    debugf("File %s opened successfully\n", filename);

    char line[256];
    static int index = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "shape:", 6) != 0) {
            if (strncmp(line, "tag:", 4) == 0) {
                sscanf(line + 4, "%9s", triggers[index].trigger);
            } else if (strncmp(line, "name:", 5) == 0) {
                sscanf(line + 5, "%49s", triggers[index].name);
            } else if (strncmp(line, "pos:", 4) == 0) {
                sscanf(line + 4, "%f %f %f", &triggers[index].pos[0], &triggers[index].pos[1], &triggers[index].pos[2]);
            } else if (strncmp(line, "rot:", 4) == 0) {
                sscanf(line + 4, "%f %f %f", &triggers[index].rot[0], &triggers[index].rot[1], &triggers[index].rot[2]);
                index++;  // Move to the next trigger after completing the current one
            }
        }
    }
    debugf("Total triggers parsed: %d\n", index);

    *triggerCount = index;  // Update the count of triggers parsed
    fclose(file);
}



#endif // TXT_H