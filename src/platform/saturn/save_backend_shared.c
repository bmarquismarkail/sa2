#include <string.h>

#include "platform/saturn/save_backend.h"

int PlatformSaturn_SelectSaveDevice(const PlatformSaturnSaveDeviceState *devices, size_t deviceCount)
{
    size_t i;

    if (devices == NULL) {
        return PLATFORM_SATURN_SAVE_DEVICE_NONE;
    }

    for (i = 0; i < deviceCount; ++i) {
        if (devices[i].mounted && devices[i].has_save_file) {
            return (int)i;
        }
    }

    for (i = 0; i < deviceCount; ++i) {
        if (devices[i].mounted) {
            return (int)i;
        }
    }

    return PLATFORM_SATURN_SAVE_DEVICE_NONE;
}

void PlatformSaturn_NormalizeSaveImage(unsigned char *dst, size_t dstSize, const unsigned char *src, size_t srcSize)
{
    size_t copySize = dstSize;

    if ((dst == NULL) || (dstSize == 0)) {
        return;
    }

    memset(dst, 0xFF, dstSize);

    if ((src == NULL) || (srcSize == 0)) {
        return;
    }

    if (srcSize < copySize) {
        copySize = srcSize;
    }

    memcpy(dst, src, copySize);
}
