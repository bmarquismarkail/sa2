#include <stdbool.h>
#include <string.h>

#if !SATURN_PROBE_SLIM_PLATFORM
#include "core.h"
#include "global.h"
#include "lib/agb_flash/flash_internal.h"
#include "platform/shared/dma.h"
#include "platform/shared/video/gpsp_renderer.h"
#endif
#include "platform/saturn/platform.h"

// Clear project-level helper macros before including jo/jo.h to avoid name collisions with the Jo Engine headers.
#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

extern void jo_get_inputs_vblank(void);

#define SATURN_RAW_INPUT_SPIN_WAIT 16
#define SATURN_RAW_KEY_Z           (1u << 0)
#define SATURN_RAW_KEY_Y           (1u << 1)
#define SATURN_RAW_KEY_X           (1u << 2)
#define SATURN_RAW_KEY_R           (1u << 3)
#define SATURN_RAW_KEY_UP          (1u << 4)
#define SATURN_RAW_KEY_DOWN        (1u << 5)
#define SATURN_RAW_KEY_LEFT        (1u << 6)
#define SATURN_RAW_KEY_RIGHT       (1u << 7)
#define SATURN_RAW_KEY_B           (1u << 8)
#define SATURN_RAW_KEY_C           (1u << 9)
#define SATURN_RAW_KEY_A           (1u << 10)
#define SATURN_RAW_KEY_START       (1u << 11)
#define SATURN_RAW_KEY_L           (1u << 15)

#if !SATURN_PROBE_SLIM_PLATFORM
static ALIGNED(256) uint16_t sGameImage[DISPLAY_WIDTH * DISPLAY_HEIGHT];
static jo_img sFrameImage = {
    .width = DISPLAY_WIDTH,
    .height = DISPLAY_HEIGHT,
    .data = sGameImage,
};
#endif
static const char *sBootStage = "boot start";
static u32 sPresentedFrames;
static u16 sLastKeyInput;
static u16 sLatchedKeyInput;
static bool sInFatalLoop;
#if SATURN_PROBE_SPRITE_PRESENT
static s32 sFrameSpriteId = -1;
#endif

#if !SATURN_PROBE_SLIM_PLATFORM
static inline void HandleVBlankIntrs(void);
#endif

#if SATURN_PROBE_VISUALS
static void PlatformSaturn_FillDebugImage(u16 color)
{
    for (s32 y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (s32 x = 0; x < DISPLAY_WIDTH; ++x) {
            const bool border = (x < 4) || (x >= (DISPLAY_WIDTH - 4)) || (y < 4) || (y >= (DISPLAY_HEIGHT - 4));
            sGameImage[(y * DISPLAY_WIDTH) + x] = border ? RGB_WHITE : color;
        }
    }
}
#else
static inline void PlatformSaturn_FillDebugImage(u16 color)
{
    (void)color;
}
#endif

static u16 PlatformSaturn_ReadRawInput(const jo_smpc_memory_address port)
{
    u16 temp;

    jo_smpc_write_byte(port, 0x60);
    for (volatile int i = 0; i < SATURN_RAW_INPUT_SPIN_WAIT; ++i) { }
    temp = (jo_smpc_read_byte(port) & 0x08) << 12;

    jo_smpc_write_byte(port, 0x40);
    for (volatile int i = 0; i < SATURN_RAW_INPUT_SPIN_WAIT; ++i) { }
    temp |= (jo_smpc_read_byte(port) & 0x0F) << 8;

    jo_smpc_write_byte(port, 0x20);
    for (volatile int i = 0; i < SATURN_RAW_INPUT_SPIN_WAIT; ++i) { }
    temp |= (jo_smpc_read_byte(port) & 0x0F) << 4;

    jo_smpc_write_byte(port, 0x00);
    for (volatile int i = 0; i < SATURN_RAW_INPUT_SPIN_WAIT; ++i) { }
    temp |= (jo_smpc_read_byte(port) & 0x0F) << 0;

    return (temp ^ 0x8FFF);
}

static void PlatformSaturn_DrawBootOverlay(void)
{
#if SATURN_PROBE_SLIM_PLATFORM
    (void)sBootStage;
    (void)sPresentedFrames;
    (void)sLastKeyInput;
    (void)sInFatalLoop;
#else
    jo_set_printf_color_index(JO_COLOR_INDEX_White);
    jo_printf(0, 0, "SA2 Saturn boot tracer");
    jo_printf(0, 1, "stage: %-28s", sBootStage);
    jo_printf(0, 2, "frames: %-10lu", (unsigned long)sPresentedFrames);
    jo_printf(0, 3, "keys:   0x%03X", sLastKeyInput);
    jo_printf(0, 4, "fatal:  %-10s", sInFatalLoop ? "yes" : "no");
#endif
}

static bool PlatformSaturn_DrawTitleShellFrame(void)
{
#if SATURN_PROBE_SLIM_PLATFORM
    return false;
#else
    u16 bg = RGB_BLUE;
    u16 panel = 0;
    u16 accent = RGB_WHITE;
    s32 bars = 3;

    if ((sBootStage == NULL) || (strncmp(sBootStage, "Saturn title:", 13) != 0)) {
        return false;
    }

    if (strstr(sBootStage, "START selected") != NULL) {
        bg = RGB_WHITE;
        panel = RGB_RED;
        accent = RGB_BLUE;
        bars = 4;
    } else if (strstr(sBootStage, "A placeholder") != NULL) {
        bg = RGB_RED;
        panel = 0;
        accent = RGB_WHITE;
    } else if (strstr(sBootStage, "B placeholder") != NULL) {
        bg = 0;
        panel = RGB_BLUE;
        accent = RGB_WHITE;
    } else if (strstr(sBootStage, "MENU DOWN") != NULL) {
        bg = RGB_RED;
        panel = RGB_WHITE;
        accent = 0;
        bars = 2;
    } else if (strstr(sBootStage, "MENU UP") != NULL) {
        bg = RGB_BLUE;
        panel = RGB_WHITE;
        accent = RGB_RED;
        bars = 2;
    } else if (strstr(sBootStage, "SHELL OK") != NULL) {
        bg = 0;
        panel = RGB_WHITE;
        accent = RGB_BLUE;
    }

    PlatformSaturn_FillDebugImage(bg);

    for (s32 y = 44; y < 116; ++y) {
        for (s32 x = 24; x < 216; ++x) {
            sGameImage[(y * DISPLAY_WIDTH) + x] = panel;
        }
    }

    for (s32 i = 0; i < bars; ++i) {
        const s32 y0 = 56 + (i * 16);
        const s32 y1 = y0 + 8;
        const s32 x0 = 40;
        const s32 x1 = 200 - (i * 16);

        for (s32 y = y0; y < y1; ++y) {
            for (s32 x = x0; x < x1; ++x) {
                sGameImage[(y * DISPLAY_WIDTH) + x] = accent;
            }
        }
    }

    return true;
#endif
}

static u16 ReadSaturnKeys(void)
{
#if SATURN_PROBE_SLIM_PLATFORM
    return 0;
#else
    const u16 raw = PlatformSaturn_ReadRawInput(PortDataRegister1);
    u16 keys = 0;

    if (raw & SATURN_RAW_KEY_A)
        keys |= A_BUTTON;
    if (raw & SATURN_RAW_KEY_B)
        keys |= B_BUTTON;
    if (raw & (SATURN_RAW_KEY_C | SATURN_RAW_KEY_X | SATURN_RAW_KEY_Y | SATURN_RAW_KEY_Z))
        keys |= SELECT_BUTTON;
    if (raw & SATURN_RAW_KEY_START)
        keys |= START_BUTTON;
    if (raw & SATURN_RAW_KEY_RIGHT)
        keys |= DPAD_RIGHT;
    if (raw & SATURN_RAW_KEY_LEFT)
        keys |= DPAD_LEFT;
    if (raw & SATURN_RAW_KEY_UP)
        keys |= DPAD_UP;
    if (raw & SATURN_RAW_KEY_DOWN)
        keys |= DPAD_DOWN;
    if (raw & SATURN_RAW_KEY_L)
        keys |= L_BUTTON;
    if (raw & SATURN_RAW_KEY_R)
        keys |= R_BUTTON;

    return keys;
#endif
}

#if !SATURN_PROBE_SLIM_PLATFORM
static void PlatformSaturn_PresentFrame(void)
{
#if SATURN_PROBE_SPRITE_PRESENT
    if (sFrameSpriteId < 0)
        sFrameSpriteId = jo_sprite_add(&sFrameImage);
    else
        sFrameSpriteId = jo_sprite_replace(&sFrameImage, sFrameSpriteId);
#else
    jo_vdp2_set_nbg1_image(&sFrameImage, 0, 0);
#endif
    PlatformSaturn_DrawBootOverlay();
}
#endif

void PlatformSaturn_RunProbeFrame(void)
{
#if SATURN_PROBE_SLIM_PLATFORM
    ++sPresentedFrames;
    PlatformSaturn_DrawBootOverlay();
#else
    sLastKeyInput = PlatformSaturn_GetKeyInput();
    REG_KEYINPUT = KEYS_MASK ^ sLastKeyInput;
    if (!PlatformSaturn_DrawTitleShellFrame()) {
        gpsp_draw_frame(sGameImage);
    }
    PlatformSaturn_NoteFramePresented();
    PlatformSaturn_PresentFrame();
#endif
}

void PlatformSaturn_DrawPresentedFrame(void)
{
#if SATURN_PROBE_SPRITE_PRESENT
    if (sFrameSpriteId >= 0)
        jo_sprite_draw3D(sFrameSpriteId, 0, 0, 500);
#endif
}

void PlatformSaturn_Init(void)
{
#if SATURN_PROBE_SKIP_INIT
    return;
#endif

#if SATURN_PROBE_SLIM_PLATFORM
    jo_set_printf_color_index(JO_COLOR_INDEX_White);
    return;
#else
    jo_set_printf_color_index(JO_COLOR_INDEX_White);
    REG_RCNT = 0x8000;
    jo_smpc_write_byte(DataDirectionRegister1, 0x60);
    jo_smpc_write_byte(DataDirectionRegister2, 0x60);
    jo_smpc_write_byte(InputOutputSelectRegister, 0x03);
    jo_smpc_write_byte(ExternalLatchEnableRegister, 0x00);
    REG_KEYINPUT = KEYS_MASK;
    sLastKeyInput = 0;
    sLatchedKeyInput = 0;
    memset(sGameImage, 0, sizeof(sGameImage));
    PlatformSaturn_FillDebugImage(RGB_BLUE);
    jo_clear_background(JO_COLOR_Black);
    jo_clear_screen();
    PlatformSaturn_DrawBootOverlay();
#endif
}

void PlatformSaturn_PrepareRuntime(void)
{
#if SATURN_PROBE_SKIP_PREPARE
    return;
#endif

#if SATURN_PROBE_SLIM_PLATFORM
    return;
#else
    PlatformSaturn_FillDebugImage(0);
    PlatformSaturn_PresentFrame();
#endif
}

u16 PlatformSaturn_GetKeyInput(void)
{
    const u16 keys = ReadSaturnKeys();

    sLatchedKeyInput = keys;
    sLastKeyInput = keys;
    return keys;
}

void PlatformSaturn_SetBootStage(const char *stage)
{
    sBootStage = (stage != NULL) ? stage : "(null)";
#if !SATURN_PROBE_SLIM_PLATFORM
    {
        jo_color color = JO_COLOR_Black;

        if (strncmp(sBootStage, "Saturn title:", 13) == 0) {
            color = JO_COLOR_Blue;
            if (strstr(sBootStage, "START selected") != NULL) {
                color = JO_COLOR_White;
            } else if (strstr(sBootStage, "A placeholder") != NULL) {
                color = JO_COLOR_Red;
            } else if (strstr(sBootStage, "B placeholder") != NULL) {
                color = JO_COLOR_Purple;
            }
        } else if (strstr(sBootStage, "stage2 returned") != NULL) {
            color = JO_COLOR_Green;
        }

        jo_vdp2_clear_bitmap_nbg1(color);
    }
#endif
    PlatformSaturn_DrawBootOverlay();
}

void PlatformSaturn_SetDebugCheckpoint(const char *stage, u32 slot)
{
#if SATURN_PROBE_SLIM_PLATFORM
    PlatformSaturn_SetBootStage(stage);
    (void)slot;
#else
    static const jo_color sCheckpointColors[] = {
        JO_COLOR_Yellow,
        JO_COLOR_Blue,
        JO_COLOR_Purple,
        JO_COLOR_Cyan,
        JO_COLOR_White,
        JO_COLOR_DarkYellow,
        JO_COLOR_DarkCyan,
        JO_COLOR_Green,
    };

    PlatformSaturn_SetBootStage(stage);
    jo_vdp2_clear_bitmap_nbg1(sCheckpointColors[slot % (sizeof(sCheckpointColors) / sizeof(sCheckpointColors[0]))]);
    slSynch();
    for (volatile u32 pause = 0; pause < 4000000; ++pause) {
        if ((pause & 0x3FFF) == 0) {
            sLatchedKeyInput = ReadSaturnKeys();
            sLastKeyInput = sLatchedKeyInput;
        }
    }
#endif
}

void PlatformSaturn_NoteFramePresented(void)
{
    ++sPresentedFrames;
}

void PlatformSaturn_Halt(const char *stage)
{
    sInFatalLoop = true;
    PlatformSaturn_SetBootStage(stage);
#if SATURN_PROBE_SLIM_PLATFORM
    for (;;) {
    }
#else
    PlatformSaturn_FillDebugImage(RGB_RED);

    for (;;) {
        sLastKeyInput = PlatformSaturn_GetKeyInput();
        REG_KEYINPUT = KEYS_MASK ^ sLastKeyInput;
        PlatformSaturn_NoteFramePresented();
        PlatformSaturn_PresentFrame();
        HandleVBlankIntrs();
    }
#endif
}

void PlatformSaturn_DebugTrap(const char *stage)
{
    PlatformSaturn_Halt(stage);
}

#if !SATURN_PROBE_SLIM_PLATFORM
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
#endif

void VBlankIntrWait(void)
{
#if SATURN_PROBE_SLIM_PLATFORM
    ++sPresentedFrames;
    return;
#else
    PlatformSaturn_RunProbeFrame();
    HandleVBlankIntrs();
#endif
}

void DoSoftReset(void)
{
    PlatformSaturn_Halt("DoSoftReset");
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
