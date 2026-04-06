#ifndef GUARD_SA2_PLATFORM_SATURN_PAYLOAD_LOADER_H
#define GUARD_SA2_PLATFORM_SATURN_PAYLOAD_LOADER_H

#include <stdint.h>

#include "gba/types.h"

typedef struct SaturnPayloadSpec {
    const char *fileName;
    uintptr_t loadAddress;
    u32 maxBytes;
    const char *bootStage;
    const char *failureStage;
} SaturnPayloadSpec;

bool8 PlatformSaturn_LoadPayload(const SaturnPayloadSpec *spec);

#endif // GUARD_SA2_PLATFORM_SATURN_PAYLOAD_LOADER_H
