#ifndef SCENERY_H
#define SCENERY_H


// structures

typedef struct {

  	uint32_t id;
	rspq_block_t *dl;
	T3DMat4FP *modelMat;
	T3DModel *model;
    
	Vector3 scale;
	Vector3 position;
	Vector3 rotation;

} Scenery;

// function prototypes

Scenery scenery_create(uint32_t id, const char *model_path);
void scenery_set(Scenery *scenery);
void scenery_draw(Scenery *scenery);
void scenery_delete(Scenery *scenery);


//function implementations

Scenery scenery_create(uint32_t id, const char *model_path)
{
    Scenery scenery = {
        .id = id,
        .model = t3d_model_load(model_path),
        .modelMat = malloc_uncached(sizeof(T3DMat4FP)), // needed for t3d

        .scale = {1.0f, 1.0f, 1.0f},
        .position = {0.0f, 0.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f},
    };

    rspq_block_begin();
    t3d_model_draw(scenery.model);
    scenery.dl = rspq_block_end();

    t3d_mat4fp_identity(scenery.modelMat);

    return scenery;
}

void scenery_set(Scenery *scenery)
{
    t3d_mat4fp_from_srt_euler(scenery->modelMat,
        (float[3]){scenery->scale.x, scenery->scale.y, scenery->scale.z},
        (float[3]){rad(scenery->rotation.x), rad(scenery->rotation.y), rad(scenery->rotation.z)},
        (float[3]){scenery->position.x, scenery->position.y, scenery->position.z}
    );
}

void scenery_draw(Scenery *scenery)
{
    t3d_matrix_set(scenery->modelMat, true);
    rspq_block_run(scenery->dl);
}

void scenery_delete(Scenery *scenery)
{
    free_uncached(scenery->modelMat);
	t3d_model_free(scenery->model);
}


// T3D MODEL DRAW BATCHING
rspq_block_t *batchDL = NULL;
T3DModel *batchModel = NULL;

Scenery* scenery_createBatch(size_t batchSize, const char *model_path)
{
    // Dynamically allocate memory for the batch
    Scenery* scenery = malloc(batchSize * sizeof(Scenery));
    if (!scenery) {
        return NULL; // Handle allocation failure
    }

    if(batchDL == NULL)
    {
        // Load model once for entire batch
        if(batchModel == NULL) batchModel = t3d_model_load(model_path);

        rspq_block_begin();

        // Prepare the scenery objects
        for (size_t i = 0; i < batchSize; i++)
        {
            scenery[i] = (Scenery) {
                .id = i,
                .dl = NULL, // Don't need for the batch
                .model = NULL, // Don't need for the batch
                .modelMat = malloc_uncached(sizeof(T3DMat4FP)), // Seems each object still needs its own matrix

                .scale = {1.0f, 1.0f, 1.0f},
                .position = {0.0f, 0.0f, 0.0f},
                .rotation = {0.0f, 0.0f, 0.0f},
            };

            t3d_matrix_set(scenery[i].modelMat, true);
            t3d_model_draw(batchModel);
        }

        // Store the batch drawing commands
        batchDL = rspq_block_end();
    }

    return scenery;
}

void scenery_drawBatch(Scenery* scenery, size_t batchSize)
{
    // Run the batch draw call
    rspq_block_run(batchDL);

    // Optionally, iterate over individual objects
    //for (size_t i = 0; i < batchSize; i++) t3d_model_draw(scenery[i].model);
}

void scenery_deleteBatch(Scenery* scenery, size_t batchSize)
{
    for (size_t i = 0; i < batchSize; i++) {
        free_uncached(scenery[i].modelMat); // Free each model matrix
    }

    t3d_model_free(batchModel);
    rspq_block_free(batchDL);
    free(scenery); // Free the batch array itself
}


#endif
