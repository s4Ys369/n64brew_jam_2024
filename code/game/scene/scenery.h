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
T3DModel *batchModel = NULL;
Scenery* scenery;
rspq_block_t** rspqBlocks;
size_t blockCount;

#define MATRIX_STACK_LIMIT 8  // Maximum number of objects per rspq block due to matrix stack limit

rspq_block_t** scenery_createBatch(size_t batchSize, const char *model_path)
{
    // Calculate the number of rspq blocks needed
    blockCount = (batchSize + MATRIX_STACK_LIMIT - 1) / MATRIX_STACK_LIMIT;
    
    // Allocate memory for the scenery objects and rspq block pointers
    scenery = malloc_uncached(batchSize * sizeof(Scenery));
    rspqBlocks = malloc_uncached(blockCount * sizeof(rspq_block_t*));

    if (!scenery || !rspqBlocks) return NULL;  // Handle allocation failure


    // Load model once for entire batch
    if(batchModel == NULL) batchModel = t3d_model_load(model_path);

    // Initialize the rspq block index and start a new rspq block
    size_t blockIndex = 0;
    rspq_block_begin();

    for (size_t i = 0; i < batchSize; i++)
    {
        scenery[i] = (Scenery) {
            .id = i,
            .modelMat = malloc_uncached(sizeof(T3DMat4FP)),
            .model = NULL,
            .dl = NULL,
            .scale = {1.0f, 1.0f, 1.0f},
            .position = {0.0f, 0.0f, 0.0f},
            .rotation = {0.0f, 0.0f, 0.0f},
        };

        // Set the model matrix and draw
        t3d_matrix_push_pos(1);
        t3d_matrix_set(scenery[i].modelMat, true);
        t3d_model_draw(batchModel);
        t3d_matrix_pop(1);

        // End the current rspq block and start a new one every 8 objects
        if ((i + 1) % MATRIX_STACK_LIMIT == 0 || i == batchSize - 1)
        {
            rspqBlocks[blockIndex++] = rspq_block_end();  // Store the completed rspq block
            if (i < batchSize - 1) rspq_block_begin();  // Start a new rspq block if more objects remain
        }
    }

    return rspqBlocks;  // Return the list of rspq blocks
}

// Iterate through and run RSPQ blocks
void scenery_drawBatch(rspq_block_t** rspqBlocks, size_t blockCount)
{
    for (size_t i = 0; i < blockCount; i++) rspq_block_run(rspqBlocks[i]);
}

void scenery_deleteBatch(Scenery* scenery, size_t batchSize, rspq_block_t** rspqBlocks, size_t blockCount)
{
    for (size_t i = 0; i < batchSize; i++)
        if (scenery[i].modelMat) free_uncached(scenery[i].modelMat); // Free the object's matrices

    t3d_model_free(batchModel); // Free the batch's model

    for (size_t i = 0; i < blockCount; i++) rspq_block_free(rspqBlocks[i]); // Free the rspq blocks

    free_uncached(scenery);       // Free the scenery array
    free_uncached(rspqBlocks);    // Free the rspq blocks array
}

#endif
