#ifndef TXT_PARSER_H
#define TXT_PARSER_H

// Text file parser written for use with the bullet exporter
// by float4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char shape[10];
    char name[50];
    float pos[3];
    float dim[3];
    float rot[3];
} ShapeInfo;

void parseFile(const char* filename, ShapeInfo* info);

void parseFile(const char* filename, ShapeInfo* info) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "shape:", 6) == 0) {
            sscanf(line + 6, "%s", info->shape);
        } else if (strncmp(line, "name:", 5) == 0) {
            sscanf(line + 5, "%s", info->name);
        } else if (strncmp(line, "pos:", 4) == 0) {
            sscanf(line + 4, "%f %f %f", &info->pos[0], &info->pos[1], &info->pos[2]);
        } else if (strncmp(line, "dim:", 4) == 0) {
            sscanf(line + 4, "%f %f %f", &info->dim[0], &info->dim[1], &info->dim[2]);
        } else if (strncmp(line, "rot:", 4) == 0) {
            sscanf(line + 4, "%f %f %f", &info->rot[0], &info->rot[1], &info->rot[2]);
        }
    }

    fclose(file);
}


#endif // TXT_PARSER_H