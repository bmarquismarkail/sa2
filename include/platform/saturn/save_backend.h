#ifndef GUARD_SA2_PLATFORM_SATURN_SAVE_BACKEND_H
#define GUARD_SA2_PLATFORM_SATURN_SAVE_BACKEND_H

#include <stddef.h>

#include "gba/types.h"

#define PLATFORM_SATURN_SAVE_DEVICE_NONE (-1)

typedef struct PlatformSaturnSaveDeviceState {
    bool8 mounted;
    bool8 has_save_file;
} PlatformSaturnSaveDeviceState;

int PlatformSaturn_SelectSaveDevice(const PlatformSaturnSaveDeviceState *devices, size_t deviceCount);
void PlatformSaturn_NormalizeSaveImage(unsigned char *dst, size_t dstSize, const unsigned char *src, size_t srcSize);

void PlatformSaturn_InitSaveBackend(void);
bool8 PlatformSaturn_HasSaveBackend(void);
bool8 PlatformSaturn_LoadSaveBackend(void);
bool8 PlatformSaturn_StoreSaveBackend(void);
const char *PlatformSaturn_GetSaveBackendName(void);
bool8 PlatformSaturn_SaveBackendHasExistingSave(void);

#endif // GUARD_SA2_PLATFORM_SATURN_SAVE_BACKEND_H
