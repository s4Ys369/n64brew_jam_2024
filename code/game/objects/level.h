#ifndef LEVEL_H
#define LEVEL_H

// @TODO: find a better place for this logic
ShapeFileData shapeData = {0};

Box* box_colliders = NULL;

void level_parse(const char *text_path, Box **colliders, ShapeFileData *shapeData)
{
    // If text file is successfully parsed
    if (parseFile(text_path, shapeData))
    {
        // Allocate dynamic shape memory
        *colliders = (Box *)malloc(sizeof(Box) * shapeData->numShapes);

        // @TODO: Add support for spheres
        for (size_t shapes = 0; shapes < shapeData->numShapes; ++shapes) 
        {

            // Parse position, scale, and rotation
            Vector3 tempPos = vector3_from_array(shapeData->shapes[shapes].info.pos);
            Vector3 tempScale = vector3_from_array(shapeData->shapes[shapes].info.dim);
            Quaternion tempRot = quat_from_array(shapeData->shapes[shapes].info.rot);

            // To match the input for the physics engine, switch to Z up, and convert rotations radians to degrees
            Vector3 pos = vector3_flip_up(tempPos);
            Vector3 scale = vector3_flip_up(tempScale);
			Vector3 rot = vector3_fromQuaternion(tempRot);
			Vector3 colRot = vector3_flip_up(rot);
            colRot.x = deg(colRot.x);
            colRot.y = deg(colRot.y);
            colRot.z = deg(colRot.z);

			// Applying transformed values to box colliders
            box_init(&(*colliders)[shapes], scale, pos, colRot, 1.0f); // If using `--base-scale=n` with T3D, enter n here as the scalar
        }
    }
}

#endif // LEVEL_H