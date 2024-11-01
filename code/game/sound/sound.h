#ifndef SOUND_H
#define SOUND_H

#include <libdragon.h>

// Core Definitions
#define MUSIC_CHANNEL 0
#define SFX_CHANNEL 31

// XM sequences
#define NUM_XM  1 // @TODO: enumerate

xm64player_t xmPlayer;

const char* xmFileNames[NUM_XM] = {
    "rom:/game/sound/ene.xm64",
};

// WAV files
#define NUM_WAV 1 // @TODO: enumerate

// Each WAV must have its own structure
wav64_t soundEffects[NUM_WAV];

const char* wavFileNames[NUM_WAV] = {
    "rom:/game/sound/boing.wav64",
};

/* Function Declarations */
void sound_load(void);
void sound_xmSwitch(int songID, float volume, bool loop);
void sound_xmStop(void);
void sound_xmUpdate(float volume, bool loop);
void sound_wavPlay(int sfxID, bool loop);
void sound_wavClose(int sfxID);
void sound_wavCleanup(void);
void sound_cleanup(void);
void sound_update(void);

/* Function Definitions */

void sound_load(void)
{
	// Open all WAVs at boot
	for(int w = 0; w < NUM_WAV; ++w) wav64_open(&soundEffects[w], wavFileNames[w]);

	// Open and play first XM in the list
    xm64player_open(&xmPlayer, xmFileNames[0]);
    xm64player_set_vol(&xmPlayer, 0.8f);
    xm64player_play(&xmPlayer, MUSIC_CHANNEL);
}

// Stops current XM, opens and plays requested module with set volume and whether to loop
void sound_xmSwitch(int songID, float volume, bool loop)
{
    xm64player_stop(&xmPlayer);
	xm64player_close(&xmPlayer);
	xm64player_open(&xmPlayer, xmFileNames[songID]);
	xm64player_set_loop(&xmPlayer, loop);
	xm64player_set_vol(&xmPlayer, volume);
	xm64player_play(&xmPlayer, MUSIC_CHANNEL);
}

// Stops and closes XM player
void sound_xmStop(void)
{
	xm64player_stop(&xmPlayer);
	xm64player_close(&xmPlayer);
}

// Adjusts volume and looping of current XM module
void sound_xmUpdate(float volume, bool loop)
{
	xm64player_set_loop(&xmPlayer, loop);
	xm64player_set_vol(&xmPlayer, volume);
}

// Plays requested WAV and whether to loop
void sound_wavPlay(int sfxID, bool loop)
{
	wav64_set_loop(&soundEffects[sfxID], loop);
	wav64_play(&soundEffects[sfxID], SFX_CHANNEL);
}

void sound_wavClose(int sfxID)
{
	wav64_close(&soundEffects[sfxID]);
}

void sound_wavCleanup(void)
{
	for(int w = 0; w < NUM_WAV; ++w) wav64_close(&soundEffects[w]);
}

void sound_cleanup(void)
{
	sound_xmStop();
	sound_wavCleanup();
}

void sound_update(void)
{
	mixer_try_play();
}

////// Spatial 3D sound experiment

Vector3 calculate_camera_forward(const Camera* camera) {
    Vector3 direction = {
        camera->target.x - camera->position.x,
        camera->target.y - camera->position.y,
        camera->target.z - camera->position.z
    };
	vector3_normalize(&direction);
    return direction;
}

void sound_spatial( const Vector3 *spawner, 
					const Vector3 *player, 
					const Camera *camera) 
{

    float pan = 0.5f;
    float volume = 1.0f;

    // Calculate distance vector for volume attenuation
    Vector3 diff = vector3_difference(spawner, player);
    float distance = vector3_magnitude(&diff);

    // If close enough to the sound spawner, skip unnecessary spatial calculations
    if(distance > 250.0f)
    {
        // Volume attenuation (inverse-square or linear falloff)
        float max_distance = 8000.0f; // Maximum distance for audible sound
        volume = 1.0f - (distance / max_distance);
        if (volume < 0.1f) volume = 0.1f; // Clamp volume to minimum

        // Calculate the horizontal angle (azimuth) for panning
        Vector3 horizontal_diff = {diff.x, diff.y, 0.0f};
        float horizontal_distance = vector3_magnitude(&horizontal_diff);        

        Vector3 player_forward = calculate_camera_forward(camera);
        float cos_angle = vector3_returnDotProduct(&horizontal_diff, &player_forward) / horizontal_distance;
        float angle = acosf(cos_angle); // Angle in radians

        // Determine if sound is to the left or right
        pan = 0.5f * (1.0f + angle / M_PI); // Normalize to [0, 1] with 0.5 as center
        if (diff.x * player_forward.y - diff.y * player_forward.x > 0) pan = 1.0f - pan; // Flip pan if on the right side

        // Apply a vertical attenuation if sound changes by height
        float vertical_attenuation = fminf(1.0f, fmaxf(0.1f, 1.0f - fabsf(diff.z) / max_distance));   

        // Final volume adjustment with vertical attenuation
        volume *= vertical_attenuation;
    }

    // Set the channel volume and panning
    mixer_ch_set_vol_pan(SFX_CHANNEL, volume, pan);
}

////// Spatial 3D sound experiment

#endif // SOUND_H