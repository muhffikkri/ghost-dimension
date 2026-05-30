#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Config.h"

/* ============================================================
   Environment.h  -  Deklarasi fungsi rendering lingkungan
   ============================================================ */

extern GLuint g_texWall;
extern GLuint g_texFloor;
extern float  g_time;

extern float camX, camY, camZ;
extern float lookX, lookY, lookZ;

void genTextures();
void initEnvironment();
void renderEnvironment();
void setupFlashlight();
void drawMinimap();
void drawTorch(float x, float y, float z, int id);
void drawWallCube(float wx, float wz, float S, float WH);
void drawTexturedFloor(float x, float z, float s);
void drawExitPoint(float ex, float ez);

#endif 
