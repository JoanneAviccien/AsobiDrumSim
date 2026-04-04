/**
 * Status global, sumber daya bersama, dan definisi tombol UI.
 *
 * Mendeklarasikan dan menginisialisasi semua layar permainan, sumber daya audio,
 * tekstur, dan persegi panjang tombol interaktif yang digunakan di seluruh aplikasi.
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include "raylib.h"

typedef enum GameScreen {
  SCREEN_MENU = 0,
  SCREEN_ABOUT,
  SCREEN_DRUMSIM,
  SCREEN_APPINFO
} GameScreen;

extern GameScreen currentScreen;
void InitGlobals(void);
void UnloadGlobals(void);

extern int muteBgmFlag, exitWindowFlag;
extern Rectangle muteBtn, startBtn, appInfoBtn, backBtn, quitBtn, aboutMeBtn;
extern Texture2D background;
extern Sound btnAction;
extern Music amenberak;

extern Sound sndKick;
extern Sound sndSnare1;
extern Sound sndSnare2;
extern Sound sndSnareRoll;
extern Sound sndHiHatClosed;
extern Sound sndHiHatOpen;
extern Sound sndHiTom1;
extern Sound sndHiTom2;
extern Sound sndLowTom1;
extern Sound sndLowTom2;
extern Sound sndRide1;
extern Sound sndRide2;
extern Sound sndRideBell;
extern Sound sndRideCrash;

#define scrWidth 1280
#define scrHeight 720

#endif
