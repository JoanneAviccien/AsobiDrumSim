/**
 * Algoritma gambar persegi panjang.
 *
 * Menyediakan fungsi untuk menggambar persegi panjang kosong, tebal, dan terisi
 * menggunakan algoritma garis Bresenham untuk sisi-sisinya.
 */
#ifndef RECT_H
#define RECT_H

#include "raylib.h"

void hollowRect(int x, int y, int width, int height, Color color);
void thickHollowRect(int x, int y, int width, int height, int stroke, Color color);
void filledRect(int x, int y, int width, int height, Color color);

#endif
