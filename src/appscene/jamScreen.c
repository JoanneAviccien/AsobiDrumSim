/**
 * Layar simulator drum - implementasi layar jam.
 *
 * Menangani rendering kit drum, input pad drum, animasi not/stik,
 * dan mode demo Amen Break otomatis.
 */
#include "jamScreen.h"
#include "global.h"
#include "raylib.h"
#include <math.h>
#include <pthread.h>
#include <time.h>

static RenderTexture2D backgroundTexture = {0};
static bool backgroundInitialized = false;

static RenderTexture2D drumKitTexture = {0};
static bool drumKitTextureInitialized = false;

static NoteAnimation notes[MAX_NOTES];
static int noteCount = 0;

static DrumStick sticks[MAX_STICKS];

static SparkParticle sparks[MAX_SPARKS];
static int sparkCount = 0;

static CymbalScaleAnimation cymbalScales[MAX_CYMBAL_SCALES];
static int cymbalScaleCount = 0;

static AmenDemo amenDemo = {0};
static int amenDemoActive = 0;

static int metronomeActive = 0;
static int metronomeBpm = 120;
static int metronomeTimeSig = 4;
static int metronomeBeat = 0;
static int metronomePopupOpen = 0;
static pthread_t metronomeThread;
static volatile int metronomeRunning = 0;

static Sound sndMetronomeClick = {0};
static Sound sndMetronomeAccent = {0};

static const int beatKick[16] = {1, 0, 0, 0, 0, 0, 0, 0,
                                 1, 0, 0, 0, 0, 0, 0, 0};
static const int beatSnare[16] = {0, 0, 0, 0, 1, 0, 0, 0,
                                  0, 0, 0, 0, 1, 0, 0, 0};
static const int beatHiHat[16] = {1, 0, 1, 0, 1, 0, 1, 0,
                                  1, 0, 1, 0, 1, 0, 1, 0};

typedef enum { NOTE_SINGLE = 0, NOTE_DOUBLE, NOTE_QUARTER } NoteType;

static Color GetNoteColor(const char *drumName) {
  if (TextIsEqual(drumName, "Kick"))
    return (Color){255, 100, 100, 255};
  if (TextIsEqual(drumName, "Snare") || TextIsEqual(drumName, "Snare Roll") ||
      TextIsEqual(drumName, "Snare 2"))
    return (Color){100, 200, 255, 255};
  if (TextIsEqual(drumName, "Hi-Hat") || TextIsEqual(drumName, "Hi-Hat Open"))
    return (Color){255, 255, 100, 255};
  if (TextIsEqual(drumName, "Hi-Tom 1") || TextIsEqual(drumName, "Hi-Tom 2"))
    return (Color){100, 255, 100, 255};
  if (TextIsEqual(drumName, "Floor Tom") || TextIsEqual(drumName, "Low Tom 2"))
    return (Color){200, 100, 255, 255};
  if (TextIsEqual(drumName, "Ride") || TextIsEqual(drumName, "Ride 2") ||
      TextIsEqual(drumName, "Ride Bell") || TextIsEqual(drumName, "Ride Crash"))
    return (Color){255, 200, 100, 255};
  return WHITE;
}

static void DrawPixelNote(int x, int y, Color color) {
  DrawPixel(x + 2, y + 12, color);
  DrawPixel(x + 3, y + 12, color);
  DrawPixel(x + 4, y + 12, color);
  DrawPixel(x + 5, y + 12, color);

  DrawPixel(x + 1, y + 13, color);
  DrawPixel(x + 2, y + 13, color);
  DrawPixel(x + 3, y + 13, color);
  DrawPixel(x + 4, y + 13, color);
  DrawPixel(x + 5, y + 13, color);
  DrawPixel(x + 6, y + 13, color);

  DrawPixel(x, y + 14, color);
  DrawPixel(x + 1, y + 14, color);
  DrawPixel(x + 2, y + 14, color);
  DrawPixel(x + 3, y + 14, color);
  DrawPixel(x + 4, y + 14, color);
  DrawPixel(x + 5, y + 14, color);
  DrawPixel(x + 6, y + 14, color);
  DrawPixel(x + 7, y + 14, color);

  DrawPixel(x, y + 15, color);
  DrawPixel(x + 1, y + 15, color);
  DrawPixel(x + 2, y + 15, color);
  DrawPixel(x + 3, y + 15, color);
  DrawPixel(x + 4, y + 15, color);
  DrawPixel(x + 5, y + 15, color);
  DrawPixel(x + 6, y + 15, color);
  DrawPixel(x + 7, y + 15, color);

  DrawPixel(x + 1, y + 16, color);
  DrawPixel(x + 2, y + 16, color);
  DrawPixel(x + 3, y + 16, color);
  DrawPixel(x + 4, y + 16, color);
  DrawPixel(x + 5, y + 16, color);
  DrawPixel(x + 6, y + 16, color);

  DrawPixel(x + 2, y + 17, color);
  DrawPixel(x + 3, y + 17, color);
  DrawPixel(x + 4, y + 17, color);
  DrawPixel(x + 5, y + 17, color);

  for (int i = 0; i < 12; i++) {
    DrawPixel(x + 6, y + 1 + i, color);
    DrawPixel(x + 7, y + 1 + i, color);
  }

  DrawPixel(x + 8, y + 1, color);
  DrawPixel(x + 9, y + 1, color);
  DrawPixel(x + 10, y + 2, color);
  DrawPixel(x + 11, y + 2, color);
  DrawPixel(x + 11, y + 3, color);
  DrawPixel(x + 10, y + 4, color);
  DrawPixel(x + 9, y + 5, color);
  DrawPixel(x + 9, y + 6, color);
  DrawPixel(x + 8, y + 6, color);
  DrawPixel(x + 8, y + 5, color);
}

static void DrawPixelDoubleNote(int x, int y, Color color) {
  DrawPixel(x + 2, y + 12, color);
  DrawPixel(x + 3, y + 12, color);
  DrawPixel(x + 4, y + 12, color);
  DrawPixel(x + 5, y + 12, color);

  DrawPixel(x + 1, y + 13, color);
  DrawPixel(x + 2, y + 13, color);
  DrawPixel(x + 3, y + 13, color);
  DrawPixel(x + 4, y + 13, color);
  DrawPixel(x + 5, y + 13, color);
  DrawPixel(x + 6, y + 13, color);

  DrawPixel(x, y + 14, color);
  DrawPixel(x + 1, y + 14, color);
  DrawPixel(x + 2, y + 14, color);
  DrawPixel(x + 3, y + 14, color);
  DrawPixel(x + 4, y + 14, color);
  DrawPixel(x + 5, y + 14, color);
  DrawPixel(x + 6, y + 14, color);
  DrawPixel(x + 7, y + 14, color);

  DrawPixel(x, y + 15, color);
  DrawPixel(x + 1, y + 15, color);
  DrawPixel(x + 2, y + 15, color);
  DrawPixel(x + 3, y + 15, color);
  DrawPixel(x + 4, y + 15, color);
  DrawPixel(x + 5, y + 15, color);
  DrawPixel(x + 6, y + 15, color);
  DrawPixel(x + 7, y + 15, color);

  DrawPixel(x + 1, y + 16, color);
  DrawPixel(x + 2, y + 16, color);
  DrawPixel(x + 3, y + 16, color);
  DrawPixel(x + 4, y + 16, color);
  DrawPixel(x + 5, y + 16, color);
  DrawPixel(x + 6, y + 16, color);

  DrawPixel(x + 2, y + 17, color);
  DrawPixel(x + 3, y + 17, color);
  DrawPixel(x + 4, y + 17, color);
  DrawPixel(x + 5, y + 17, color);

  DrawPixel(x + 14, y + 10, color);
  DrawPixel(x + 15, y + 10, color);
  DrawPixel(x + 16, y + 10, color);
  DrawPixel(x + 17, y + 10, color);

  DrawPixel(x + 13, y + 11, color);
  DrawPixel(x + 14, y + 11, color);
  DrawPixel(x + 15, y + 11, color);
  DrawPixel(x + 16, y + 11, color);
  DrawPixel(x + 17, y + 11, color);
  DrawPixel(x + 18, y + 11, color);

  DrawPixel(x + 12, y + 12, color);
  DrawPixel(x + 13, y + 12, color);
  DrawPixel(x + 14, y + 12, color);
  DrawPixel(x + 15, y + 12, color);
  DrawPixel(x + 16, y + 12, color);
  DrawPixel(x + 17, y + 12, color);
  DrawPixel(x + 18, y + 12, color);
  DrawPixel(x + 19, y + 12, color);

  DrawPixel(x + 12, y + 13, color);
  DrawPixel(x + 13, y + 13, color);
  DrawPixel(x + 14, y + 13, color);
  DrawPixel(x + 15, y + 13, color);
  DrawPixel(x + 16, y + 13, color);
  DrawPixel(x + 17, y + 13, color);
  DrawPixel(x + 18, y + 13, color);
  DrawPixel(x + 19, y + 13, color);

  DrawPixel(x + 13, y + 14, color);
  DrawPixel(x + 14, y + 14, color);
  DrawPixel(x + 15, y + 14, color);
  DrawPixel(x + 16, y + 14, color);
  DrawPixel(x + 17, y + 14, color);
  DrawPixel(x + 18, y + 14, color);

  DrawPixel(x + 14, y + 15, color);
  DrawPixel(x + 15, y + 15, color);
  DrawPixel(x + 16, y + 15, color);
  DrawPixel(x + 17, y + 15, color);

  for (int i = 0; i < 12; i++) {
    DrawPixel(x + 6, y + 1 + i, color);
    DrawPixel(x + 7, y + 1 + i, color);
  }

  for (int i = 0; i < 10; i++) {
    DrawPixel(x + 18, y + 1 + i, color);
    DrawPixel(x + 19, y + 1 + i, color);
  }

  for (int i = 7; i <= 18; i++) {
    DrawPixel(x + i, y + 1, color);
    DrawPixel(x + i, y + 2, color);
    DrawPixel(x + i, y + 3, color);
  }
}

static void DrawPixelQuarterNote(int x, int y, Color color) {
  DrawPixel(x + 2, y + 12, color);
  DrawPixel(x + 3, y + 12, color);
  DrawPixel(x + 4, y + 12, color);
  DrawPixel(x + 5, y + 12, color);

  DrawPixel(x + 1, y + 13, color);
  DrawPixel(x + 2, y + 13, color);
  DrawPixel(x + 3, y + 13, color);
  DrawPixel(x + 4, y + 13, color);
  DrawPixel(x + 5, y + 13, color);
  DrawPixel(x + 6, y + 13, color);

  DrawPixel(x, y + 14, color);
  DrawPixel(x + 1, y + 14, color);
  DrawPixel(x + 2, y + 14, color);
  DrawPixel(x + 3, y + 14, color);
  DrawPixel(x + 4, y + 14, color);
  DrawPixel(x + 5, y + 14, color);
  DrawPixel(x + 6, y + 14, color);
  DrawPixel(x + 7, y + 14, color);

  DrawPixel(x, y + 15, color);
  DrawPixel(x + 1, y + 15, color);
  DrawPixel(x + 2, y + 15, color);
  DrawPixel(x + 3, y + 15, color);
  DrawPixel(x + 4, y + 15, color);
  DrawPixel(x + 5, y + 15, color);
  DrawPixel(x + 6, y + 15, color);
  DrawPixel(x + 7, y + 15, color);

  DrawPixel(x + 1, y + 16, color);
  DrawPixel(x + 2, y + 16, color);
  DrawPixel(x + 3, y + 16, color);
  DrawPixel(x + 4, y + 16, color);
  DrawPixel(x + 5, y + 16, color);
  DrawPixel(x + 6, y + 16, color);

  DrawPixel(x + 2, y + 17, color);
  DrawPixel(x + 3, y + 17, color);
  DrawPixel(x + 4, y + 17, color);
  DrawPixel(x + 5, y + 17, color);

  for (int i = 0; i < 14; i++) {
    DrawPixel(x + 6, y + i, color);
    DrawPixel(x + 7, y + i, color);
  }
}

void SpawnNoteAnimation(Vector2 origin, const char *drumName) {
  if (noteCount >= MAX_NOTES)
    return;

  int idx = noteCount++;
  NoteAnimation *note = &notes[idx];

  note->x = origin.x;
  note->y = origin.y;
  note->startY = origin.y;
  note->life = NOTE_ANIM_DURATION;
  note->maxLife = NOTE_ANIM_DURATION;

  note->noteType = GetRandomValue(0, 2);

  note->color = GetNoteColor(drumName);
}

static void UpdateNoteAnimations(void) {
  for (int i = noteCount - 1; i >= 0; i--) {
    notes[i].life--;

    if (notes[i].life <= 0) {
      noteCount--;
      if (i < noteCount) {
        notes[i] = notes[noteCount];
      }
      continue;
    }

    float progress = 1.0f - (float)notes[i].life / notes[i].maxLife;
    notes[i].y = notes[i].startY - (progress * 130);
    notes[i].x += sinf(progress * 3.14159f * 2) * 0.5f;

    float alpha = (float)notes[i].life / notes[i].maxLife;
    notes[i].color.a = (unsigned char)(alpha * 255);
  }
}

void DrawNoteAnimations(void) {
  for (int i = 0; i < noteCount; i++) {
    if (notes[i].life > 0) {
      switch (notes[i].noteType) {
      case NOTE_SINGLE:
        DrawPixelNote((int)notes[i].x, (int)notes[i].y, notes[i].color);
        break;
      case NOTE_DOUBLE:
        DrawPixelDoubleNote((int)notes[i].x, (int)notes[i].y, notes[i].color);
        break;
      case NOTE_QUARTER:
        DrawPixelQuarterNote((int)notes[i].x, (int)notes[i].y, notes[i].color);
        break;
      }
    }
  }
}

static void DrawDrumstickLine(int x1, int y1, int x2, int y2) {
  Bres_ThickLine(x1, y1, x2, y2, 3, (Color){210, 180, 140, 255});
}

void SpawnStickAnimation(Vector2 hitPos, int padIndex) {
  if (padIndex < 0 || padIndex >= MAX_STICKS)
    return;

  if (padIndex == 0)
    return;

  DrumStick *stick = &sticks[padIndex];

  int yOffset =
      (padIndex == 1 || padIndex == 4 || padIndex == 5 || padIndex == 6) ? 150
                                                                         : 90;

  stick->startPos = hitPos;
  stick->position = (Vector2){hitPos.x + 50, hitPos.y - yOffset};
  stick->life = STICK_ANIM_DURATION;
  stick->maxLife = STICK_ANIM_DURATION;
  stick->rotation = -0.8f;
  stick->color = WHITE;
  stick->active = 1;
}

void SpawnSparkParticles(Vector2 hitPos, Color noteColor) {
  int numSparks = GetRandomValue(5, 8);

  for (int i = 0; i < numSparks && sparkCount < MAX_SPARKS; i++) {
    int idx = sparkCount++;
    SparkParticle *spark = &sparks[idx];

    spark->position = hitPos;

    float angle = (GetRandomValue(0, 360) * 3.14159f) / 180.0f;
    float speed = (float)GetRandomValue(2, 8);
    spark->velocity.x = cosf(angle) * speed;
    spark->velocity.y = sinf(angle) * speed - 2.0f;

    spark->life = SPARK_LIFETIME;
    spark->maxLife = SPARK_LIFETIME;

    float variation = (float)GetRandomValue(0, 100) / 100.0f;
    spark->color = (Color){
        (unsigned char)(noteColor.r * 0.5f + 255 * 0.5f * variation),
        (unsigned char)(noteColor.g * 0.5f + 255 * 0.5f * variation),
        (unsigned char)(noteColor.b * 0.5f + 255 * 0.5f * variation), 255};
    spark->active = 1;
  }
}

void UpdateSparkParticles(void) {
  for (int i = sparkCount - 1; i >= 0; i--) {
    sparks[i].life--;

    if (sparks[i].life <= 0) {
      sparkCount--;
      if (i < sparkCount) {
        sparks[i] = sparks[sparkCount];
      }
      continue;
    }

    sparks[i].position.x += sparks[i].velocity.x;
    sparks[i].position.y += sparks[i].velocity.y;
    sparks[i].velocity.y += 0.3f;
    sparks[i].velocity.x *= 0.98f;

    float alpha = (float)sparks[i].life / sparks[i].maxLife;
    sparks[i].color.a = (unsigned char)(alpha * 255);
  }
}

void DrawSparkParticles(void) {
  for (int i = 0; i < sparkCount; i++) {
    if (sparks[i].active && sparks[i].life > 0) {
      float size = (float)sparks[i].life / sparks[i].maxLife;
      int pixelSize = (int)(size * 3) + 1;

      Color glowOuter = sparks[i].color;
      glowOuter.a = (unsigned char)(size * 80);
      Color glowMid = sparks[i].color;
      glowMid.a = (unsigned char)(size * 160);
      Color glowCore = sparks[i].color;
      glowCore.a = (unsigned char)(size * 255);

      for (int dx = 0; dx < pixelSize; dx++) {
        for (int dy = 0; dy < pixelSize; dy++) {
          int endY = (int)sparks[i].position.y + GetRandomValue(10, 15);
          int startY = (int)sparks[i].position.y + dy;

          Bres_ThickLine((int)sparks[i].position.x, startY,
                         (int)sparks[i].position.x + 14, endY + 2, 4,
                         glowOuter);
          Bres_ThickLine((int)sparks[i].position.x, startY,
                         (int)sparks[i].position.x + 12, endY + 1, 3, glowMid);
          Bres_ThickLine((int)sparks[i].position.x, startY,
                         (int)sparks[i].position.x + 10, endY, 2, glowCore);
        }
      }
    }
  }
}

void SpawnCymbalScaleAnimation(float x, float y, float baseRx, float baseRy) {
  if (cymbalScaleCount >= MAX_CYMBAL_SCALES)
    return;

  int idx = cymbalScaleCount++;
  CymbalScaleAnimation *anim = &cymbalScales[idx];

  anim->x = x;
  anim->y = y;
  anim->baseRx = baseRx;
  anim->baseRy = baseRy;
  anim->life = CYMBAL_SCALE_DURATION;
  anim->maxLife = CYMBAL_SCALE_DURATION;
  anim->scaleAmount = 0.0f;
  anim->active = 1;
}

void UpdateCymbalScaleAnimations(void) {
  for (int i = cymbalScaleCount - 1; i >= 0; i--) {
    cymbalScales[i].life--;

    if (cymbalScales[i].life <= 0) {
      cymbalScaleCount--;
      if (i < cymbalScaleCount) {
        cymbalScales[i] = cymbalScales[cymbalScaleCount];
      }
      continue;
    }

    float progress =
        1.0f - (float)cymbalScales[i].life / cymbalScales[i].maxLife;

    if (progress < 0.3f) {
      float t = progress / 0.3f;
      cymbalScales[i].scaleAmount = t * 1.0f;
    } else if (progress < 0.6f) {
      float t = (progress - 0.3f) / 0.3f;
      cymbalScales[i].scaleAmount = 1.0f - t * 0.3f;
    } else {
      float t = (progress - 0.6f) / 0.4f;
      cymbalScales[i].scaleAmount = 0.7f - t * 0.7f;
    }
  }
}

void DrawCymbalScaleAnimations(void) {
  for (int i = 0; i < cymbalScaleCount; i++) {
    if (!cymbalScales[i].active || cymbalScales[i].life <= 0)
      continue;

    float rx = cymbalScales[i].baseRx +
               (cymbalScales[i].baseRx * cymbalScales[i].scaleAmount * 0.15f);
    float ry = cymbalScales[i].baseRy +
               (cymbalScales[i].baseRy * cymbalScales[i].scaleAmount * 0.15f);

    float alpha = (float)cymbalScales[i].life / cymbalScales[i].maxLife;
    Color animColor = WHITE;
    animColor.a = (unsigned char)(alpha * 200);

    MidpointEllipseThick((int)cymbalScales[i].x, (int)cymbalScales[i].y,
                         (int)rx, (int)ry, 3, animColor);
  }
}

static void *MetronomeThreadFunc(void *arg) {
  (void)arg;
  while (metronomeRunning) {
    if (metronomeActive) {
      if (metronomeBeat == 0 && sndMetronomeAccent.stream.buffer != NULL) {
        PlaySound(sndMetronomeAccent);
      } else if (sndMetronomeClick.stream.buffer != NULL) {
        PlaySound(sndMetronomeClick);
      }

      metronomeBeat = (metronomeBeat + 1) % metronomeTimeSig;

      int msPerBeat = 60000 / metronomeBpm;
      struct timespec ts;
      ts.tv_sec = msPerBeat / 1000;
      ts.tv_nsec = (msPerBeat % 1000) * 1000000L;
      nanosleep(&ts, NULL);
    } else {
      struct timespec ts;
      ts.tv_sec = 0;
      ts.tv_nsec = 50000000L;
      nanosleep(&ts, NULL);
    }
  }
  return NULL;
}

static void InitMetronomeSounds(void) {
  sndMetronomeClick = LoadSound("audio/metronomeclick.wav");
  sndMetronomeAccent = LoadSound("audio/metronomeclick.wav");

  SetSoundVolume(sndMetronomeClick, 0.5f);
  SetSoundVolume(sndMetronomeAccent, 0.8f);
}

static void UnloadMetronomeSounds(void) {
  if (sndMetronomeClick.stream.buffer != NULL) {
    UnloadSound(sndMetronomeClick);
    sndMetronomeClick = (Sound){0};
  }
  if (sndMetronomeAccent.stream.buffer != NULL) {
    UnloadSound(sndMetronomeAccent);
    sndMetronomeAccent = (Sound){0};
  }
}

static void StartMetronomeThread(void) {
  if (!metronomeRunning) {
    metronomeRunning = 1;
    pthread_create(&metronomeThread, NULL, MetronomeThreadFunc, NULL);
    pthread_detach(metronomeThread);
  }
}

static void StopMetronomeThread(void) {
  metronomeRunning = 0;
  pthread_join(metronomeThread, NULL);
}

static void DrawMetronomeButton(void) {
  Rectangle metBtn = {20, scrHeight - 60, 140, 40};

  Color btnColor = metronomeActive ? GREEN : (Color){100, 100, 100, 255};
  DrawRectangleRec(metBtn, btnColor);
  DrawRectangleLinesEx(metBtn, 2, WHITE);

  const char *btnText = metronomeActive ? "METRO ON" : "METRO OFF";
  int textWidth = MeasureText(btnText, 14);
  DrawText(btnText, (int)(metBtn.x + (metBtn.width - textWidth) / 2),
           (int)(metBtn.y + (metBtn.height - 14) / 2), 14, WHITE);

  if (CheckCollisionPointRec(GetMousePosition(), metBtn) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (!metronomeActive) {
      metronomeActive = 1;
      metronomeBeat = 0;
      if (!sndMetronomeClick.stream.buffer) {
        InitMetronomeSounds();
      }
      StartMetronomeThread();
    } else {
      metronomeActive = 0;
    }
    PlaySound(btnAction);
  }

  if (CheckCollisionPointRec(GetMousePosition(), metBtn) &&
      IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    metronomePopupOpen = !metronomePopupOpen;
    PlaySound(btnAction);
  }
}

static void DrawMetronomePopup(void) {
  if (!metronomePopupOpen)
    return;

  int popupW = 280;
  int popupH = 180;
  int popupX = 20;
  int popupY = scrHeight - 60 - popupH - 10;

  DrawRectangle(popupX, popupY, popupW, popupH, (Color){30, 30, 30, 230});
  DrawRectangleLines(popupX, popupY, popupW, popupH, WHITE);

  DrawText("METRONOME SETTINGS", popupX + 40, popupY + 10, 14,
           (Color){255, 200, 50, 255});

  int labelY = popupY + 45;
  int sliderY = popupY + 42;
  int valueY = popupY + 45;

  DrawText("BPM:", popupX + 15, labelY, 12, WHITE);

  float bpmFloat = (float)metronomeBpm;
  if (GuiSlider((Rectangle){popupX + 60, sliderY, 130, 16}, NULL, NULL,
                &bpmFloat, 30.0f, 300.0f)) {
    metronomeBpm = (int)bpmFloat;
    if (metronomeBpm < 30)
      metronomeBpm = 30;
  }
  DrawText(TextFormat("%d", metronomeBpm), popupX + 200, valueY, 12, YELLOW);

  DrawText("TIME SIG:", popupX + 15, labelY + 40, 12, WHITE);

  float sigFloat = (float)metronomeTimeSig;
  if (GuiSlider((Rectangle){popupX + 90, sliderY + 38, 100, 16}, NULL, NULL,
                &sigFloat, 2.0f, 8.0f)) {
    metronomeTimeSig = (int)sigFloat;
    if (metronomeTimeSig < 2)
      metronomeTimeSig = 2;
  }
  DrawText(TextFormat("%d/%d", metronomeTimeSig, 4), popupX + 200, valueY + 40,
           12, YELLOW);

  Rectangle closeBtn = {popupX + popupW - 50, popupY + 5, 40, 20};
  if (GuiButton(closeBtn, "X")) {
    metronomePopupOpen = 0;
    PlaySound(btnAction);
  }

  if (metronomeActive) {
    int indicatorY = popupY + popupH - 30;
    Color indicatorColor = (metronomeBeat == 0) ? RED : GREEN;
    DrawCircle(popupX + 30, indicatorY, 6, indicatorColor);
    DrawText(TextFormat("Beat: %d", metronomeBeat + 1), popupX + 50,
             indicatorY - 6, 12, WHITE);
  }
}

void UpdateStickAnimations(void) {
  for (int i = 0; i < MAX_STICKS; i++) {
    if (!sticks[i].active)
      continue;

    sticks[i].life--;

    if (sticks[i].life <= 0) {
      sticks[i].active = 0;
      continue;
    }

    float progress = 1.0f - (float)sticks[i].life / sticks[i].maxLife;

    float hitX = sticks[i].startPos.x;
    float hitY = sticks[i].startPos.y;

    int yOffset = (i == 1 || i == 4 || i == 5 || i == 6) ? 150 : 90;

    float startX = hitX + 50;
    float startY = hitY - yOffset;

    if (progress < 0.35f) {
      float t = progress / 0.35f;
      sticks[i].position.x = startX + (t * (hitX - startX));
      sticks[i].position.y = startY + (t * (hitY - startY));
      sticks[i].rotation = -0.8f + (t * 1.6f);
    } else if (progress < 0.5f) {
      sticks[i].position.x = hitX;
      sticks[i].position.y = hitY;
      sticks[i].rotation = 0.8f;
    } else {
      float t = (progress - 0.5f) / 0.5f;
      sticks[i].position.x = hitX + (t * (startX - hitX));
      sticks[i].position.y = hitY + (t * (startY - hitY));
      sticks[i].rotation = 0.8f - (t * 1.6f);
    }
  }
}

void DrawStickAnimations(void) {
  for (int i = 0; i < MAX_STICKS; i++) {
    if (sticks[i].active) {
      float pivotX = sticks[i].position.x - (cosf(sticks[i].rotation) * 35);
      float pivotY = sticks[i].position.y - (sinf(sticks[i].rotation) * 35);

      DrawDrumstickLine((int)pivotX, (int)pivotY, (int)sticks[i].position.x,
                        (int)sticks[i].position.y);
    }
  }
}

static void TriggerDrumHit(DrumPad *pads, int padIndex) {
  if (padIndex < 0 || padIndex >= MAX_KEYBINDS)
    return;

  PlaySound(*pads[padIndex].sound);
  pads[padIndex].hitFeedback = HIT_FEED_DURATION;

  Vector2 noteOrigin = {pads[padIndex].zone.x + pads[padIndex].zone.width / 2,
                        pads[padIndex].zone.y + pads[padIndex].zone.height / 2};

  if (padIndex == 1 || padIndex == 4 || padIndex == 5 || padIndex == 6 ||
      padIndex == 11 || padIndex == 12 || padIndex == 13) {
    noteOrigin.y = pads[padIndex].zone.y + 10;
  }

  Color noteColor = GetNoteColor(pads[padIndex].drumName);
  SpawnNoteAnimation(noteOrigin, pads[padIndex].drumName);

  Vector2 stickPos = {noteOrigin.x, noteOrigin.y};
  SpawnSparkParticles(stickPos, noteColor);
  SpawnStickAnimation(stickPos, padIndex);

  if (padIndex == 2 || padIndex == 3) {
    SpawnCymbalScaleAnimation(162, 184, 100, 15);
  }

  if (padIndex >= 7 && padIndex <= 10) {
    SpawnCymbalScaleAnimation(955, 155, 100, 15);
  }
}

static int FindPadIndexByName(DrumPad *pads, const char *name) {
  for (int i = 0; i < MAX_KEYBINDS; i++) {
    if (TextIsEqual(pads[i].drumName, name))
      return i;
  }
  return -1;
}

void InitAmenDemo(int bpm) {
  amenDemo.active = 1;
  amenDemo.step = 0;
  amenDemo.beatTimer = 0;
  amenDemo.beatsPerStep = (60 * 60) / (bpm * 4);
}

void UpdateAmenDemo(DrumPad *pads) {
  if (!amenDemo.active)
    return;

  amenDemo.beatTimer++;

  if (amenDemo.beatTimer >= amenDemo.beatsPerStep) {
    amenDemo.beatTimer = 0;

    int kickIdx = FindPadIndexByName(pads, "Kick");
    int snareIdx = FindPadIndexByName(pads, "Snare");
    int hihatIdx = FindPadIndexByName(pads, "Hi-Hat");

    if (beatKick[amenDemo.step] && kickIdx >= 0)
      TriggerDrumHit(pads, kickIdx);

    if (beatSnare[amenDemo.step] && snareIdx >= 0)
      TriggerDrumHit(pads, snareIdx);

    if (beatHiHat[amenDemo.step] && hihatIdx >= 0)
      TriggerDrumHit(pads, hihatIdx);

    amenDemo.step = (amenDemo.step + 1) % 16;
  }
}

void DrawAmenDemoButton(Rectangle btn) {
  Color btnColor = amenDemoActive ? GREEN : (Color){100, 100, 100, 255};
  DrawRectangleRec(btn, btnColor);
  DrawRectangleLinesEx(btn, 2, WHITE);

  const char *btnText = amenDemoActive ? "STOP DEMO" : "DRUM DEMO";
  int textWidth = MeasureText(btnText, 16);
  DrawText(btnText, (int)(btn.x + (btn.width - textWidth) / 2),
           (int)(btn.y + (btn.height - 16) / 2), 16, WHITE);

  if (CheckCollisionPointRec(GetMousePosition(), btn) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    amenDemoActive = !amenDemoActive;
    if (amenDemoActive) {
      InitAmenDemo(AMEN_DEMO_BPM);
    } else {
      amenDemo.active = 0;
    }
    PlaySound(btnAction);
  }
}

void InitDrumPads(DrumPad *pads, DrumKitConfig *drum) {
  int idx = 0;

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->kick.x - drum->kickOuterRadius,
                          drum->kick.y - drum->kickOuterRadius,
                          drum->kickOuterRadius * 2, drum->kickOuterRadius * 2},
      .key = KEY_SPACE,
      .sound = &sndKick,
      .keyLabel = "SPACE",
      .drumName = "Kick",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->snare.x - 10, drum->snare.y - 10,
                          drum->snareWidth + 20, drum->snareHeight + 20},
      .key = KEY_J,
      .sound = &sndSnare1,
      .keyLabel = "J",
      .drumName = "Snare",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->hiHatTopCX - drum->hiHatRX - 10,
                          drum->hiHatTopCY - drum->hiHatRY - 10,
                          drum->hiHatRX * 2 + 20, drum->hiHatRY * 2 + 30},
      .key = KEY_D,
      .sound = &sndHiHatClosed,
      .keyLabel = "D",
      .drumName = "Hi-Hat",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->hiHatTopCX - drum->hiHatRX - 10,
                          drum->hiHatTopCY - drum->hiHatRY - 10,
                          drum->hiHatRX * 2 + 20, drum->hiHatRY * 2 + 30},
      .key = KEY_F,
      .sound = &sndHiHatOpen,
      .keyLabel = "F",
      .drumName = "Hi-Hat Open",
      .hitFeedback = 0};

  pads[idx++] =
      (DrumPad){.zone = (Rectangle){drum->tom1.x - 10, drum->tom1.y - 10,
                                    drum->tomWidth + 20, drum->tomHeight + 20},
                .key = KEY_K,
                .sound = &sndHiTom1,
                .keyLabel = "K",
                .drumName = "Hi-Tom 1",
                .hitFeedback = 0};

  pads[idx++] =
      (DrumPad){.zone = (Rectangle){drum->tom2.x - 10, drum->tom2.y - 10,
                                    drum->tomWidth + 20, drum->tomHeight + 20},
                .key = KEY_L,
                .sound = &sndHiTom2,
                .keyLabel = "L",
                .drumName = "Hi-Tom 2",
                .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->floorTom.x - 10, drum->floorTom.y - 10,
                          drum->floorTomWidth + 20, drum->floorTomHeight + 20},
      .key = KEY_H,
      .sound = &sndLowTom1,
      .keyLabel = "H",
      .drumName = "Floor Tom",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->rideCymbal.x - drum->cymbalRx - 10,
                          drum->rideCymbal.y - drum->cymbalRy - 10,
                          drum->cymbalRx * 2 + 20, drum->cymbalRy * 2 + 20},
      .key = KEY_U,
      .sound = &sndRide1,
      .keyLabel = "U",
      .drumName = "Ride",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->rideCymbal.x - drum->cymbalRx - 10,
                          drum->rideCymbal.y - drum->cymbalRy - 10,
                          drum->cymbalRx * 2 + 20, drum->cymbalRy * 2 + 20},
      .key = KEY_I,
      .sound = &sndRide2,
      .keyLabel = "I",
      .drumName = "Ride 2",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->rideCymbal.x - drum->cymbalRx - 10,
                          drum->rideCymbal.y - drum->cymbalRy - 10,
                          drum->cymbalRx * 2 + 20, drum->cymbalRy * 2 + 20},
      .key = KEY_O,
      .sound = &sndRideBell,
      .keyLabel = "O",
      .drumName = "Ride Bell",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->rideCymbal.x - drum->cymbalRx - 10,
                          drum->rideCymbal.y - drum->cymbalRy - 10,
                          drum->cymbalRx * 2 + 20, drum->cymbalRy * 2 + 20},
      .key = KEY_P,
      .sound = &sndRideCrash,
      .keyLabel = "P",
      .drumName = "Ride Crash",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->snare.x - 10, drum->snare.y - 10,
                          drum->snareWidth + 20, drum->snareHeight + 20},
      .key = KEY_N,
      .sound = &sndSnareRoll,
      .keyLabel = "N",
      .drumName = "Snare Roll",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->snare.x - 10, drum->snare.y - 10,
                          drum->snareWidth + 20, drum->snareHeight + 20},
      .key = KEY_M,
      .sound = &sndSnare2,
      .keyLabel = "M",
      .drumName = "Snare 2",
      .hitFeedback = 0};

  pads[idx++] = (DrumPad){
      .zone = (Rectangle){drum->floorTom.x - 10, drum->floorTom.y - 10,
                          drum->floorTomWidth + 20, drum->floorTomHeight + 20},
      .key = KEY_B,
      .sound = &sndLowTom2,
      .keyLabel = "B",
      .drumName = "Low Tom 2",
      .hitFeedback = 0};
}

void UpdateDrumPads(DrumPad *pads, int count) {
  for (int i = 0; i < count; i++) {
    if (pads[i].hitFeedback > 0) {
      pads[i].hitFeedback--;
    }

    if (IsKeyPressed(pads[i].key)) {
      PlaySound(*pads[i].sound);
      pads[i].hitFeedback = HIT_FEED_DURATION;

      Vector2 noteOrigin = {pads[i].zone.x + pads[i].zone.width / 2,
                            pads[i].zone.y + pads[i].zone.height / 2};

      if (i == 1 || i == 4 || i == 5 || i == 6 || i == 11 || i == 12 ||
          i == 13) {
        noteOrigin.y = pads[i].zone.y + 10;
      }

      Color noteColor = GetNoteColor(pads[i].drumName);
      SpawnNoteAnimation(noteOrigin, pads[i].drumName);

      Vector2 stickPos = {noteOrigin.x, noteOrigin.y};
      SpawnSparkParticles(stickPos, noteColor);
      SpawnStickAnimation(stickPos, i);

      if (i == 2 || i == 3) {
        SpawnCymbalScaleAnimation(162, 184, 100, 15);
      }

      if (i >= 7 && i <= 10) {
        SpawnCymbalScaleAnimation(955, 155, 100, 15);
      }
    }
  }

  UpdateNoteAnimations();
  UpdateStickAnimations();
  UpdateSparkParticles();
  UpdateCymbalScaleAnimations();
}

void drawStudioBackground(void) {
  int vanishingX = scrWidth / 2;
  int vanishingY = scrHeight / 2;
  for (int i = 0; i <= scrWidth; i += 80) {
    Bres_ThickLine(vanishingX, vanishingY, i, scrHeight, 2,
                   (Color){80, 80, 90, 200});
  }

  for (int i = 0; i < scrHeight / 2; i += 30) {
    int y = scrHeight - i;
    DDA_ThickLine(0, y, scrWidth, y, 2, (Color){70, 70, 80, 180});
  }
}

static void initBackgroundTexture(void) {
  backgroundTexture = LoadRenderTexture(scrWidth, scrHeight);
  backgroundInitialized = true;

  BeginTextureMode(backgroundTexture);
  ClearBackground(BLACK);
  drawStudioBackground();
  EndTextureMode();
}

void CleanupBackgroundTexture(void) {
  if (backgroundInitialized) {
    UnloadRenderTexture(backgroundTexture);
    backgroundTexture = (RenderTexture2D){0};
    backgroundInitialized = false;
  }

  if (drumKitTextureInitialized) {
    UnloadRenderTexture(drumKitTexture);
    drumKitTexture = (RenderTexture2D){0};
    drumKitTextureInitialized = false;
  }
}

static void renderDrumKitToTexture(DrumKitConfig *mainDrum) {
  if (drumKitTextureInitialized) {
    UnloadRenderTexture(drumKitTexture);
  }

  drumKitTexture = LoadRenderTexture(scrWidth, scrHeight);
  drumKitTextureInitialized = true;

  BeginTextureMode(drumKitTexture);
  ClearBackground(BLANK);

  int kickLegLeftX1 = mainDrum->kick.x + mainDrum->kickLegLeftDX[0];
  int kickLegLeftY1 = mainDrum->kick.y + mainDrum->kickLegLeftDY[0];
  int kickLegLeftX2 = mainDrum->kick.x + mainDrum->kickLegLeftDX[1];
  int kickLegLeftY2 = mainDrum->kick.y + mainDrum->kickLegLeftDY[1];

  int kickLegRightX1 = mainDrum->kick.x + mainDrum->kickLegRightDX[0];
  int kickLegRightY1 = mainDrum->kick.y + mainDrum->kickLegRightDY[0];
  int kickLegRightX2 = mainDrum->kick.x + mainDrum->kickLegRightDX[1];
  int kickLegRightY2 = mainDrum->kick.y + mainDrum->kickLegRightDY[1];

  Bres_ThickLine(kickLegLeftX1, kickLegLeftY1, kickLegLeftX2, kickLegLeftY2,
                 mainDrum->kickStroke + 5, WHITE);
  Bres_ThickLine(kickLegRightX1, kickLegRightY1, kickLegRightX2, kickLegRightY2,
                 mainDrum->kickStroke + 5, WHITE);
  MidcircleFilled(mainDrum->kick.x, mainDrum->kick.y,
                  mainDrum->kickOuterRadius - mainDrum->kickStroke, BLACK);
  MidcircleThick(mainDrum->kick.x, mainDrum->kick.y, mainDrum->kickOuterRadius,
                 mainDrum->kickStroke, WHITE);
  MidcircleFilled(mainDrum->kick.x, mainDrum->kick.y, mainDrum->kickInnerRadius,
                  WHITE);

  filledRect(mainDrum->floorTom.x + mainDrum->floorTomStroke,
             mainDrum->floorTom.y + mainDrum->floorTomStroke,
             mainDrum->floorTomWidth - mainDrum->floorTomStroke * 2,
             mainDrum->floorTomHeight - mainDrum->floorTomStroke * 2, BLACK);
  thickHollowRect(mainDrum->floorTom.x, mainDrum->floorTom.y,
                  mainDrum->floorTomWidth, mainDrum->floorTomHeight,
                  mainDrum->floorTomStroke, WHITE);
  thickHollowRect(mainDrum->floorTom.x - 5,
                  mainDrum->floorTom.y + mainDrum->floorTomRimBottomDY,
                  mainDrum->floorTomRimWidth, mainDrum->floorTomRimHeight,
                  mainDrum->floorTomRimStroke, WHITE);
  thickHollowRect(mainDrum->floorTom.x - 5,
                  mainDrum->floorTom.y + mainDrum->floorTomRimTopDY,
                  mainDrum->floorTomRimWidth, mainDrum->floorTomRimHeight,
                  mainDrum->floorTomRimStroke, WHITE);

  int ftLugSpacing = mainDrum->floorTomWidth / (mainDrum->floorTomLugCount - 1);
  for (int i = 0; i < mainDrum->floorTomLugCount; i++) {
    int lugX = mainDrum->floorTom.x + (i * ftLugSpacing);
    Bres_ThickLine(lugX, mainDrum->floorTom.y + mainDrum->floorTomRimTopDY - 5,
                   lugX,
                   mainDrum->floorTom.y + mainDrum->floorTomRimTopDY +
                       mainDrum->floorTomLugHeight,
                   2, WHITE);
    Bres_ThickLine(lugX,
                   mainDrum->floorTom.y + mainDrum->floorTomRimBottomDY -
                       mainDrum->floorTomLugHeight,
                   lugX,
                   mainDrum->floorTom.y + mainDrum->floorTomRimBottomDY + 5, 2,
                   WHITE);
  }

  int ftStandCenterX = mainDrum->floorTom.x + (mainDrum->floorTomWidth / 2);
  int ftStandTopY = mainDrum->floorTom.y + mainDrum->floorTomRimBottomDY +
                    mainDrum->floorTomRimHeight;

  for (int i = 0; i < mainDrum->floorTomLegCount; i++) {
    int legStartX = mainDrum->floorTom.x + (i * mainDrum->floorTomWidth / 2);
    int legStartY = ftStandTopY;

    int legMidX = legStartX + mainDrum->floorTomLegDX[i] / 2;
    int legMidY = legStartY + mainDrum->floorTomLegDY[i] / 2;

    int legEndX = ftStandCenterX + mainDrum->floorTomLegFootDX[i];
    int legEndY = ftStandTopY + mainDrum->floorTomLegFootDY[i];

    Bres_ThickLine(legStartX, legStartY, legMidX, legMidY, 5, WHITE);
    Bres_ThickLine(legMidX, legMidY, legEndX, legEndY, 5, WHITE);
  }

  filledRect(mainDrum->tom1.x + mainDrum->tomStroke,
             mainDrum->tom1.y + mainDrum->tomStroke,
             mainDrum->tomWidth - mainDrum->tomStroke * 2,
             mainDrum->tomHeight - mainDrum->tomStroke * 2, BLACK);
  thickHollowRect(mainDrum->tom1.x, mainDrum->tom1.y, mainDrum->tomWidth,
                  mainDrum->tomHeight, mainDrum->tomStroke, WHITE);
  thickHollowRect(mainDrum->tom1.x - 5,
                  mainDrum->tom1.y + mainDrum->tomRimBottomDY,
                  mainDrum->tomRimWidth, mainDrum->tomRimHeight,
                  mainDrum->tomRimStroke, WHITE);
  thickHollowRect(mainDrum->tom1.x - 5,
                  mainDrum->tom1.y + mainDrum->tomRimTopDY,
                  mainDrum->tomRimWidth, mainDrum->tomRimHeight,
                  mainDrum->tomRimStroke, WHITE);

  filledRect(mainDrum->tom2.x + mainDrum->tomStroke,
             mainDrum->tom2.y + mainDrum->tomStroke,
             mainDrum->tomWidth - mainDrum->tomStroke * 2,
             mainDrum->tomHeight - mainDrum->tomStroke * 2, BLACK);
  thickHollowRect(mainDrum->tom2.x, mainDrum->tom2.y, mainDrum->tomWidth,
                  mainDrum->tomHeight, mainDrum->tomStroke, WHITE);
  thickHollowRect(mainDrum->tom2.x - 5,
                  mainDrum->tom2.y + mainDrum->tomRimBottomDY,
                  mainDrum->tomRimWidth, mainDrum->tomRimHeight,
                  mainDrum->tomRimStroke, WHITE);
  thickHollowRect(mainDrum->tom2.x - 5,
                  mainDrum->tom2.y + mainDrum->tomRimTopDY,
                  mainDrum->tomRimWidth, mainDrum->tomRimHeight,
                  mainDrum->tomRimStroke, WHITE);

  int tom1LugSpacing = mainDrum->tomWidth / (mainDrum->tomLugCount - 1);
  for (int i = 0; i < mainDrum->tomLugCount; i++) {
    int lugX = mainDrum->tom1.x + (i * tom1LugSpacing);
    Bres_ThickLine(lugX, mainDrum->tom1.y + mainDrum->tomRimTopDY - 5, lugX,
                   mainDrum->tom1.y + mainDrum->tomRimTopDY +
                       mainDrum->tomLugHeight,
                   2, WHITE);
    Bres_ThickLine(
        lugX,
        mainDrum->tom1.y + mainDrum->tomRimBottomDY - mainDrum->tomLugHeight,
        lugX, mainDrum->tom1.y + mainDrum->tomRimBottomDY + 5, 2, WHITE);
  }

  int tom2LugSpacing = mainDrum->tomWidth / (mainDrum->tomLugCount - 1);
  for (int i = 0; i < mainDrum->tomLugCount; i++) {
    int lugX = mainDrum->tom2.x + (i * tom2LugSpacing);
    Bres_ThickLine(lugX, mainDrum->tom2.y + mainDrum->tomRimTopDY - 5, lugX,
                   mainDrum->tom2.y + mainDrum->tomRimTopDY +
                       mainDrum->tomLugHeight,
                   2, WHITE);
    Bres_ThickLine(
        lugX,
        mainDrum->tom2.y + mainDrum->tomRimBottomDY - mainDrum->tomLugHeight,
        lugX, mainDrum->tom2.y + mainDrum->tomRimBottomDY + 5, 2, WHITE);
  }
  int tomBarY = mainDrum->tom1.y + mainDrum->tomBarDY;
  int tomBarXStart = mainDrum->tom1.x + mainDrum->tomBarDX;
  Bres_ThickLine(tomBarXStart, tomBarY, mainDrum->tom2.x, tomBarY,
                 mainDrum->tomStroke, WHITE);
  Bres_ThickLine(tomBarXStart, tomBarY + 20, mainDrum->tom2.x - 24,
                 tomBarY + 20, mainDrum->tomStroke, WHITE);
  Bres_ThickLine(mainDrum->tom2.x, tomBarY + 20, mainDrum->tom2.x - 6,
                 tomBarY + 20, mainDrum->tomStroke, WHITE);
  Bres_ThickLine(mainDrum->tom2.x - 24, tomBarY + 20, mainDrum->tom2.x - 24,
                 tomBarY + 70, mainDrum->tomStroke, WHITE);
  Bres_ThickLine(mainDrum->tom2.x - 6, tomBarY + 20, mainDrum->tom2.x - 6,
                 tomBarY + 70, mainDrum->tomStroke, WHITE);

  filledRect(mainDrum->snare.x + mainDrum->snareStroke,
             mainDrum->snare.y + mainDrum->snareStroke,
             mainDrum->snareWidth - mainDrum->snareStroke * 2,
             mainDrum->snareHeight - mainDrum->snareStroke * 2, BLACK);
  thickHollowRect(mainDrum->snare.x, mainDrum->snare.y, mainDrum->snareWidth,
                  mainDrum->snareHeight, 3, WHITE);
  thickHollowRect(mainDrum->snare.x - 5,
                  mainDrum->snare.y + mainDrum->snareRimTopDY,
                  mainDrum->snareRimWidth, mainDrum->snareRimHeight,
                  mainDrum->snareRimStroke, WHITE);
  thickHollowRect(mainDrum->snare.x - 5,
                  mainDrum->snare.y + mainDrum->snareRimBottomDY,
                  mainDrum->snareRimWidth, mainDrum->snareRimHeight,
                  mainDrum->snareRimStroke, WHITE);

  int snareLugSpacing = mainDrum->snareWidth / 5;
  for (int i = 0; i < 6; i++) {
    int lugX = mainDrum->snare.x + (i * snareLugSpacing);
    int lugTopY =
        mainDrum->snare.y + mainDrum->snareRimTopDY + mainDrum->snareRimHeight;
    int lugBottomY = mainDrum->snare.y + mainDrum->snareRimBottomDY -
                     mainDrum->snareRimHeight;

    thickHollowRect(lugX - 4, lugTopY, 8, lugBottomY - lugTopY + 5, 2, WHITE);

    Bres_ThickLine(lugX, lugTopY + 3, lugX, lugBottomY, 2, LIGHTGRAY);
  }

  int snareStandCenterX = mainDrum->snare.x + (mainDrum->snareWidth / 2);
  int snareStandTopY =
      mainDrum->snare.y + mainDrum->snareRimBottomDY + mainDrum->snareRimHeight;

  Bres_ThickLine(snareStandCenterX, snareStandTopY, snareStandCenterX,
                 snareStandTopY + mainDrum->snareStandRod.height,
                 mainDrum->snareStandRod.width, WHITE);

  int snareLegLeftX1 = snareStandCenterX + mainDrum->snareStandLegLeftDX[1];
  int snareLegLeftY1 = snareStandTopY + mainDrum->snareStandLegLeftDY[1];
  int snareLegLeftX2 = snareStandCenterX + mainDrum->snareStandLegLeftDX[0];
  int snareLegLeftY2 = snareStandTopY + mainDrum->snareStandLegLeftDY[0];
  Bres_ThickLine(snareLegLeftX1, snareLegLeftY1, snareLegLeftX2, snareLegLeftY2,
                 4, WHITE);

  int snareLegRightX1 = snareStandCenterX + mainDrum->snareStandLegRightDX[1];
  int snareLegRightY1 = snareStandTopY + mainDrum->snareStandLegRightDY[1];
  int snareLegRightX2 = snareStandCenterX + mainDrum->snareStandLegRightDX[0];
  int snareLegRightY2 = snareStandTopY + mainDrum->snareStandLegRightDY[0];
  Bres_ThickLine(snareLegRightX1, snareLegRightY1, snareLegRightX2,
                 snareLegRightY2, 4, WHITE);

  Bres_ThickLine(snareLegLeftX2 - 10, snareLegLeftY2, snareLegLeftX2 + 10,
                 snareLegLeftY2, 3, WHITE);
  Bres_ThickLine(snareLegRightX2 - 10, snareLegRightY2, snareLegRightX2 + 10,
                 snareLegRightY2, 3, WHITE);

  Bres_ThickLine(mainDrum->cymbalStandRodX, mainDrum->cymbalStandRodDY[0],
                 mainDrum->cymbalStandRodX, mainDrum->cymbalStandRodDY[1], 5,
                 WHITE);

  Bres_ThickLine(mainDrum->cymbalLegLeftDX[0], mainDrum->cymbalLegLeftDY[0],
                 mainDrum->cymbalLegLeftDX[1], mainDrum->cymbalLegLeftDY[1], 5,
                 WHITE);

  Bres_ThickLine(mainDrum->cymbalLegRightDX[0], mainDrum->cymbalLegRightDY[0],
                 mainDrum->cymbalLegRightDX[1], mainDrum->cymbalLegRightDY[1],
                 5, WHITE);

  Bres_ThickLine(mainDrum->cymbalBoomArmDX[0], mainDrum->cymbalBoomArmDY[0],
                 mainDrum->cymbalBoomArmDX[1], mainDrum->cymbalBoomArmDY[1], 5,
                 WHITE);

  MidpointEllipseThick(mainDrum->rideCymbal.x, mainDrum->rideCymbal.y,
                       mainDrum->cymbalRx, mainDrum->cymbalRy, 3, WHITE);

  MidpointHalfEllipse(mainDrum->hiHatTopCX, mainDrum->hiHatTopCY,
                      mainDrum->hiHatRX, mainDrum->hiHatRY, HALF_ELLIPSE_TOP,
                      WHITE);
  MidpointHalfEllipse(mainDrum->hiHatTopCX, mainDrum->hiHatTopCY + 10,
                      mainDrum->hiHatRX, mainDrum->hiHatRY, HALF_ELLIPSE_BOTTOM,
                      WHITE);
  MidpointHalfEllipse(mainDrum->hiHatTopCX, mainDrum->hiHatTopCY,
                      mainDrum->hiHatRX, mainDrum->hiHatRY, HALF_ELLIPSE_TOP,
                      WHITE);
  MidpointHalfEllipse(mainDrum->hiHatTopCX, mainDrum->hiHatTopCY + 10,
                      mainDrum->hiHatRX, mainDrum->hiHatRY, HALF_ELLIPSE_BOTTOM,
                      WHITE);
  BresenhamLine(mainDrum->hiHatTopCX - mainDrum->hiHatRX - 5,
                mainDrum->hiHatTopCY,
                mainDrum->hiHatTopCX + mainDrum->hiHatRX + 5,
                mainDrum->hiHatTopCY, WHITE);
  BresenhamLine(mainDrum->hiHatTopCX - mainDrum->hiHatRX - 5,
                mainDrum->hiHatTopCY + 10,
                mainDrum->hiHatTopCX + mainDrum->hiHatRX + 5,
                mainDrum->hiHatTopCY + 10, WHITE);

  Bres_ThickLine(mainDrum->hiHatStandRodX, mainDrum->hiHatStandRodDY[0],
                 mainDrum->hiHatStandRodX, mainDrum->hiHatStandRodDY[1], 5,
                 WHITE);

  Bres_ThickLine(mainDrum->hiHatBoomArmDX[0], mainDrum->hiHatBoomArmDY[0],
                 mainDrum->hiHatBoomArmDX[1], mainDrum->hiHatBoomArmDY[1], 4,
                 WHITE);

  Bres_ThickLine(mainDrum->hiHatLegLeftDX[0], mainDrum->hiHatLegLeftDY[0],
                 mainDrum->hiHatLegLeftDX[1], mainDrum->hiHatLegLeftDY[1], 5,
                 WHITE);

  Bres_ThickLine(mainDrum->hiHatLegRightDX[0], mainDrum->hiHatLegRightDY[0],
                 mainDrum->hiHatLegRightDX[1], mainDrum->hiHatLegRightDY[1], 5,
                 WHITE);

  Bres_ThickLine(mainDrum->hiHatLegLeftDX[1] - 10, mainDrum->hiHatLegLeftDY[1],
                 mainDrum->hiHatLegLeftDX[1] + 10, mainDrum->hiHatLegLeftDY[1],
                 3, WHITE);
  Bres_ThickLine(mainDrum->hiHatLegRightDX[1] - 10,
                 mainDrum->hiHatLegRightDY[1],
                 mainDrum->hiHatLegRightDX[1] + 10,
                 mainDrum->hiHatLegRightDY[1], 3, WHITE);

  EndTextureMode();
}

void jamScreen(void) {
  if (!backgroundInitialized) {
    initBackgroundTexture();
  }

  ClearBackground(BLACK);
  PauseMusicStream(amenberak);

  DrawTextureRec(backgroundTexture.texture,
                 (Rectangle){0, 0, (float)backgroundTexture.texture.width,
                             -(float)backgroundTexture.texture.height},
                 (Vector2){0, 0}, WHITE);

  static DrumKitConfig mainDrum = {
      .kick = {.x = scrWidth / 2, .y = (scrHeight / 2) + 80},
      .kickOuterRadius = 150,
      .kickInnerRadius = 140,
      .kickStroke = 3,
      .kickLegLeftDX = {-110, -176},
      .kickLegLeftDY = {100, 157},
      .kickLegRightDX = {110, 176},
      .kickLegRightDY = {100, 157},

      .tom1 = {.x = 470, .y = 190},
      .tomWidth = 160,
      .tomHeight = 80,
      .tomStroke = 3,
      .tomRimWidth = 170,
      .tomRimHeight = 10,
      .tomRimStroke = 2,
      .tomRimTopDY = -8,
      .tomRimBottomDY = 80,
      .tomBarDX = 160,
      .tomBarDY = 31,
      .tomLugCount = 6,
      .tomLugWidth = 8,
      .tomLugHeight = 15,
      .tom2 = {.x = 660, .y = 195},

      .snare = {.x = 837, .y = 328},
      .snareWidth = 140,
      .snareHeight = 60,
      .snareStroke = 3,
      .snareRimHeight = 7,
      .snareRimWidth = 150,
      .snareRimStroke = 2,
      .snareRimTopDY = -8,
      .snareRimBottomDY = 60,
      .snareStandRod = {.x = 0, .y = 0, .width = 4, .height = 180},
      .snareStandLegLeftDX = {-70, 0},
      .snareStandLegLeftDY = {180, 165},
      .snareStandLegRightDX = {70, 0},
      .snareStandLegRightDY = {180, 165},

      .floorTom = {.x = 270, .y = 280},
      .floorTomWidth = 160,
      .floorTomHeight = 170,
      .floorTomStroke = 3,
      .floorTomRimWidth = 170,
      .floorTomRimHeight = 10,
      .floorTomRimStroke = 2,
      .floorTomLugCount = 6,
      .floorTomLugWidth = 8,
      .floorTomLugHeight = 15,
      .floorTomRimTopDY = -10,
      .floorTomRimBottomDY = 170,
      .floorTomLegCount = 3,
      .floorTomLegDX = {-70, 0, 70},
      .floorTomLegDY = {120, 140, 120},
      .floorTomLegFootDX = {-100, 0, 100},
      .floorTomLegFootDY = {130, 150, 130},

      .rideCymbal = {955, 155},
      .cymbalRx = 100,
      .cymbalRy = 15,
      .cymbalStandRodX = 1056,
      .cymbalStandRodDY = {203, 550},
      .cymbalBoomArmDX = {955, 1089},
      .cymbalBoomArmDY = {155, 216},
      .cymbalLegLeftDX = {1055, 1000},
      .cymbalLegLeftDY = {515, 550},
      .cymbalLegRightDX = {1055, 1105},
      .cymbalLegRightDY = {515, 550},

      .hiHatTopCX = 162,
      .hiHatTopCY = 179,
      .hiHatRX = 100,
      .hiHatRY = 15,
      .hiHatStandRodX = 162,
      .hiHatStandRodDY = {160, 520},
      .hiHatBoomArmDX = {162, 162},
      .hiHatBoomArmDY = {195, 220},
      .hiHatLegLeftDX = {162, 110},
      .hiHatLegLeftDY = {485, 520},
      .hiHatLegRightDX = {162, 215},
      .hiHatLegRightDY = {485, 520}};

  if (!drumKitTextureInitialized) {
    renderDrumKitToTexture(&mainDrum);
  }

  DrawTextureRec(drumKitTexture.texture,
                 (Rectangle){0, 0, (float)drumKitTexture.texture.width,
                             -(float)drumKitTexture.texture.height},
                 (Vector2){0, 0}, WHITE);

  static int padsInitialized = 0;
  static DrumPad drumPads[MAX_KEYBINDS];
  if (!padsInitialized) {
    InitDrumPads(drumPads, &mainDrum);
    padsInitialized = 1;
  }

  UpdateDrumPads(drumPads, MAX_KEYBINDS);

  Vector2 mousePos = GetMousePosition();
  DrawText(TextFormat("Screen X: %.0f", mousePos.x), 200, 20, 12, WHITE);
  DrawText(TextFormat("Screen Y: %.0f", mousePos.y), 200, 40, 12, WHITE);

  DrawNoteAnimations();

  DrawSparkParticles();

  DrawCymbalScaleAnimations();

  DrawStickAnimations();

  UpdateAmenDemo(drumPads);

  Rectangle amenDemoBtn = {scrWidth - 200, 40, 160, 50};
  DrawAmenDemoButton(amenDemoBtn);

  DrawMetronomeButton();
  DrawMetronomePopup();

  if (IsKeyPressed(KEY_ESCAPE)) {
    currentScreen = SCREEN_MENU;
    PlayMusicStream(amenberak);
    amenDemoActive = 0;
    amenDemo.active = 0;
    metronomeActive = 0;
    metronomePopupOpen = 0;
  }

  if (GuiButton(backBtn, "Kembali")) {
    currentScreen = SCREEN_MENU;
    PlayMusicStream(amenberak);
    amenDemoActive = 0;
    amenDemo.active = 0;
    metronomeActive = 0;
    metronomePopupOpen = 0;
  }
}
