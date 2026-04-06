#include <stdbool.h>

#include "core.h"
#include "platform/saturn/platform.h"
#include "platform/saturn/payload_loader.h"

#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

extern void AgbMain(void);

static bool sAgbMainStarted;

#ifndef SATURN_STAGE2_GAME_FILE
#define SATURN_STAGE2_GAME_FILE "SA2GAME.BIN"
#endif

#ifndef SATURN_STAGE2_GAME_LOAD_ADDRESS
#define SATURN_STAGE2_GAME_LOAD_ADDRESS 0x00280000u
#endif

#define SATURN_GAME_UNCACHED_ADDR ((uintptr_t)(SATURN_STAGE2_GAME_LOAD_ADDRESS | 0x20000000u))
#define SATURN_GAME_MAX_BYTES     (12u * 1024u * 1024u)

bool8 PlatformSaturn_StartGameBridge(void)
{
    PlatformSaturn_SetBootStage("Saturn game: START");
#if SATURN_STAGE2_SHELL
    static const SaturnPayloadSpec sGamePayload = {
        .fileName = SATURN_STAGE2_GAME_FILE,
        .loadAddress = SATURN_GAME_UNCACHED_ADDR,
        .maxBytes = SATURN_GAME_MAX_BYTES,
        .bootStage = "Saturn game: LOAD",
        .failureStage = "Saturn game: FAIL",
    };

    if (PlatformSaturn_GetShellBootTrapEnabled()) {
        PlatformSaturn_DebugTrap("pre-game trap");
    }

    if (!PlatformSaturn_LoadPayload(&sGamePayload)) {
        return FALSE;
    }

    PlatformSaturn_SetBootStage("Saturn game: JUMP");
    ((void (*)(void))sGamePayload.loadAddress)();
    PlatformSaturn_Halt("game payload returned");
#endif
    return TRUE;
}

static void PlatformSaturn_StartGame(void)
{
    if (sAgbMainStarted)
        return;

    sAgbMainStarted = true;
#if SATURN_BOOT_TRAP
    PlatformSaturn_DebugTrap("pre-AgbMain trap");
#endif
    AgbMain();
    PlatformSaturn_Halt("AgbMain returned");
}

void jo_main(void)
{
#if SATURN_STAGE1_LOADER
    jo_core_init(JO_COLOR_Blue);
#else
    jo_core_init(JO_COLOR_Black);
#endif
    PlatformSaturn_Init();
    PlatformSaturn_PrepareRuntime();
#if SATURN_STAGE2_ONLY
    PlatformSaturn_RunProbeFrame();
    PlatformSaturn_StartGame();
#else
    jo_core_add_callback(PlatformSaturn_StartGame);
    jo_core_run();
#endif
}
