#include "core.h"
#include "platform/saturn/platform.h"

#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

extern void AgbMain(void);

void jo_main(void)
{
    jo_core_init(JO_COLOR_Black);
    PlatformSaturn_Init();
    PlatformSaturn_PrepareRuntime();
    AgbMain();
}
