/**
 * Definisi status global dan inisialisasi sumber daya.
 *
 * Mengimplementasikan InitGlobals() dan UnloadGlobals() untuk memuat/melepas
 * semua sumber daya audio, tekstur, dan suara yang digunakan oleh aplikasi.
 */
#include "global.h"
#include "raylib.h"

GameScreen currentScreen = SCREEN_MENU;
int muteBgmFlag = 0;
int exitWindowFlag = 0;

Music amenberak = {0};
Sound btnAction = {0};
Texture2D background = {0};

Sound sndKick = {0};
Sound sndSnare1 = {0};
Sound sndSnare2 = {0};
Sound sndSnareRoll = {0};
Sound sndHiHatClosed = {0};
Sound sndHiHatOpen = {0};
Sound sndHiTom1 = {0};
Sound sndHiTom2 = {0};
Sound sndLowTom1 = {0};
Sound sndLowTom2 = {0};
Sound sndRide1 = {0};
Sound sndRide2 = {0};
Sound sndRideBell = {0};
Sound sndRideCrash = {0};

Rectangle muteBtn = {scrWidth - 125, scrHeight - 120, 50, 50};
Rectangle startBtn = {(scrWidth / 2) - 100, 100, 200, 80};
Rectangle appInfoBtn = {(scrWidth / 2) - 350, 100, 200, 80};
Rectangle quitBtn = {(scrWidth / 2) + 150, 100, 200, 80};
Rectangle backBtn = {40, 40, 140, 50};
Rectangle aboutMeBtn = {40, scrHeight - 80, 100, 40};

void InitGlobals(void) {
  background = LoadTexture("bg.png");
  amenberak = LoadMusicStream("audio/brikor.wav");
  btnAction = LoadSound("audio/interact.wav");

  sndKick = LoadSound("audio/pearlkit-kick.wav");
  sndSnare1 = LoadSound("audio/pearlkit-snare1.wav");
  sndSnare2 = LoadSound("audio/pearlkit-snare2.wav");
  sndSnareRoll = LoadSound("audio/pearlkit-snareroll.wav");
  sndHiHatClosed = LoadSound("audio/pearlkit-hihat.wav");
  sndHiHatOpen = LoadSound("audio/pearlkit-hihatO.wav");
  sndHiTom1 = LoadSound("audio/pearlkit-hitom1.wav");
  sndHiTom2 = LoadSound("audio/pearlkit-hitom2.wav");
  sndLowTom1 = LoadSound("audio/pearlkit-lowtom1.wav");
  sndLowTom2 = LoadSound("audio/pearlkit-lowtom2.wav");
  sndRide1 = LoadSound("audio/pearlkit-ride1.wav");
  sndRide2 = LoadSound("audio/pearlkit-ride2.wav");
  sndRideBell = LoadSound("audio/pearlkit-ridebell.wav");
  sndRideCrash = LoadSound("audio/pearlkit-ridecrash.wav");

  PlayMusicStream(amenberak);
  SetMusicVolume(amenberak, muteBgmFlag ? 0.0f : 1.0f);
}

void UnloadGlobals(void) {
  UnloadMusicStream(amenberak);
  UnloadSound(btnAction);
  UnloadTexture(background);

  UnloadSound(sndKick);
  UnloadSound(sndSnare1);
  UnloadSound(sndSnare2);
  UnloadSound(sndSnareRoll);
  UnloadSound(sndHiHatClosed);
  UnloadSound(sndHiHatOpen);
  UnloadSound(sndHiTom1);
  UnloadSound(sndHiTom2);
  UnloadSound(sndLowTom1);
  UnloadSound(sndLowTom2);
  UnloadSound(sndRide1);
  UnloadSound(sndRide2);
  UnloadSound(sndRideBell);
  UnloadSound(sndRideCrash);
}
