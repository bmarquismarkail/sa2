#include "core.h"
#include "platform/shared/audio/cgb_audio.h"
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
    cgb_audio_init(48000);
    PlatformSaturn_PrepareRuntime();
    AgbMain();
}
