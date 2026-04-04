/**
 * Layar menu utama - menampilkan tombol navigasi untuk aplikasi.
 */
#include "menuScreen.h"

void mainMenu(void) {
  Vector2 mousePoint = GetMousePosition();

  if (CheckCollisionPointRec(mousePoint, muteBtn)) {
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      switch (muteBgmFlag) {
      case 0:
        muteBgmFlag = !muteBgmFlag;
        break;
      case 1:
        muteBgmFlag = !muteBgmFlag;
        break;
      }
    }
  }

  DrawTexture(background, 0, 0, WHITE);

  DrawRectangleRec(muteBtn, muteBgmFlag ? RED : GRAY);
  DrawRectangleRec(startBtn, WHITE);
  DrawRectangleRec(appInfoBtn, WHITE);
  DrawRectangleRec(quitBtn, RED);

  if (CheckCollisionPointRec(mousePoint, startBtn)) {
    DrawRectangleLines(startBtn.x - 3, startBtn.y - 3, startBtn.width + 7,
                       startBtn.height + 7, WHITE);
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      PlaySound(btnAction);
      currentScreen = SCREEN_DRUMSIM;
    }
  }
  if (CheckCollisionPointRec(mousePoint, appInfoBtn)) {
    DrawRectangleLines(appInfoBtn.x - 3, appInfoBtn.y - 3, appInfoBtn.width + 7,
                       appInfoBtn.height + 7, WHITE);
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      PlaySound(btnAction);
      currentScreen = SCREEN_APPINFO;
    }
  }
  if (CheckCollisionPointRec(mousePoint, quitBtn)) {
    DrawRectangleLines(quitBtn.x - 3, quitBtn.y - 3, quitBtn.width + 7,
                       quitBtn.height + 7, RED);
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      PlaySound(btnAction);
      exitWindowFlag = 1;
    }
  }

  if (GuiButton(aboutMeBtn, "About Me")) {
    PlaySound(btnAction);
    currentScreen = SCREEN_ABOUT;
  }

  DrawText("Exit App", quitBtn.x + 50, quitBtn.y + 30, 24, WHITE);
  DrawText("App Info", appInfoBtn.x + 40, appInfoBtn.y + 30, 24, GRAY);
  DrawText("Start Jamming", (scrWidth / 2) - 80, 130, 24, GRAY);
  DrawText("Click to mute BGM", scrWidth - 200, scrHeight - 50, 20, GRAY);
}
