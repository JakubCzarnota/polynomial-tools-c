#include "plot.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static void set_pixel(unsigned char *img, int w, int h,
                      int x, int y,
                      unsigned char r,
                      unsigned char g,
                      unsigned char b)
{
    if (x < 0 || x >= w || y < 0 || y >= h)
        return;

    int i = (y * w + x) * 3;
    img[i + 0] = r;
    img[i + 1] = g;
    img[i + 2] = b;
}

static void draw_thick_point(unsigned char *img, int w, int h,
                             int cx, int cy, int radius,
                             unsigned char r,
                             unsigned char g,
                             unsigned char b)
{
    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            if (dx * dx + dy * dy <= radius * radius)
            {
                set_pixel(img, w, h,
                          cx + dx, cy + dy,
                          r, g, b);
            }
        }
    }
}

static void draw_thick_line(unsigned char *img, int w, int h,
                            int x0, int y0,
                            int x1, int y1,
                            int radius,
                            unsigned char r,
                            unsigned char g,
                            unsigned char b)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    if (steps == 0)
        return;

    double x_inc = (double)dx / steps;
    double y_inc = (double)dy / steps;

    double x = x0;
    double y = y0;

    for (int i = 0; i <= steps; i++)
    {
        draw_thick_point(img, w, h,
                         (int)(x + 0.5),
                         (int)(y + 0.5),
                         radius,
                         r, g, b);
        x += x_inc;
        y += y_inc;
    }
}

void plot_function(unsigned char *image, int width, int height,
                   Function f,
                   double xmin, double xmax,
                   double ymin, double ymax,
                   int thickness)
{
    // Clear background (white)
    memset(image, 255, width * height * 3);

    // Draw X axis
    if (xmin < 0 && xmax > 0)
    {
        int x0 = (int)((0 - xmin) / (xmax - xmin) * width);
        for (int y = 0; y < height; y++)
            set_pixel(image, width, height, x0, y, 0, 0, 0);
    }

    // Draw Y axis
    if (ymin < 0 && ymax > 0)
    {
        int y0 = height - (int)((0 - ymin) / (ymax - ymin) * height);
        for (int x = 0; x < width; x++)
            set_pixel(image, width, height, x, y0, 0, 0, 0);
    }

    // Plot function with connected values
    int prev_x = 0;
    int prev_y = 0;
    int has_prev = 0;

    for (int x = 0; x < width; x++)
    {
        double fx = xmin + (double)x / width * (xmax - xmin);
        double fy = f(fx);

        if (isnan(fy) || isinf(fy))
        {
            has_prev = 0;
            continue;
        }

        int y = height - (int)((fy - ymin) / (ymax - ymin) * height);

        if (has_prev)
        {
            draw_thick_line(
                image, width, height,
                prev_x, prev_y,
                x, y,
                thickness,
                255, 0, 0);
        }

        prev_x = x;
        prev_y = y;
        has_prev = 1;
    }
}
