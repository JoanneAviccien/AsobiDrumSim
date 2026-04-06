/**
 * Layar simulator drum - rendering, animasi, dan penanganan input.
 *
 * Berisi logika rendering kit drum, penanganan input pad drum,
 * animasi not dan stik drum, serta mode demo otomatis.
 */
#ifndef JAMSCREEN_H
#define JAMSCREEN_H
#include "../algo/bresenham.h"
#include "../algo/dda.h"
#include "../algo/midcircle.h"
#include "../algo/midpoint_ellipse.h"
#include "../algo/rect.h"
#include "global.h"
#include "raygui.h"
#include "raylib.h"

#define MAX_KEYBINDS 14
#define MAX_NOTES 50
#define MAX_STICKS 14
#define MAX_SPARKS 120
#define NOTE_ANIM_DURATION 45
#define HIT_FEED_DURATION 10
#define STICK_ANIM_DURATION 20
#define SPARK_LIFETIME 25
#define AMEN_DEMO_BPM 65

typedef struct {
  int x, y;
} Anchor;

typedef struct {
  Vector2 position;
  Vector2 velocity;
  int life;
  int maxLife;
  Color color;
  int active;
} SparkParticle;

typedef struct {
  Vector2 position;
  Vector2 startPos;
  int life;
  int maxLife;
  float rotation;
  Color color;
  int active;
} DrumStick;

typedef struct {
  int active;
  int step;
  int beatTimer;
  int beatsPerStep;
} AmenDemo;

typedef struct {
  float x, y;
  float startY;
  int life;
  int maxLife;
  int noteType;
  Color color;
} NoteAnimation;

typedef struct {
  Rectangle zone;
  int key;
  Sound *sound;
  char keyLabel[10];
  char drumName[20];
  int hitFeedback;
  Vector2 noteOrigin;
} DrumPad;

typedef struct {
  Anchor kick;
  int kickOuterRadius;
  int kickInnerRadius;
  int kickStroke;
  int kickLegLeftDX[2], kickLegLeftDY[2];
  int kickLegRightDX[2], kickLegRightDY[2];

  Anchor tom1;
  int tomWidth;
  int tomHeight;
  int tomStroke;
  int tomRimWidth;
  int tomRimHeight;
  int tomRimStroke;
  int tomLugCount;
  int tomLugWidth;
  int tomLugHeight;
  int tomRimTopDY;
  int tomRimBottomDY;
  int tomBarDX, tomBarDY;

  Anchor tom2;

  Anchor snare;
  int snareWidth;
  int snareHeight;
  int snareStroke;
  int snareLugCount;
  int snareLugHeight;
  int snareLugWidth;
  int snareRimWidth;
  int snareRimHeight;
  int snareRimStroke;
  int snareRimTopDY;
  int snareRimBottomDY;
  Rectangle snareStandRod;
  int snareStandLegLeftDX[2], snareStandLegLeftDY[2];
  int snareStandLegRightDX[2], snareStandLegRightDY[2];

  Anchor floorTom;
  int floorTomWidth;
  int floorTomHeight;
  int floorTomStroke;
  int floorTomRimWidth;
  int floorTomRimHeight;
  int floorTomRimStroke;
  int floorTomLugCount;
  int floorTomLugWidth;
  int floorTomLugHeight;
  int floorTomRimTopDY;
  int floorTomRimBottomDY;
  int floorTomLegCount;
  int floorTomLegDX[3], floorTomLegDY[3];
  int floorTomLegFootDX[3], floorTomLegFootDY[3];

  Anchor rideCymbal;
  int cymbalRx;
  int cymbalRy;
  int cymbalStandRodX, cymbalStandRodDY[2];
  int cymbalBoomArmDX[2], cymbalBoomArmDY[2];
  int cymbalLegLeftDX[2], cymbalLegLeftDY[2];
  int cymbalLegRightDX[2], cymbalLegRightDY[2];

  Anchor hiHat;
  int hiHatTopCX;
  int hiHatTopCY;
  int hiHatRX;
  int hiHatRY;
  int hiHatStandRodX;
  int hiHatStandRodDY[2];
  int hiHatBoomArmDX[2], hiHatBoomArmDY[2];
  int hiHatLegLeftDX[2], hiHatLegLeftDY[2];
  int hiHatLegRightDX[2], hiHatLegRightDY[2];

} DrumKitConfig;

void jamScreen(void);
void CleanupBackgroundTexture(void);
void InitDrumPads(DrumPad *pads, DrumKitConfig *drum);
void UpdateDrumPads(DrumPad *pads, int count);
void SpawnStickAnimation(Vector2 hitPos, int padIndex);
void UpdateStickAnimations(void);
void DrawStickAnimations(void);
void DrawNoteAnimations(void);
void SpawnNoteAnimation(Vector2 origin, const char *drumName);
void InitAmenDemo(int bpm);
void UpdateAmenDemo(DrumPad *pads);
void DrawAmenDemoButton(Rectangle btn);
void SpawnSparkParticles(Vector2 hitPos, Color noteColor);
void UpdateSparkParticles(void);
void DrawSparkParticles(void);

#endif
