#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <GL/glut.h>

/*
    Environment.h  -  Deklarasi semua fungsi dan variabel global
    dari Environment.cpp. Include di main.cpp dan file lain.
*/

/* Tekstur */
extern GLuint g_texWall;
extern GLuint g_texFloor;

/* Waktu animasi */
extern float g_time;

/* Kamera (di-set main.cpp setiap frame sebelum render) */
extern float camX, camY, camZ;
extern float lookX, lookY, lookZ;

/* Inisialisasi */
void initEnvironment();

/* Render utama */
void renderEnvironment();

/* Objek individual */
void drawWallCube(flo   at wx, float wz, float S, float WH);
void drawTexturedFloor(float x, float z, float s);
void drawTorchFlame(float flickVal);
void drawTorch(float x, float y, float z, int id);

/* Lighting */
void setupFlashlight();

/* Atmosfer */
void setupFog();

#endif
