#ifndef GUARD_SA2_PLATFORM_SATURN_H
#define GUARD_SA2_PLATFORM_SATURN_H

#include "gba/types.h"

void PlatformSaturn_Init(void);
void PlatformSaturn_PrepareRuntime(void);
u16 PlatformSaturn_GetKeyInput(void);
void PlatformSaturn_SetBootStage(const char *stage);
void PlatformSaturn_SetDebugCheckpoint(const char *stage, u32 slot);
void PlatformSaturn_NoteFramePresented(void);
void PlatformSaturn_DebugTrap(const char *stage);
void PlatformSaturn_Halt(const char *stage);
void PlatformSaturn_RunProbeFrame(void);
void PlatformSaturn_DrawPresentedFrame(void);

#endif // GUARD_SA2_PLATFORM_SATURN_H
