#include "platform/saturn/platform.h"

#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

#ifndef SATURN_STAGE1_LOADER
#define SATURN_STAGE1_LOADER 0
#endif

static const char *sProbeStage = "entry probe init";
static u32 sProbeFrames;

static void EntryProbeDraw(void)
{
    jo_set_printf_color_index(JO_COLOR_INDEX_White);
    jo_printf(0, 0, "SA2 Saturn entry probe");
    jo_printf(0, 1, "stage: %-24s", sProbeStage);
    jo_printf(0, 2, "frames: %-10lu", (unsigned long)sProbeFrames++);
}

void PlatformSaturn_Init(void)
{
    sProbeStage = "PlatformSaturn_Init";
}

void PlatformSaturn_PrepareRuntime(void)
{
    sProbeStage = "PlatformSaturn_PrepareRuntime";
}

u16 PlatformSaturn_GetKeyInput(void)
{
    return 0;
}

void PlatformSaturn_SetBootStage(const char *stage)
{
    sProbeStage = (stage != NULL) ? stage : "(null)";
}

void PlatformSaturn_NoteFramePresented(void) {}

void PlatformSaturn_DebugTrap(const char *stage)
{
    PlatformSaturn_SetBootStage(stage);
    jo_core_add_callback(EntryProbeDraw);
    jo_core_run();
}

void PlatformSaturn_Halt(const char *stage)
{
    PlatformSaturn_SetBootStage(stage);
    jo_core_add_callback(EntryProbeDraw);
    jo_core_run();
}

#if !SATURN_STAGE1_LOADER
void AgbMain(void)
{
    PlatformSaturn_SetBootStage("entry probe AgbMain");
    jo_core_add_callback(EntryProbeDraw);
    jo_core_run();
}
#endif
