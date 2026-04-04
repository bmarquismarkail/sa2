#include "global.h"
#include "lib/m4a/m4a.h"

#define SATURN_NOAUDIO_SONG_COUNT 512

struct SoundMixerState gSoundInfo = { 0 };

struct MP2KPlayerState gMPlayInfo_BGM = { 0 };
struct MP2KPlayerState gMPlayInfo_SE1 = { 0 };
struct MP2KPlayerState gMPlayInfo_SE2 = { 0 };
struct MP2KPlayerState gMPlayInfo_SE3 = { 0 };

const struct Song gSongTable[SATURN_NOAUDIO_SONG_COUNT] = { 0 };

const struct MusicPlayer gMPlayTable[] = {
    { &gMPlayInfo_BGM, NULL, 0, 0 },
    { &gMPlayInfo_SE1, NULL, 0, 0 },
    { &gMPlayInfo_SE2, NULL, 0, 0 },
    { &gMPlayInfo_SE3, NULL, 0, 0 },
};

struct MP2KSongHeader se_ring_copy = { 0 };

void m4aSoundVSync(void) { }

void m4aSoundInit(void) { }

void m4aSoundMain(void) { }

void m4aSoundMode(u32 mode) { (void)mode; }

void m4aSoundVSyncOn(void) { }

void m4aSoundVSyncOff(void) { }

void m4aSongNumStart(u16 n) { (void)n; }

void m4aSongNumStartOrChange(u16 n) { (void)n; }

void m4aSongNumStartOrContinue(u16 n) { (void)n; }

void m4aSongNumStop(u16 n) { (void)n; }

void m4aSongNumContinue(u16 n) { (void)n; }

void m4aMPlayAllStop(void) { }

void m4aMPlayAllContinue(void) { }

void m4aMPlayContinue(struct MP2KPlayerState *mplayInfo) { (void)mplayInfo; }

void m4aMPlayFadeOut(struct MP2KPlayerState *mplayInfo, u16 speed)
{
    (void)mplayInfo;
    (void)speed;
}

void m4aMPlayFadeOutTemporarily(struct MP2KPlayerState *mplayInfo, u16 speed)
{
    (void)mplayInfo;
    (void)speed;
}

void m4aMPlayFadeIn(struct MP2KPlayerState *mplayInfo, u16 speed)
{
    (void)mplayInfo;
    (void)speed;
}

void m4aMPlayImmInit(struct MP2KPlayerState *mplayInfo) { (void)mplayInfo; }

void m4aMPlayTempoControl(struct MP2KPlayerState *mplayInfo, u16 tempo)
{
    (void)mplayInfo;
    (void)tempo;
}

void m4aMPlayVolumeControl(struct MP2KPlayerState *mplayInfo, u16 trackBits, u16 volume)
{
    (void)mplayInfo;
    (void)trackBits;
    (void)volume;
}

void m4aMPlayPitchControl(struct MP2KPlayerState *mplayInfo, u16 trackBits, s16 pitch)
{
    (void)mplayInfo;
    (void)trackBits;
    (void)pitch;
}

void m4aMPlayPanpotControl(struct MP2KPlayerState *mplayInfo, u16 trackBits, s8 pan)
{
    (void)mplayInfo;
    (void)trackBits;
    (void)pan;
}

void m4aMPlayModDepthSet(struct MP2KPlayerState *mplayInfo, u16 trackBits, u8 modDepth)
{
    (void)mplayInfo;
    (void)trackBits;
    (void)modDepth;
}

void m4aMPlayLFOSpeedSet(struct MP2KPlayerState *mplayInfo, u16 trackBits, u8 lfoSpeed)
{
    (void)mplayInfo;
    (void)trackBits;
    (void)lfoSpeed;
}

void MPlayStart(struct MP2KPlayerState *mplayInfo, struct MP2KSongHeader *songHeader)
{
    (void)mplayInfo;
    (void)songHeader;
}

void MPlayStop(struct MP2KPlayerState *mplayInfo) { (void)mplayInfo; }

void MPlayContinue(struct MP2KPlayerState *mplayInfo) { (void)mplayInfo; }

void MPlayFadeOut(struct MP2KPlayerState *mplayInfo, u16 speed)
{
    (void)mplayInfo;
    (void)speed;
}
