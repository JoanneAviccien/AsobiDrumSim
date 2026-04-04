/**
 * Asobi Drumkit Simulator - Titik masuk program.
 *
 * Menginisialisasi jendela, audio, dan sumber daya global, lalu menjalankan loop
 * permainan utama yang mengalihkan ke layar yang sesuai berdasarkan status.
 */
#include "raylib.h"
#include "src/appscene/aboutScreen.h"
#include "src/appscene/appInfoScreen.h"
#include "src/appscene/global.h"
#include "src/appscene/jamScreen.h"
#include "src/appscene/menuScreen.h"

int main(void) {
  InitAudioDevice();

  InitWindow(scrWidth, scrHeight, "Asobi Drumkit Simulator");
  SetTargetFPS(60);

  InitGlobals();
  currentScreen = SCREEN_MENU;

  while (!exitWindowFlag) {
    if (WindowShouldClose())
      exitWindowFlag = 1;
    if (muteBgmFlag) {
      SetMusicVolume(amenberak, 0);
    } else {
      SetMusicVolume(amenberak, 1);
    }
    UpdateMusicStream(amenberak);
    BeginDrawing();
    ClearBackground(WHITE);

    switch (currentScreen) {
    case SCREEN_MENU:
      mainMenu();
      break;
    case SCREEN_ABOUT:
      aboutScreen();
      break;
    case SCREEN_DRUMSIM:
      jamScreen();
      break;
    case SCREEN_APPINFO:
      appInfoScreen();
      break;
    default:
      currentScreen = SCREEN_MENU;
      break;
    }

    EndDrawing();
  }

  UnloadGlobals();

  CloseAudioDevice();
  CloseWindow();
  return 0;
}
