#include "global.h"

// Saturn startup is currently sensitive to where Jo Engine's large .bss blocks land.
// This guard reproduces the placement offset proven by the passing memory probes.
ALIGNED(256) u8 gSaturnBssGuard[(OAM_SIZE + VRAM_SIZE) + sizeof(PLTT)] = { 0 };
