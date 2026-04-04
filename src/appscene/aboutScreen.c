/**
 * Layar tentang - menampilkan informasi pengembang/profil.
 */
#include "aboutScreen.h"
#include "global.h"
#include "raylib.h"

void aboutScreen(void) {
  Vector2 mousePoint = GetMousePosition();

  ClearBackground(BLACK);

  DrawText("TENTANG SAYA", scrWidth / 2 - 160, 60, 36, GREEN);
  DrawText("Profil Pengembang Aplikasi", scrWidth / 2 - 110, 105, 16,
           LIGHTGRAY);

  int boxX = scrWidth / 2 - 250;
  int boxY = 150;
  int boxW = 500;
  int boxH = 280;

  DrawRectangle(boxX, boxY, boxW, boxH, (Color){30, 30, 35, 255});
  DrawRectangleLinesEx(
      (Rectangle){(float)boxX, (float)boxY, (float)boxW, (float)boxH}, 2,
      (Color){100, 100, 120, 255});

  DrawRectangle(boxX, boxY, boxW, 50, (Color){60, 60, 80, 255});
  DrawText("INFORMASI MAHASISWA", boxX + 20, boxY + 13, 20, YELLOW);

  int photoX = boxX + boxW / 2;
  int photoY = boxY + 120;
  int photoRadius = 50;
  DrawCircle(photoX, photoY, photoRadius, (Color){80, 80, 100, 255});
  DrawCircleLines(photoX, photoY, photoRadius, (Color){150, 150, 180, 255});
  DrawText("J", photoX - 20, photoY - 8, 16, LIGHTGRAY);

  DrawText("Nama:", boxX + 40, boxY + 200, 18, GREEN);
  DrawText("Johan Muhammad Avicenna", boxX + 140, boxY + 200, 18, WHITE);

  DrawText("NIM:", boxX + 40, boxY + 235, 18, GREEN);
  DrawText("241524013", boxX + 140, boxY + 235, 18, WHITE);

  int infoY = boxY + boxH + 40;
  DrawText("Proyek: Asobi Drumkit Simulator", scrWidth / 2 - 140, infoY, 16,
           LIGHTGRAY);
  DrawText("Mata Kuliah: Komputer Grafik", scrWidth / 2 - 120, infoY + 30, 14,
           DARKGRAY);

  bool hoverBack = CheckCollisionPointRec(mousePoint, backBtn);
  Color backBtnColor =
      hoverBack ? (Color){220, 220, 220, 255} : (Color){180, 180, 180, 255};
  DrawRectangleRec(backBtn, backBtnColor);
  DrawText("KEMBALI", (int)backBtn.x + 32, (int)backBtn.y + 14, 22,
           (Color){30, 30, 30, 255});

  if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
    currentScreen = SCREEN_MENU;
  }

  if (hoverBack && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    PlaySound(btnAction);
    currentScreen = SCREEN_MENU;
  }
}
