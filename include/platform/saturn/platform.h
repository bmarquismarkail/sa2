#ifndef GUARD_SA2_PLATFORM_SATURN_H
#define GUARD_SA2_PLATFORM_SATURN_H

#include "gba/types.h"

void PlatformSaturn_Init(void);
void PlatformSaturn_PrepareRuntime(void);
u16 PlatformSaturn_GetKeyInput(void);

#endif // GUARD_SA2_PLATFORM_SATURN_H
