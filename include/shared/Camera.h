// Nama File : Camera.h
// Deskripsi : Definisi struktur kamera untuk pergerakan dan sudut pandang pemain.
// Tanggal Dibuat : 24 Maret 2026

// System & Camera

#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>

struct Camera {
    float x, y, z;           // Position
    float angle;             // Yaw (left-right rotation)
    float pitch;             // Pitch (up-down rotation)
    float bobbingTimer;      // For head bobbing animation
    
    // Movement state (WASD)
    bool moveW, moveA, moveS, moveD;
    
    // Look state (Arrow keys)
    bool lookUp, lookDown, lookLeft, lookRight;

    void handleInput(float dt);
    void update(float dt);   // NEW: update with delta time for bobbing
    void apply();            // gluLookAt with pitch calculation
};

// Kamera pemain utama yang dipakai modul rendering lain (mis. lighting).
extern Camera playerCam;

// Render HUD menggunakan proyeksi ortografis.
void drawHUD();

#endif