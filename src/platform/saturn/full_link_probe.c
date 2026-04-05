#include <stdint.h>

#include "platform/saturn/platform.h"

#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

#ifndef SATURN_STAGE2_FILE
#define SATURN_STAGE2_FILE "SA2STAGE.BIN"
#endif

#ifndef SATURN_STAGE2_LOAD_ADDRESS
#define SATURN_STAGE2_LOAD_ADDRESS 0x00220000u
#endif

#define SATURN_STAGE2_CACHED_ADDR   ((uintptr_t)SATURN_STAGE2_LOAD_ADDRESS)
#define SATURN_STAGE2_UNCACHED_ADDR ((uintptr_t)(SATURN_STAGE2_CACHED_ADDR | 0x20000000u))
#define SATURN_STAGE2_MAX_BYTES     (896 * 1024)

static void Stage1Fail(const char *stage)
{
    jo_vdp2_clear_bitmap_nbg1(JO_COLOR_Red);
    PlatformSaturn_Halt(stage);
}

static void LoadStage2AndJump(void)
{
#if defined(JO_COMPILE_WITH_FS_SUPPORT)
    int loadedBytes = 0;

    PlatformSaturn_SetBootStage("stage2 load");
    jo_vdp2_clear_bitmap_nbg1(JO_COLOR_Yellow);
    char *loaded = jo_fs_read_file_ptr(SATURN_STAGE2_FILE, (void *)SATURN_STAGE2_UNCACHED_ADDR, &loadedBytes);

    if (loaded == NULL) {
        Stage1Fail("stage2 missing");
    }

    if ((loadedBytes <= 0) || (loadedBytes > SATURN_STAGE2_MAX_BYTES)) {
        Stage1Fail("stage2 bad size");
    }

    PlatformSaturn_SetBootStage("stage2 jump");
    ((void (*)(void))SATURN_STAGE2_UNCACHED_ADDR)();
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
