/**
 * Implementasi algoritma gambar elips Midpoint.
 *
 * Mengimplementasikan gambar elips menggunakan algoritma midpoint dengan simetri 4-arah,
 * menggunakan hanya aritmetika bilangan bulat. Termasuk varian terisi, tebal, putus-putus, dan elips setengah.
 */
#include "midpoint_ellipse.h"
#include "bresenham.h"
#include <math.h>

static void PlotSymmetryPoints(int cx, int cy, int x, int y, Color color) {
  DrawPixel(cx + x, cy + y, color);
  DrawPixel(cx - x, cy + y, color);
  DrawPixel(cx - x, cy - y, color);
  DrawPixel(cx + x, cy - y, color);
}

void MidpointEllipse(int centerX, int centerY, int rx, int ry, Color color) {
  if (rx <= 0 || ry <= 0) {
    DrawPixel(centerX, centerY, color);
    return;
  }

  int x = 0;
  int y = ry;

  int rx2 = rx * rx;
  int ry2 = ry * ry;
  int twoRx2 = 2 * rx2;
  int twoRy2 = 2 * ry2;

  int d1 = ry2 - rx2 * ry + (rx2 / 4);
  int inc1_x = twoRy2 * x;
  int inc1_y = twoRx2 * y;

  while (inc1_x < inc1_y) {
    PlotSymmetryPoints(centerX, centerY, x, y, color);

    if (d1 < 0) {
      d1 += inc1_x + ry2;
      x++;
      inc1_x += twoRy2;
    } else {
      d1 += inc1_x - inc1_y + ry2;
      x++;
      y--;
      inc1_x += twoRy2;
      inc1_y -= twoRx2;
    }
  }

  int d2 =
      ry2 * ((x + 0.5) * (x + 0.5)) + rx2 * ((y - 1) * (y - 1)) - rx2 * ry2;

  while (y >= 0) {
    PlotSymmetryPoints(centerX, centerY, x, y, color);

    if (d2 < 0) {
      d2 += inc1_x + ry2;
      x++;
      inc1_x += twoRy2;
    } else {
      d2 -= inc1_y + rx2;
      y--;
      inc1_y -= twoRx2;
    }
  }
}

void MidpointEllipseFilled(int centerX, int centerY, int rx, int ry,
                           Color color) {
  if (rx <= 0 || ry <= 0) {
    DrawPixel(centerX, centerY, color);
    return;
  }

  int x = 0;
  int y = ry;

  int rx2 = rx * rx;
  int ry2 = ry * ry;
  int twoRx2 = 2 * rx2;
  int twoRy2 = 2 * ry2;

  int d1 = ry2 - rx2 * ry + (rx2 / 4);
  int inc1_x = twoRy2 * x;
  int inc1_y = twoRx2 * y;

  while (inc1_x < inc1_y) {
    for (int i = centerX - x; i <= centerX + x; i++) {
      DrawPixel(i, centerY + y, color);
      DrawPixel(i, centerY - y, color);
    }

    if (d1 < 0) {
      d1 += inc1_x + ry2;
      x++;
      inc1_x += twoRy2;
    } else {
      d1 += inc1_x - inc1_y + ry2;
      x++;
      y--;
      inc1_x += twoRy2;
      inc1_y -= twoRx2;
    }
  }

  int d2 =
      ry2 * ((x + 0.5) * (x + 0.5)) + rx2 * ((y - 1) * (y - 1)) - rx2 * ry2;

  while (y >= 0) {
    for (int i = centerX - x; i <= centerX + x; i++) {
      DrawPixel(i, centerY + y, color);
      DrawPixel(i, centerY - y, color);
    }

    if (d2 < 0) {
      d2 += inc1_x + ry2;
      x++;
      inc1_x += twoRy2;
    } else {
      d2 -= inc1_y + rx2;
      y--;
      inc1_y -= twoRx2;
    }
  }
}

void MidpointEllipseThick(int centerX, int centerY, int rx, int ry,
                          int thickness, Color color) {
  if (rx <= 0 || ry <= 0 || thickness <= 0) {
    DrawPixel(centerX, centerY, color);
    return;
  }

  int innerRx = rx - thickness / 2;
  int innerRy = ry - thickness / 2;

  if (innerRx < 1)
    innerRx = 1;
  if (innerRy < 1)
    innerRy = 1;

  for (int r = innerRx; r <= rx + thickness / 2; r++) {
    MidpointEllipse(centerX, centerY, r, ry * r / rx, color);
  }
}

void MidpointEllipseDashed(int centerX, int centerY, int rx, int ry,
                           int dashLen, int gapLen, Color color) {
  if (rx <= 0 || ry <= 0) {
    DrawPixel(centerX, centerY, color);
    return;
  }

  int x = 0;
  int y = ry;

  int rx2 = rx * rx;
  int ry2 = ry * ry;
  int twoRx2 = 2 * rx2;
  int twoRy2 = 2 * ry2;

  int d1 = ry2 - rx2 * ry + (rx2 / 4);
  int inc1_x = twoRy2 * x;
  int inc1_y = twoRx2 * y;

  int counter = 0;
  int drawing = 1;
  int current_limit = dashLen;

  while (inc1_x < inc1_y) {
    if (drawing) {
      PlotSymmetryPoints(centerX, centerY, x, y, color);
    }

    counter += 4;
    if (counter >= current_limit) {
      counter = 0;
      drawing = !drawing;
      current_limit = drawing ? dashLen : gapLen;
    }

    if (d1 < 0) {
      d1 += inc1_x + ry2;
      x++;
      inc1_x += twoRy2;
    } else {
      d1 += inc1_x - inc1_y + ry2;
      x++;
      y--;
      inc1_x += twoRy2;
      inc1_y -= twoRx2;
    }
  }

  int d2 =
      ry2 * ((x + 0.5) * (x + 0.5)) + rx2 * ((y - 1) * (y - 1)) - rx2 * ry2;

  while (y >= 0) {
    if (drawing) {
      PlotSymmetryPoints(centerX, centerY, x, y, color);
    }

    counter += 4;
    if (counter >= current_limit) {
      counter = 0;
      drawing = !drawing;
      current_limit = drawing ? dashLen : gapLen;
    }

    if (d2 < 0) {
      d2 += inc1_x + ry2;
      x++;
      inc1_x += twoRy2;
    } else {
      d2 -= inc1_y + rx2;
      y--;
      inc1_y -= twoRx2;
    }
  }
}

static void PlotHalfEllipsePoints(int cx, int cy, int x, int y, HalfEllipseType type, Color color) {
  switch (type) {
    case HALF_ELLIPSE_TOP:
      DrawPixel(cx + x, cy - y, color);
      DrawPixel(cx - x, cy - y, color);
      break;
    case HALF_ELLIPSE_BOTTOM:
      DrawPixel(cx + x, cy + y, color);
      DrawPixel(cx - x, cy + y, color);
      break;
    case HALF_ELLIPSE_LEFT:
      DrawPixel(cx - x, cy + y, color);
      DrawPixel(cx - x, cy - y, color);
      break;
    case HALF_ELLIPSE_RIGHT:
      DrawPixel(cx + x, cy + y, color);
      DrawPixel(cx + x, cy - y, color);
      break;
  }
}

void MidpointHalfEllipse(int centerX, int centerY, int rx, int ry, HalfEllipseType type, Color color) {
  if (rx <= 0 || ry <= 0) {
    DrawPixel(centerX, centerY, color);
    return;
  }

  int x = 0;
  int y = ry;

  int rx2 = rx * rx;
  int ry2 = ry * ry;
  int twoRx2 = 2 * rx2;
  int twoRy2 = 2 * ry2;

  int d1 = ry2 - rx2 * ry + (rx2 / 4);
  int inc1_x = twoRy2 * x;
  int inc1_y = twoRx2 * y;

  while (inc1_x < inc1_y) {
    PlotHalfEllipsePoints(centerX, centerY, x, y, type, color);

    if (d1 < 0) {
      d1 += inc1_x + ry2;
      x++;
      inc1_x += twoRy2;
    } else {
      d1 += inc1_x - inc1_y + ry2;
      x++;
      y--;
      inc1_x += twoRy2;
      inc1_y -= twoRx2;
    }
  }

  int d2 =
      ry2 * ((x + 0.5) * (x + 0.5)) + rx2 * ((y - 1) * (y - 1)) - rx2 * ry2;

  while (y >= 0) {
    PlotHalfEllipsePoints(centerX, centerY, x, y, type, color);

    if (d2 < 0) {
      d2 += inc1_x + ry2;
      x++;
      inc1_x += twoRy2;
    } else {
      d2 -= inc1_y + rx2;
      y--;
      inc1_y -= twoRx2;
    }
  }
}
