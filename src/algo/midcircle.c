/**
 * Implementasi algoritma gambar lingkaran Midpoint.
 *
 * Mengimplementasikan gambar lingkaran menggunakan algoritma midpoint dengan simetri 8-arah,
 * menggunakan hanya aritmetika bilangan bulat. Termasuk varian terisi, tebal, dan putus-putus.
 */
#include "midcircle.h"

static void PlotSymmetryPoints(int cx, int cy, int x, int y, Color color) {
    DrawPixel(cx + x, cy + y, color);
    DrawPixel(cx + y, cy + x, color);
    DrawPixel(cx + y, cy - x, color);
    DrawPixel(cx + x, cy - y, color);
    DrawPixel(cx - x, cy - y, color);
    DrawPixel(cx - y, cy - x, color);
    DrawPixel(cx - y, cy + x, color);
    DrawPixel(cx - x, cy + y, color);
}

void Midcircle(int centerX, int centerY, int radius, Color color) {
    if (radius <= 0) {
        DrawPixel(centerX, centerY, color);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        PlotSymmetryPoints(centerX, centerY, x, y, color);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void MidcircleFilled(int centerX, int centerY, int radius, Color color) {
    if (radius <= 0) {
        DrawPixel(centerX, centerY, color);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        for (int i = centerX - x; i <= centerX + x; i++) {
            DrawPixel(i, centerY + y, color);
            DrawPixel(i, centerY - y, color);
        }
        for (int i = centerX - y; i <= centerX + y; i++) {
            DrawPixel(i, centerY + x, color);
            DrawPixel(i, centerY - x, color);
        }

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void MidcircleThick(int centerX, int centerY, int radius, int thickness, Color color) {
    if (radius <= 0 || thickness <= 0) {
        DrawPixel(centerX, centerY, color);
        return;
    }

    int innerR = radius - thickness / 2;
    if (innerR < 0) innerR = 0;
    int outerR = radius + thickness / 2;

    for (int r = innerR; r <= outerR; r++) {
        Midcircle(centerX, centerY, r, color);
    }
}

void MidcircleDashed(int centerX, int centerY, int radius, int dashLen, int gapLen, Color color) {
    if (radius <= 0) {
        DrawPixel(centerX, centerY, color);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    int counter = 0;
    int drawing = 1;
    int current_limit = dashLen;

    while (y >= x) {
        if (drawing) {
            DrawPixel(centerX + x, centerY + y, color);
            DrawPixel(centerX + y, centerY + x, color);
            DrawPixel(centerX + y, centerY - x, color);
            DrawPixel(centerX + x, centerY - y, color);
            DrawPixel(centerX - x, centerY - y, color);
            DrawPixel(centerX - y, centerY - x, color);
            DrawPixel(centerX - y, centerY + x, color);
            DrawPixel(centerX - x, centerY + y, color);
        }

        counter += 8;
        if (counter >= current_limit) {
            counter = 0;
            drawing = !drawing;
            current_limit = drawing ? dashLen : gapLen;
        }

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
