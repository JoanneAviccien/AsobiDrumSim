/**
 * Algoritma gambar elips Midpoint.
 *
 * Menyediakan fungsi untuk menggambar elips menggunakan algoritma midpoint
 * dengan simetri 4-arah, termasuk varian terisi, tebal, putus-putus, dan elips setengah.
 */
#ifndef MIDPOINT_ELLIPSE_H
#define MIDPOINT_ELLIPSE_H

#include "raylib.h"

typedef enum {
    HALF_ELLIPSE_TOP,
    HALF_ELLIPSE_BOTTOM,
    HALF_ELLIPSE_LEFT,
    HALF_ELLIPSE_RIGHT
} HalfEllipseType;

void MidpointEllipse(int centerX, int centerY, int rx, int ry, Color color);
void MidpointEllipseFilled(int centerX, int centerY, int rx, int ry, Color color);
void MidpointEllipseThick(int centerX, int centerY, int rx, int ry, int thickness, Color color);
void MidpointEllipseDashed(int centerX, int centerY, int rx, int ry, int dashLen, int gapLen, Color color);
void MidpointHalfEllipse(int centerX, int centerY, int rx, int ry, HalfEllipseType type, Color color);

#endif
