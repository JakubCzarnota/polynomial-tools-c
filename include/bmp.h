#ifndef BMP_H
#define BMP_H

int bmp_write(
    const char *filename,
    int width,
    int height,
    const unsigned char *data
);

#endif // BMP_H
