#include "platform/saturn/payload_loader.h"
#include "platform/saturn/platform.h"

#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

bool8 PlatformSaturn_LoadPayload(const SaturnPayloadSpec *spec)
{
#if defined(JO_COMPILE_WITH_FS_SUPPORT)
    int loadedBytes = 0;
    char *loaded;

    if (spec == NULL) {
        return FALSE;
    }

    if (spec->bootStage != NULL) {
        PlatformSaturn_SetBootStage(spec->bootStage);
    }

    loaded = jo_fs_read_file_ptr(spec->fileName, (void *)spec->loadAddress, &loadedBytes);
    if (loaded == NULL) {
        if (spec->failureStage != NULL) {
            PlatformSaturn_SetBootStage(spec->failureStage);
        }
        return FALSE;
    }

    if ((loadedBytes <= 0) || ((u32)loadedBytes > spec->maxBytes)) {
        if (spec->failureStage != NULL) {
            PlatformSaturn_SetBootStage(spec->failureStage);
        }
        return FALSE;
    }

    return TRUE;
#else
    (void)spec;
    return FALSE;
#endif
}
