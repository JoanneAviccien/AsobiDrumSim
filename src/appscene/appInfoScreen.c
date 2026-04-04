/**
 * Layar info aplikasi - menampilkan tutorial penggunaan dan tombol keyboard.
 */
#define RAYGUI_IMPLEMENTATION
#include "appInfoScreen.h"
#include "global.h"
#include "raygui.h"
#include "raylib.h"

#define PADDING_X 25
#define PADDING_TOP 15
#define LINE_HEIGHT 28
#define SECTION_GAP 30
#define TITLE_HEIGHT 35

static void DrawSection(int x, int y, int w, int h, const char *title) {
  DrawRectangle(x, y, w, h, (Color){30, 30, 35, 255});
  DrawRectangleLinesEx((Rectangle){(float)x, (float)y, (float)w, (float)h}, 2,
                       (Color){100, 100, 120, 255});

  DrawRectangle(x, y, w, TITLE_HEIGHT, (Color){60, 60, 80, 255});
  DrawText(title, x + PADDING_X, y + 9, 16, YELLOW);
}

void appInfoScreen(void) {
  ClearBackground(BLACK);

  DrawText("SIMULATOR DRUMKIT ASOBI - TUTORIAL", scrWidth / 2 - 210, 25, 22,
           GREEN);
  DrawText("Panduan penggunaan drum kit virtual", scrWidth / 2 - 120, 58, 14,
           LIGHTGRAY);

  int leftX = 40;
  int rightX = 900;
  int sectionWidth = 520;
  int currentY = 120;

  int sec1Height = 150;
  DrawSection(leftX, currentY, sectionWidth, sec1Height, "1. Navigasi");
  int textX = leftX + PADDING_X;
  int textY = currentY + TITLE_HEIGHT + PADDING_TOP;
  DrawText("- Dari Menu Utama, klik 'Start Jamming' untuk masuk ke Simulator",
           textX, textY, 13, WHITE);
  textY += LINE_HEIGHT;
  DrawText("- Klik 'Kembali' untuk kembali ke menu", textX, textY, 13, WHITE);
  textY += LINE_HEIGHT;
  DrawText("- Klik tombol 'App Info' di menu untuk melihat panduan ini", textX,
           textY, 13, WHITE);

  currentY += sec1Height + SECTION_GAP;

  int sec2Height = 150;
  DrawSection(leftX, currentY, sectionWidth, sec2Height, "2. Bermain Drum");
  textY = currentY + TITLE_HEIGHT + PADDING_TOP;
  DrawText("- Gunakan tombol keyboard untuk memukul setiap bagian drum", textX,
           textY, 13, WHITE);
  textY += LINE_HEIGHT;
  DrawText("- Setiap drum memiliki tombol unik yang berbeda", textX, textY, 13,
           WHITE);
  textY += LINE_HEIGHT;
  DrawText("- Tekan beberapa tombol sekaligus untuk kombinasi suara", textX,
           textY, 13, WHITE);

  currentY += sec2Height + SECTION_GAP;

  int sec3Height = 130;
  DrawSection(leftX, currentY, sectionWidth, sec3Height, "3. Mode Demo");
  textY = currentY + TITLE_HEIGHT + PADDING_TOP;
  DrawText("- Klik tombol 'DRUM DEMO' (pojok kanan atas) untuk putar otomatis",
           textX, textY, 13, WHITE);
  textY += LINE_HEIGHT;
  DrawText("- Klik 'STOP DEMO' untuk menghentikan pemutaran otomatis", textX,
           textY, 13, WHITE);
  textY += LINE_HEIGHT;

  currentY = 120;
  int rightWidth = 340;
  int sec4Height = 300;
  DrawSection(rightX, currentY, rightWidth, sec4Height, "4. Tombol Keyboard");
  int col1X = rightX + PADDING_X;
  int col2X = rightX + (rightWidth / 2) + 10;
  int startY = currentY + TITLE_HEIGHT + PADDING_TOP;
  int rowH = 26;

  const char *keys[] = {"SPACE", "J", "D", "F", "K", "L", "H"};
  const char *drums1[] = {"Kick",   "Snare",  "HiHat",   "HiHatO",
                          "HiTom1", "HiTom2", "FloorTom"};

  for (int i = 0; i < 7; i++) {
    int y = startY + (i * rowH);
    DrawText(keys[i], col1X, y, 12, GREEN);
    DrawText("->", col1X + 55, y, 12, LIGHTGRAY);
    DrawText(drums1[i], col1X + 75, y, 12, WHITE);
  }

  const char *keys2[] = {"U", "I", "O", "P", "N", "M", "B"};
  const char *drums2[] = {"Ride",   "Ride2",  "RideBell", "RideCrash",
                          "SnRoll", "Snare2", "LowTom2"};

  for (int i = 0; i < 7; i++) {
    int y = startY + (i * rowH);
    DrawText(keys2[i], col2X, y, 12, GREEN);
    DrawText("->", col2X + 40, y, 12, LIGHTGRAY);
    DrawText(drums2[i], col2X + 60, y, 12, WHITE);
  }

  currentY += sec4Height + SECTION_GAP;

  if (GuiButton(backBtn, "Kembali")) {
    currentScreen = SCREEN_MENU;
    PlaySound(btnAction);
  }
}
