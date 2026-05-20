// Nama File : Camera.h
// Deskripsi : Definisi struktur kamera untuk pergerakan dan sudut pandang pemain.
// Tanggal Dibuat : 24 Maret 2026

// System & Camera

#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>

struct Camera {
    float x, y, z;
    float angle;
    float pitch;
    float bobbingTimer;

    bool moveUp;
    bool moveDown;
    bool moveLeft;
    bool moveRight;

    bool lookUp;
    bool lookDown;
    bool lookLeft;
    bool lookRight;

    void handleInput(float dt);
    void update(); // TODO: Implementasi perhitungan posisi kamera & head bobbing
    void apply();  // TODO: Implementasi gluLookAt
};

// Kamera pemain utama yang dipakai modul rendering lain (mis. lighting).
extern Camera playerCam;

// Render HUD menggunakan proyeksi ortografis.
void drawHUD();

#endif