#ifndef SHAPEPARSER_H
#define SHAPEPARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SHAPES 20
#define MAX_TAGS 20

typedef struct
{
    char name[50];
    float pos[3];
    float dim[3];
    float rot[3];
} TagInfo;

typedef struct
{
    char shape[10];
    TagInfo info;
} ShapeInfo;

typedef struct
{
    ShapeInfo* shapes;
    size_t numShapes;
    TagInfo* tags;
    size_t numTags;
} ShapeFileData;


// AI do it for me momento
void parseTagInfo(const char* line, TagInfo* info)
{
    const char* offset = line;

    // Parse name
    sscanf(offset + 5, "%s", info->name); // Adjusting to start after "name:"

    // Move offset past name and spaces
    offset += 5 + strlen(info->name) + 1; // 5 for "name:" + length of name + space/tab

    // Parse position
    sscanf(offset, "pos: %f %f %f", &info->pos[0], &info->pos[1], &info->pos[2]);
    offset += strlen(offset) + 1; // Move past position

    // Parse dimensions
    sscanf(offset, "dim: %f %f %f", &info->dim[0], &info->dim[1], &info->dim[2]);
    offset += strlen(offset) + 1; // Move past dimensions

    // Parse rotation
    sscanf(offset, "rot: %f %f %f %f", &info->rot[0], &info->rot[1], &info->rot[2], &info->rot[3]);
}


// Fill the struct one filed at a time?
void parseShape(const char* line, ShapeFileData *shapeFileData, int index) {
    // @TODO: Y and Z are reversed, bullet and t3d uses Y Up, code base using Z Up, not sure if left or right handed.
    sscanf(line, "shape: %*s name: %s pos: %f %f %f dim: %f %f %f rot: %f %f %f",
        shapeFileData->shapes[index].info.name,
        &shapeFileData->shapes[index].info.pos[0],
        &shapeFileData->shapes[index].info.pos[1],
        &shapeFileData->shapes[index].info.pos[2],
        &shapeFileData->shapes[index].info.dim[0],
        &shapeFileData->shapes[index].info.dim[1],
        &shapeFileData->shapes[index].info.dim[2],
        &shapeFileData->shapes[index].info.rot[0],
        &shapeFileData->shapes[index].info.rot[1],
        &shapeFileData->shapes[index].info.rot[2]);
}

bool parseFile(const char* filename, ShapeFileData* info)
{
    if (info == NULL)
    {
        return false;
    }

    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Could not open file %s\n", filename);
        return false;
    }

    int currentShapeIndex = 0;
    int currentTagIndex = 0;

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        // Handle shape count
        if (strncmp(line, "shapeCount:", 11) == 0)
        {
            sscanf(line + 11, " %d", &info->numShapes);
            info->shapes = (ShapeInfo*)malloc_uncached(sizeof(ShapeInfo) * info->numShapes);
        }
        // Handle tag count
        else if (strncmp(line, "tagCount:", 9) == 0)
        {
            sscanf(line + 9, "%d", &info->numTags);
            info->tags = (TagInfo*)malloc_uncached(sizeof(TagInfo) * info->numTags);
        }
        // Handle shape lines
        else if (strncmp(line, "shape:", 6) == 0)
        {
            parseShape(line, info, currentShapeIndex++);
        }
        // Handle tag lines
        else if (strncmp(line, "tag:", 4) == 0)
        {
            parseTagInfo(line + 6, &info->tags[currentTagIndex++]); // Parse tag info
        }
    }

    fclose(file);
    return true;
}

void parsePrint(ShapeFileData *shapeFileData)
{
    // Print all shapes
    for (int i = 0; i < shapeFileData->numShapes; i++) {
        debugf("Shape %d:\n", i + 1);
        debugf("Name: %s\n", shapeFileData->shapes[i].info.name);
        debugf("Position: (%f, %f, %f)\n", 
                shapeFileData->shapes[i].info.pos[0], 
                shapeFileData->shapes[i].info.pos[1], 
                shapeFileData->shapes[i].info.pos[2]);
        debugf("Dimensions: (%f, %f, %f)\n", 
                shapeFileData->shapes[i].info.dim[0], 
                shapeFileData->shapes[i].info.dim[1], 
                shapeFileData->shapes[i].info.dim[2]);
        debugf("Rotation: (%f, %f, %f)\n",
                shapeFileData->shapes[i].info.rot[0], 
                shapeFileData->shapes[i].info.rot[1], 
                shapeFileData->shapes[i].info.rot[2]);
        debugf("\n");
    }

    // Print all tags
    for (int i = 0; i < shapeFileData->numTags; i++) {
        debugf("Tag %d:\n", i + 1);
        debugf("Name: %s\n", shapeFileData->tags[i].name);
        debugf("Position: (%f, %f, %f)\n", 
                shapeFileData->tags[i].pos[0], 
                shapeFileData->tags[i].pos[1], 
                shapeFileData->tags[i].pos[2]);
        debugf("Rotation: (%f, %f, %f)\n",
                shapeFileData->tags[i].rot[0], 
                shapeFileData->tags[i].rot[1], 
                shapeFileData->tags[i].rot[2]);
        debugf("\n");
    }
}

void parseCheck(ShapeFileData* data)
{
    if(parseFile("rom:/game/levels/testLevel.txt", data))
        parsePrint(data);
}

void destroyShapeFileData(ShapeFileData* data)
{
    if (data->shapes)
        free(data->shapes);
    if (data->tags)
        free(data->tags);
}

#endif // SHAPEPARSER_H