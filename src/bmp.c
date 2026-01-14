#include "bmp.h"
#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop)

int bmp_write(const char *filename, int width, int height,
              const unsigned char *data)
{
    FILE *f = fopen(filename, "wb");
    if (!f) return 1;

    int row_size = (width * 3 + 3) & ~3;
    int image_size = row_size * height;

    BITMAPFILEHEADER file_header = {
        0x4D42,
        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + image_size,
        0, 0,
        sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    };

    BITMAPINFOHEADER info_header = {
        sizeof(BITMAPINFOHEADER),
        width,
        height,
        1,
        24,
        0,
        image_size,
        0, 0,
        0, 0
    };

    fwrite(&file_header, sizeof(file_header), 1, f);
    fwrite(&info_header, sizeof(info_header), 1, f);

    unsigned char padding[3] = {0};

    for (int y = height - 1; y >= 0; y--) {
        const unsigned char *row = data + y * width * 3;
        for (int x = 0; x < width; x++) {
            unsigned char bgr[3] = {
                row[x * 3 + 2], // B
                row[x * 3 + 1], // G
                row[x * 3 + 0]  // R
            };
            fwrite(bgr, 3, 1, f);
        }
        fwrite(padding, row_size - width * 3, 1, f);
    }

    fclose(f);
    return 0;
}
