// Nama File : Camera.h
// Deskripsi : Definisi struktur kamera untuk pergerakan dan sudut pandang pemain.
// Tanggal Dibuat : 24 Maret 2026

// System & Camera
#ifndef CAMERA_H
#define CAMERA_H

#include "Config.h"

struct Camera {
    float x, y, z;
    float angle, pitch;
    float bobbingTimer;   /* C++98: tanpa "= 0.0f" di sini */
    void apply();
};

extern Camera playerCam;

/* ----------------------------------------------------------------
   Fungsi HUD yang diimplementasi di Camera.cpp
   ---------------------------------------------------------------- */
void drawGameHUD();
void drawGhostWarning(float ghostDist);
void drawGameOver();
void drawGameWin();
void drawSettings();

#endif /* CAMERA_H */

