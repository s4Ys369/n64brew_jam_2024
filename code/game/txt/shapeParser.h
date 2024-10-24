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
    float rot[4];
} TagInfo;

typedef struct
{
    char shape[10];
    TagInfo info;
} ShapeInfo;

typedef struct
{
    ShapeInfo* shapes;
    int numShapes;
    TagInfo* tags;
    int numTags;
} ShapeFileData;

void parseTagInfo(const char* line, TagInfo* info)
{
    // @TODO: This is the part that seems broken and I ran out of time to fix it!
    const char* offset = line;
    sscanf(offset + 5, "%s", info->name);
    offset += strlen(info->name);

    sscanf(offset + 4, "%f %f %f", &info->pos[0], &info->pos[1], &info->pos[2]);


    offset += sscanf(offset + 4, "%f %f %f", &info->dim[0], &info->dim[1], &info->dim[2]);
    offset += sscanf(offset + 4, "%f %f %f %f", &info->rot[0], &info->rot[1], &info->rot[2], &info->rot[3]);
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
        if (strncmp(line, "shapeCount:", 11) == 0)
        {
            sscanf(line + 11, " %d", &info->numShapes);
            info->shapes = (ShapeInfo*)malloc(sizeof(ShapeInfo) * info->numShapes);
            for (size_t i = 0; i < info->numShapes; i++)
            {
                info->shapes[i] = (ShapeInfo){};
            }
        }
        else if (strncmp(line, "tagCount:", 9) == 0)
        {
            sscanf(line + 9, "%d", &info->numTags);
            info->tags = (TagInfo*)malloc(sizeof(TagInfo) * info->numTags);
        }
        else if (strncmp(line, "shape:", 6) == 0)
		{
            char* current = line;
            current += 7;

            ShapeInfo* currentShape = info->shapes + currentShapeIndex;

            sscanf(current, "%s", currentShape->shape);
            current += strlen(currentShape->shape) + 1;

            parseTagInfo(current, &currentShape->info);

            currentShapeIndex++;
        }
        else if (strncmp(line, "tag:", 4) == 0)
        {
            parseTagInfo(line + 6, &info->tags[currentTagIndex]);

            currentTagIndex++;
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
        debugf("Position: (%f, %f, %f)\n", shapeFileData->shapes[i].info.pos[0], shapeFileData->shapes[i].info.pos[1], shapeFileData->shapes[i].info.pos[2]);
        debugf("Dimensions: (%f, %f, %f)\n", shapeFileData->shapes[i].info.dim[0], shapeFileData->shapes[i].info.dim[1], shapeFileData->shapes[i].info.dim[2]);
        debugf("Rotation: (%f, %f, %f)\n", shapeFileData->shapes[i].info.rot[0], shapeFileData->shapes[i].info.rot[1], shapeFileData->shapes[i].info.rot[2]);
        debugf("\n");
    }

    // Print all triggers
    for (int i = 0; i < shapeFileData->numTags; i++) {
        debugf("Tag %d:\n", i + 1);
        debugf("Name: %s\n", shapeFileData->tags[i].name);
        debugf("Position: (%f, %f, %f)\n", shapeFileData->tags[i].pos[0], shapeFileData->tags[i].pos[1], shapeFileData->tags[i].pos[2]);
        debugf("Rotation: (%f, %f, %f)\n", shapeFileData->tags[i].rot[0], shapeFileData->tags[i].rot[1], shapeFileData->tags[i].rot[2]);
        debugf("\n");
    }
}

void parseCheck(ShapeFileData* data)
{
    if(parseFile("rom:/game/levels/levelA.txt", data))
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