/**
 * Implementasi algoritma gambar persegi panjang.
 *
 * Mengimplementasikan gambar persegi panjang kosong, tebal, dan terisi dengan menyusun
 * garis Bresenham untuk sisi-sisinya.
 */
#include "rect.h"
#include "bresenham.h"
#include <math.h>

void hollowRect(int x, int y, int width, int height, Color color) {
    int x2 = x + width;
    int y2 = y + height;

    BresenhamLine(x, y, x2, y, color);
    BresenhamLine(x, y2, x2, y2, color);
    BresenhamLine(x, y, x, y2, color);
    BresenhamLine(x2, y, x2, y2, color);
}

void thickHollowRect(int x, int y, int width, int height, int stroke, Color color) {
    int x2 = x + width;
    int y2 = y + height;

    Bres_ThickLine(x, y, x2, y, stroke, color);
    Bres_ThickLine(x, y2, x2, y2, stroke, color);
    Bres_ThickLine(x, y, x, y2, stroke, color);
    Bres_ThickLine(x2, y, x2, y2, stroke, color);
}

void filledRect(int x, int y, int width, int height, Color color) {
    for (int scanline = y; scanline < y + height; scanline++) {
        BresenhamLine(x, scanline, x + width, scanline, color);
    }
}
