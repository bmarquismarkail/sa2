#include <stdint.h>

#include "platform/saturn/payload_loader.h"
#include "platform/saturn/platform.h"

#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

#ifndef SATURN_STAGE2_LOAD_ADDRESS
#define SATURN_STAGE2_LOAD_ADDRESS 0x00220000u
#endif

static void Stage1Fail(const char *stage)
{
    jo_vdp2_clear_bitmap_nbg1(JO_COLOR_Red);
    PlatformSaturn_Halt(stage);
}

static void LoadStage2AndJump(void)
{
#if defined(JO_COMPILE_WITH_FS_SUPPORT)
    static const SaturnPayloadSpec sStage2Payload = {
        .fileName = "SA2STAGE.BIN",
        .loadAddress = (uintptr_t)(SATURN_STAGE2_LOAD_ADDRESS | 0x20000000u),
        .maxBytes = (896u * 1024u),
        .bootStage = "stage2 load",
        .failureStage = "stage2 missing",
    };

    if (!PlatformSaturn_LoadPayload(&sStage2Payload)) {
        Stage1Fail("stage2 bad size");
    }

    PlatformSaturn_SetBootStage("stage2 jump");
    ((void (*)(void))sStage2Payload.loadAddress)();
    PlatformSaturn_Halt("stage2 returned");
#else
    Stage1Fail("fs disabled");
#endif
}

void AgbMain(void)
{
    PlatformSaturn_SetBootStage("stage1 boot");
    LoadStage2AndJump();
}
