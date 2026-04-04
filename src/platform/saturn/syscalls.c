#include <string.h>

#include "gba/gba.h"

static const s16 sSaturnSineTable[256] = {
    (s16)0x0000, (s16)0x0192, (s16)0x0323, (s16)0x04B5, (s16)0x0645, (s16)0x07D5, (s16)0x0964, (s16)0x0AF1, (s16)0x0C7C,
    (s16)0x0E05, (s16)0x0F8C, (s16)0x1111, (s16)0x1294, (s16)0x1413, (s16)0x158F, (s16)0x1708, (s16)0x187D, (s16)0x19EF,
    (s16)0x1B5D, (s16)0x1CC6, (s16)0x1E2B, (s16)0x1F8B, (s16)0x20E7, (s16)0x223D, (s16)0x238E, (s16)0x24DA, (s16)0x261F,
    (s16)0x275F, (s16)0x2899, (s16)0x29CD, (s16)0x2AFA, (s16)0x2C21, (s16)0x2D41, (s16)0x2E5A, (s16)0x2F6B, (s16)0x3076,
    (s16)0x3179, (s16)0x3274, (s16)0x3367, (s16)0x3453, (s16)0x3536, (s16)0x3612, (s16)0x36E5, (s16)0x37AF, (s16)0x3871,
    (s16)0x392A, (s16)0x39DA, (s16)0x3A82, (s16)0x3B20, (s16)0x3BB6, (s16)0x3C42, (s16)0x3CC5, (s16)0x3D3E, (s16)0x3DAE,
    (s16)0x3E14, (s16)0x3E71, (s16)0x3EC5, (s16)0x3F0E, (s16)0x3F4E, (s16)0x3F84, (s16)0x3FB1, (s16)0x3FD3, (s16)0x3FEC,
    (s16)0x3FFB, (s16)0x4000, (s16)0x3FFB, (s16)0x3FEC, (s16)0x3FD3, (s16)0x3FB1, (s16)0x3F84, (s16)0x3F4E, (s16)0x3F0E,
    (s16)0x3EC5, (s16)0x3E71, (s16)0x3E14, (s16)0x3DAE, (s16)0x3D3E, (s16)0x3CC5, (s16)0x3C42, (s16)0x3BB6, (s16)0x3B20,
    (s16)0x3A82, (s16)0x39DA, (s16)0x392A, (s16)0x3871, (s16)0x37AF, (s16)0x36E5, (s16)0x3612, (s16)0x3536, (s16)0x3453,
    (s16)0x3367, (s16)0x3274, (s16)0x3179, (s16)0x3076, (s16)0x2F6B, (s16)0x2E5A, (s16)0x2D41, (s16)0x2C21, (s16)0x2AFA,
    (s16)0x29CD, (s16)0x2899, (s16)0x275F, (s16)0x261F, (s16)0x24DA, (s16)0x238E, (s16)0x223D, (s16)0x20E7, (s16)0x1F8B,
    (s16)0x1E2B, (s16)0x1CC6, (s16)0x1B5D, (s16)0x19EF, (s16)0x187D, (s16)0x1708, (s16)0x158F, (s16)0x1413, (s16)0x1294,
    (s16)0x1111, (s16)0x0F8C, (s16)0x0E05, (s16)0x0C7C, (s16)0x0AF1, (s16)0x0964, (s16)0x07D5, (s16)0x0645, (s16)0x04B5,
    (s16)0x0323, (s16)0x0192, (s16)0x0000, (s16)0xFE6E, (s16)0xFCDD, (s16)0xFB4B, (s16)0xF9BB, (s16)0xF82B, (s16)0xF69C,
    (s16)0xF50F, (s16)0xF384, (s16)0xF1FB, (s16)0xF074, (s16)0xEEEF, (s16)0xED6C, (s16)0xEBED, (s16)0xEA71, (s16)0xE8F8,
    (s16)0xE783, (s16)0xE611, (s16)0xE4A3, (s16)0xE33A, (s16)0xE1D5, (s16)0xE075, (s16)0xDF19, (s16)0xDDC3, (s16)0xDC72,
    (s16)0xDB26, (s16)0xD9E1, (s16)0xD8A1, (s16)0xD767, (s16)0xD633, (s16)0xD506, (s16)0xD3DF, (s16)0xD2BF, (s16)0xD1A6,
    (s16)0xD095, (s16)0xCF8A, (s16)0xCE87, (s16)0xCD8C, (s16)0xCC99, (s16)0xCBAD, (s16)0xCACA, (s16)0xC9EE, (s16)0xC91B,
    (s16)0xC851, (s16)0xC78F, (s16)0xC6D6, (s16)0xC626, (s16)0xC57E, (s16)0xC4E0, (s16)0xC44A, (s16)0xC3BE, (s16)0xC33B,
    (s16)0xC2C2, (s16)0xC252, (s16)0xC1EC, (s16)0xC18F, (s16)0xC13B, (s16)0xC0F2, (s16)0xC0B2, (s16)0xC07C, (s16)0xC04F,
    (s16)0xC02D, (s16)0xC014, (s16)0xC005, (s16)0xC000, (s16)0xC005, (s16)0xC014, (s16)0xC02D, (s16)0xC04F, (s16)0xC07C,
    (s16)0xC0B2, (s16)0xC0F2, (s16)0xC13B, (s16)0xC18F, (s16)0xC1EC, (s16)0xC252, (s16)0xC2C2, (s16)0xC33B, (s16)0xC3BE,
    (s16)0xC44A, (s16)0xC4E0, (s16)0xC57E, (s16)0xC626, (s16)0xC6D6, (s16)0xC78F, (s16)0xC851, (s16)0xC91B, (s16)0xC9EE,
    (s16)0xCACA, (s16)0xCBAD, (s16)0xCC99, (s16)0xCD8C, (s16)0xCE87, (s16)0xCF8A, (s16)0xD095, (s16)0xD1A6, (s16)0xD2BF,
    (s16)0xD3DF, (s16)0xD506, (s16)0xD633, (s16)0xD767, (s16)0xD8A1, (s16)0xD9E1, (s16)0xDB26, (s16)0xDC72, (s16)0xDDC3,
    (s16)0xDF19, (s16)0xE075, (s16)0xE1D5, (s16)0xE33A, (s16)0xE4A3, (s16)0xE611, (s16)0xE783, (s16)0xE8F8, (s16)0xEA71,
    (s16)0xEBED, (s16)0xED6C, (s16)0xEEEF, (s16)0xF074, (s16)0xF1FB, (s16)0xF384, (s16)0xF50F, (s16)0xF69C, (s16)0xF82B,
    (s16)0xF9BB, (s16)0xFB4B, (s16)0xFCDD, (s16)0xFE6E,
};

static u16 SaturnSqrt(u32 num)
{
    u32 bit = 1UL << 30;
    u32 result = 0;

    while (bit > num) {
        bit >>= 2;
    }

    while (bit != 0) {
        if (num >= result + bit) {
            num -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }

        bit >>= 2;
    }

    return (u16)result;
}

u16 Sqrt(u32 num) { return SaturnSqrt(num); }

u16 ArcTan(s16 i)
{
    s32 a = -((i * i) >> 14);
    s32 b = ((0xA9 * a) >> 14) + 0x390;
    b = ((b * a) >> 14) + 0x91C;
    b = ((b * a) >> 14) + 0xFB6;
    b = ((b * a) >> 14) + 0x16AA;
    b = ((b * a) >> 14) + 0x2081;
    b = ((b * a) >> 14) + 0x3651;
    b = ((b * a) >> 14) + 0xA2F9;

    return (i * b) >> 16;
}

u16 ArcTan2(s16 x, s16 y)
{
    if (y == 0) {
        if (x >= 0) {
            return 0;
        }

        return 0x8000;
    }

    if (x == 0) {
        if (y >= 0) {
            return 0x4000;
        }

        return 0xC000;
    }

    if (y >= 0) {
        if (x >= 0) {
            if (x >= y) {
                return ArcTan((y << 14) / x);
            }

            return 0x4000 - ArcTan((x << 14) / y);
        }

        if (-x >= y) {
            return ArcTan((y << 14) / x) + 0x8000;
        }

        return 0x4000 - ArcTan((x << 14) / y);
    }

    if (x < 0) {
        if (-x > -y) {
            return ArcTan((y << 14) / x) + 0x8000;
        }

        return 0xC000 - ArcTan((x << 14) / y);
    }

    if (x >= -y) {
        return ArcTan((y << 14) / x) + 0x10000;
    }

    return 0xC000 - ArcTan((x << 14) / y);
}

void BgAffineSet(struct BgAffineSrcData *src, struct BgAffineDstData *dest, s32 count)
{
    s32 i;

    for (i = 0; i < count; i++) {
        s32 cx = src[i].texX;
        s32 cy = src[i].texY;
        s16 dispx = src[i].scrX;
        s16 dispy = src[i].scrY;
        s16 rx = src[i].sx;
        s16 ry = src[i].sy;
        u16 theta = src[i].alpha >> 8;
        s32 a = sSaturnSineTable[(theta + 0x40) & 255];
        s32 b = sSaturnSineTable[theta];
        s16 dx = (rx * a) >> 14;
        s16 dmx = (rx * b) >> 14;
        s16 dy = (ry * b) >> 14;
        s16 dmy = (ry * a) >> 14;
        s32 startx;
        s32 starty;

        dest[i].pa = dx;
        dest[i].pb = -dmx;
        dest[i].pc = dy;
        dest[i].pd = dmy;

        startx = cx - dx * dispx + dmx * dispy;
        starty = cy - dy * dispx - dmy * dispy;

        dest[i].dx = startx;
        dest[i].dy = starty;
    }
}

static void SaturnWriteHalfWord(void *dest, u16 val) { memcpy(dest, &val, sizeof(val)); }

void ObjAffineSet(struct ObjAffineSrcData *src, void *dest, s32 count, s32 offset)
{
    s32 i;

    for (i = 0; i < count; i++) {
        s16 rx = src[i].xScale;
        s16 ry = src[i].yScale;
        u16 theta = src[i].rotation >> 8;
        s32 a = sSaturnSineTable[(theta + 64) & 255];
        s32 b = sSaturnSineTable[theta];
        s16 dx = ((s32)rx * a) >> 14;
        s16 dmx = ((s32)rx * b) >> 14;
        s16 dy = ((s32)ry * b) >> 14;
        s16 dmy = ((s32)ry * a) >> 14;

        SaturnWriteHalfWord(dest, dx);
        dest += offset;
        SaturnWriteHalfWord(dest, -dmx);
        dest += offset;
        SaturnWriteHalfWord(dest, dy);
        dest += offset;
        SaturnWriteHalfWord(dest, dmy);
        dest += offset;
    }
}

static void SaturnLZDecompressUnsafe(const unsigned char *src, unsigned char *dest)
{
    int dest_size = (src[3] << 16) | (src[2] << 8) | src[1];
    int src_pos = 4;
    int dest_pos = 0;

    while (dest_pos < dest_size) {
        unsigned char flags = src[src_pos++];
        int i;

        for (i = 0; i < 8 && dest_pos < dest_size; i++) {
            if (flags & 0x80) {
                int block_size = (src[src_pos] >> 4) + 3;
                int block_distance = (((src[src_pos] & 0xF) << 8) | src[src_pos + 1]) + 1;
                int block_pos;
                int j;

                src_pos += 2;
                block_pos = dest_pos - block_distance;
                if (dest_pos + block_size > dest_size) {
                    block_size = dest_size - dest_pos;
                }

                for (j = 0; j < block_size; j++) {
                    dest[dest_pos++] = dest[block_pos + j];
                }
            } else {
                dest[dest_pos++] = src[src_pos++];
            }

            flags <<= 1;
        }
    }
}

static void SaturnRLDecompressUnsafe(const unsigned char *src, unsigned char *dest)
{
    int dest_size = (src[3] << 16) | (src[2] << 8) | src[1];
    int src_pos = 4;
    int dest_pos = 0;

    while (dest_pos < dest_size) {
        unsigned char flags = src[src_pos++];
        int length = (flags & 0x7F) + ((flags & 0x80) ? 3 : 1);
        int i;

        if (flags & 0x80) {
            unsigned char value = src[src_pos++];

            for (i = 0; i < length; i++) {
                dest[dest_pos++] = value;
            }
        } else {
            for (i = 0; i < length; i++) {
                dest[dest_pos++] = src[src_pos++];
            }
        }
    }
}

void LZ77UnCompWram(const void *src, void *dest) { SaturnLZDecompressUnsafe(src, dest); }

void LZ77UnCompVram(const void *src, void *dest) { SaturnLZDecompressUnsafe(src, dest); }

void RLUnCompWram(const void *src, void *dest) { SaturnRLDecompressUnsafe(src, dest); }

void RLUnCompVram(const void *src, void *dest) { SaturnRLDecompressUnsafe(src, dest); }
