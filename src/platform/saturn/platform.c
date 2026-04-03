#include <string.h>

#include "core.h"
#include "global.h"
#include "lib/agb_flash/flash_internal.h"
#include "platform/shared/dma.h"
#include "platform/shared/video/gpsp_renderer.h"
#include "platform/saturn/platform.h"

// Clear project-level helper macros before including jo/jo.h to avoid name collisions with the Jo Engine headers.
#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

static ALIGNED(256) uint16_t sGameImage[DISPLAY_WIDTH * DISPLAY_HEIGHT];
static jo_img sFrameImage = {
    .width = DISPLAY_WIDTH,
    .height = DISPLAY_HEIGHT,
    .data = sGameImage,
};

static u16 ReadSaturnKeys(void)
{
    u16 keys = 0;

    if (!jo_is_pad1_available()) {
        return 0;
    }

    if (jo_is_pad1_key_pressed(JO_KEY_A))
        keys |= A_BUTTON;
    if (jo_is_pad1_key_pressed(JO_KEY_B))
        keys |= B_BUTTON;
    if (jo_is_pad1_key_pressed(JO_KEY_X))
        keys |= SELECT_BUTTON;
    if (jo_is_pad1_key_pressed(JO_KEY_START))
        keys |= START_BUTTON;
    if (jo_is_pad1_key_pressed(JO_KEY_RIGHT))
        keys |= DPAD_RIGHT;
    if (jo_is_pad1_key_pressed(JO_KEY_LEFT))
        keys |= DPAD_LEFT;
    if (jo_is_pad1_key_pressed(JO_KEY_UP))
        keys |= DPAD_UP;
    if (jo_is_pad1_key_pressed(JO_KEY_DOWN))
        keys |= DPAD_DOWN;
    if (jo_is_pad1_key_pressed(JO_KEY_L))
        keys |= L_BUTTON;
    if (jo_is_pad1_key_pressed(JO_KEY_R))
        keys |= R_BUTTON;

    return keys;
}

static void PlatformSaturn_PresentFrame(void)
{
    jo_vdp2_set_nbg1_image(&sFrameImage, 0, 0);
}

void PlatformSaturn_Init(void)
{
    jo_set_printf_color_index(JO_COLOR_INDEX_White);
    REG_RCNT = 0x8000;
    REG_KEYINPUT = KEYS_MASK;
    memset(sGameImage, 0, sizeof(sGameImage));
    jo_clear_background(JO_COLOR_Black);
}

void PlatformSaturn_PrepareRuntime(void)
{
    PlatformSaturn_PresentFrame();
}

u16 PlatformSaturn_GetKeyInput(void) { return ReadSaturnKeys(); }

static inline void HandleVBlankIntrs(void)
{
    REG_VCOUNT = DISPLAY_HEIGHT + 1;
    REG_DISPSTAT |= INTR_FLAG_VBLANK;
    RunDMAs(DMA_VBLANK);
    if ((REG_DISPSTAT & DISPSTAT_VBLANK_INTR) && gIntrTable[INTR_INDEX_VBLANK]) {
        gIntrTable[INTR_INDEX_VBLANK]();
    }
    REG_DISPSTAT &= ~INTR_FLAG_VBLANK;
}

void VBlankIntrWait(void)
{
    REG_KEYINPUT = KEYS_MASK ^ PlatformSaturn_GetKeyInput();
    gpsp_draw_frame(sGameImage);
    PlatformSaturn_PresentFrame();
    HandleVBlankIntrs();
}

void DoSoftReset(void)
{
    // TODO(saturn): Implement DoSoftReset by returning to the loader or invoking a Saturn reset/shutdown path.
    // Safe fallback for now: intentionally no-op until a platform reset routine is available.
}

void Platform_StoreSaveFile(void)
{
    // TODO(saturn): Implement Platform_StoreSaveFile using a persistent Saturn save backend.
    // When this is wired up, make sure write/open failures are surfaced and handled appropriately.
}

void Platform_QueueAudio(const s16 *data, u32 numBytes)
{
    (void)data;
    (void)numBytes;

    // TODO(saturn): Implement Platform_QueueAudio by forwarding PCM data to the Saturn audio API.
    // Safe fallback for now: drop the mixed audio buffer until the Saturn audio backend is connected.
}
