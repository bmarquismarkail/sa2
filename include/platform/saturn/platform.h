#ifndef GUARD_SA2_PLATFORM_SATURN_H
#define GUARD_SA2_PLATFORM_SATURN_H

#include "gba/types.h"

void PlatformSaturn_Init(void);
void PlatformSaturn_PrepareRuntime(void);
u16 PlatformSaturn_GetKeyInput(void);
bool8 PlatformSaturn_HasSaveBackend(void);
bool8 PlatformSaturn_StartGameBridge(void);
void PlatformSaturn_SetShellBootTrapEnabled(bool8 enabled);
bool8 PlatformSaturn_GetShellBootTrapEnabled(void);
void PlatformSaturn_SetShellOverlayEnabled(bool8 enabled);
bool8 PlatformSaturn_GetShellOverlayEnabled(void);
void PlatformSaturn_SetShellTheme(u8 theme);
u8 PlatformSaturn_GetShellTheme(void);
void PlatformSaturn_SetShellMenuSoundsEnabled(bool8 enabled);
bool8 PlatformSaturn_GetShellMenuSoundsEnabled(void);
void PlatformSaturn_SetBootStage(const char *stage);
void PlatformSaturn_SetDebugCheckpoint(const char *stage, u32 slot);
void PlatformSaturn_NoteFramePresented(void);
void PlatformSaturn_DebugTrap(const char *stage);
void PlatformSaturn_Halt(const char *stage);
void PlatformSaturn_RunProbeFrame(void);
void PlatformSaturn_DrawPresentedFrame(void);

#endif // GUARD_SA2_PLATFORM_SATURN_H
