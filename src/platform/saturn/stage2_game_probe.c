#include "core.h"
#include "game/game.h"
#include "platform/saturn/platform.h"

#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

void AgbMain(void)
{
    EngineInit();
    GameInit();
    PlatformSaturn_SetBootStage("Saturn title: PRESS START");
    EngineMainLoop();
    PlatformSaturn_Halt("stage2 returned");
}
