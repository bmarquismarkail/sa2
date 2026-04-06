#include <string.h>

#include "global.h"
#include "lib/agb_flash/flash_internal.h"
#include "platform/saturn/save_backend.h"

#undef TRUE
#undef FALSE
#undef MIN
#undef MAX
#undef ABS

#include <jo/jo.h>

#define SATURN_SAVE_FILE_NAME    "SA2SAVE.BIN"
#define SATURN_SAVE_FILE_COMMENT "SA2 SAVE"

typedef struct PlatformSaturnMountedSaveDevice {
    jo_backup_device device;
    bool8 mounted;
    bool8 hasSaveFile;
} PlatformSaturnMountedSaveDevice;

static PlatformSaturnMountedSaveDevice sSaveDevices[] = {
    { JoInternalMemoryBackup, FALSE, FALSE },
    { JoCartridgeMemoryBackup, FALSE, FALSE },
    { JoExternalDeviceBackup, FALSE, FALSE },
};
static s8 sActiveSaveDevice = PLATFORM_SATURN_SAVE_DEVICE_NONE;
static const char *const sSaveDeviceNames[] = {
    "internal",
    "cartridge",
    "external",
};

static void PlatformSaturn_RefreshSaveDeviceState(void)
{
    size_t i;

    for (i = 0; i < ARRAY_COUNT(sSaveDevices); ++i) {
        PlatformSaturnMountedSaveDevice *device = &sSaveDevices[i];

        device->mounted = jo_backup_mount(device->device) ? TRUE : FALSE;
        device->hasSaveFile = (device->mounted && jo_backup_file_exists(device->device, SATURN_SAVE_FILE_NAME)) ? TRUE : FALSE;
    }
}

void PlatformSaturn_InitSaveBackend(void)
{
    PlatformSaturnSaveDeviceState states[ARRAY_COUNT(sSaveDevices)];
    size_t i;

    PlatformSaturn_RefreshSaveDeviceState();

    for (i = 0; i < ARRAY_COUNT(sSaveDevices); ++i) {
        states[i].mounted = sSaveDevices[i].mounted;
        states[i].has_save_file = sSaveDevices[i].hasSaveFile;
    }

    sActiveSaveDevice = (s8)PlatformSaturn_SelectSaveDevice(states, ARRAY_COUNT(states));
}

bool8 PlatformSaturn_HasSaveBackend(void)
{
    return (sActiveSaveDevice != PLATFORM_SATURN_SAVE_DEVICE_NONE) ? TRUE : FALSE;
}

bool8 PlatformSaturn_LoadSaveBackend(void)
{
    unsigned int length = 0;
    unsigned char *loaded;

    PlatformSaturn_NormalizeSaveImage(FLASH_BASE, sizeof(FLASH_BASE), NULL, 0);

    if (!PlatformSaturn_HasSaveBackend() || !sSaveDevices[sActiveSaveDevice].hasSaveFile) {
        return FALSE;
    }

    loaded = jo_backup_load_file_contents(sSaveDevices[sActiveSaveDevice].device, SATURN_SAVE_FILE_NAME, &length);
    if (loaded == NULL) {
        return FALSE;
    }

    PlatformSaturn_NormalizeSaveImage(FLASH_BASE, sizeof(FLASH_BASE), loaded, length);
    jo_free(loaded);
    return TRUE;
}

bool8 PlatformSaturn_StoreSaveBackend(void)
{
    size_t attempt;
    size_t startIndex;

    if (!PlatformSaturn_HasSaveBackend()) {
        return FALSE;
    }

    startIndex = (size_t)sActiveSaveDevice;
    for (attempt = 0; attempt < ARRAY_COUNT(sSaveDevices); ++attempt) {
        const size_t deviceIndex = (startIndex + attempt) % ARRAY_COUNT(sSaveDevices);
        PlatformSaturnMountedSaveDevice *device = &sSaveDevices[deviceIndex];

        if (!device->mounted) {
            continue;
        }

        if (jo_backup_save_file_contents(device->device, SATURN_SAVE_FILE_NAME, SATURN_SAVE_FILE_COMMENT, FLASH_BASE, sizeof(FLASH_BASE))) {
            device->hasSaveFile = TRUE;
            sActiveSaveDevice = (s8)deviceIndex;
            return TRUE;
        }
    }

    return FALSE;
}

const char *PlatformSaturn_GetSaveBackendName(void)
{
    if (!PlatformSaturn_HasSaveBackend()) {
        return "none";
    }

    return sSaveDeviceNames[sActiveSaveDevice];
}

bool8 PlatformSaturn_SaveBackendHasExistingSave(void)
{
    if (!PlatformSaturn_HasSaveBackend()) {
        return FALSE;
    }

    return sSaveDevices[sActiveSaveDevice].hasSaveFile;
}
