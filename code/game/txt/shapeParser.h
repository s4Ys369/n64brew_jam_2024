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

    sscanf(offset + 4, "%f %f %f", info->pos[0], info->pos[1], info->pos[2]);


    offset += sscanf(offset + 4, "%f %f %f", info->dim[0], info->dim[1], &info->dim[2]);
    offset += sscanf(offset + 4, "%f %f %f %f", info->rot[0], info->rot[1], info->rot[2], info->rot[3]);
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
                info->shapes[i] = {};
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

void destroyShapeFileData(ShapeFileData* data)
{
    if (data->shapes)
        free(data->shapes);
    if (data->tags)
        free(data->tags);
}