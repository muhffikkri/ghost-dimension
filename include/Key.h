#ifndef KEY_H
#define KEY_H

// ============================================================
// Key.h
// Header untuk entity Kunci
// ============================================================

#include <GL/glut.h>
#include "Config.h"

// Data kunci
struct Key {
    float x, y;          // posisi dunia (game)
    bool  aktif;          // masih ada atau sudah dipungut
    float spinAngle;      // sudut rotasi animasi
    float floatTime;      // waktu akumulasi untuk animasi naik-turun
    float floatOffset;    // offset Y naik-turun saat ini
};

// ----- Fungsi Gambar -----

// Untuk DEMO: gambar kunci di tengah, ikut rotasi keyboard
void drawKeyDemo(float rotX, float rotY, float rotZ);

// Untuk GAME: gambar kunci di posisi (px, py) dengan animasi
void drawKeyGame(float px, float py, float floatOffset, float spinAngle);

// ----- Fungsi Game -----

// Inisialisasi kunci dari map (karakter 'K')
void initKey(Key& k, const char map[][MAZE_WIDTH], int mapW, int mapH, float tile);

// Update animasi kunci (naik-turun + spin)
void updateKey(Key& k, float dt);

#endif
