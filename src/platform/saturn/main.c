#include <stdbool.h>

#include "core.h"
#include "platform/saturn/platform.h"

#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

extern void AgbMain(void);

static bool sAgbMainStarted;

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
