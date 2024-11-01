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

#endif // SOUND_H