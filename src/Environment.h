#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <GL/glut.h>

/* Globals accessible from main.cpp */
extern GLuint g_texWall;
extern GLuint g_texFloor;
extern float  g_time;

extern float camX,  camY,  camZ;
extern float lookX, lookY, lookZ;

/* Functions */
void initEnvironment();
void genTextures();
void setupFlashlight();
void setupFog();
void renderEnvironment();
void drawMinimap();
void drawTexturedFloor(float x, float z, float s);
void drawTorch(float x, float y, float z, int id);
void drawTorchFlame(float flickVal);
void drawWallCube(float wx, float wz, float S, float WH);
void drawShadows();
void drawVolumetricSpotlight();

#endif
