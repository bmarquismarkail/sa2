#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "platform/saturn/save_backend.h"

static void TestSelectPrefersExistingSave(void)
{
    const PlatformSaturnSaveDeviceState devices[] = {
        { .mounted = 1, .has_save_file = 0 },
        { .mounted = 1, .has_save_file = 1 },
        { .mounted = 1, .has_save_file = 1 },
    };

    assert(PlatformSaturn_SelectSaveDevice(devices, 3) == 1);
}

static void TestSelectFallsBackToFirstMountedDevice(void)
{
    const PlatformSaturnSaveDeviceState devices[] = {
        { .mounted = 0, .has_save_file = 0 },
        { .mounted = 1, .has_save_file = 0 },
        { .mounted = 1, .has_save_file = 0 },
    };

    assert(PlatformSaturn_SelectSaveDevice(devices, 3) == 1);
}

static void TestSelectReturnsNoneWithoutMountedDevice(void)
{
    const PlatformSaturnSaveDeviceState devices[] = {
        { .mounted = 0, .has_save_file = 0 },
        { .mounted = 0, .has_save_file = 1 },
        { .mounted = 0, .has_save_file = 0 },
    };

    assert(PlatformSaturn_SelectSaveDevice(devices, 3) == PLATFORM_SATURN_SAVE_DEVICE_NONE);
}

static void TestNormalizeSaveImageCopiesAndPads(void)
{
    unsigned char dst[8];
    const unsigned char src[] = { 0x01, 0x02, 0x03 };

    PlatformSaturn_NormalizeSaveImage(dst, sizeof(dst), src, sizeof(src));

    assert(dst[0] == 0x01);
    assert(dst[1] == 0x02);
    assert(dst[2] == 0x03);
    for (size_t i = 3; i < sizeof(dst); ++i) {
        assert(dst[i] == 0xFF);
    }
}

static void TestNormalizeSaveImageTruncatesOversizedInput(void)
{
    unsigned char dst[4];
    const unsigned char src[] = { 0x10, 0x11, 0x12, 0x13, 0x14 };

    PlatformSaturn_NormalizeSaveImage(dst, sizeof(dst), src, sizeof(src));

    assert(memcmp(dst, src, sizeof(dst)) == 0);
}

int main(void)
{
    TestSelectPrefersExistingSave();
    TestSelectFallsBackToFirstMountedDevice();
    TestSelectReturnsNoneWithoutMountedDevice();
    TestNormalizeSaveImageCopiesAndPads();
    TestNormalizeSaveImageTruncatesOversizedInput();
    return 0;
}
