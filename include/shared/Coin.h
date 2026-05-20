#ifndef COIN_H
#define COIN_H

// ============================================================
// Coin.h
// Header untuk entity Koin
// ============================================================

#include <GL/glut.h>
#include "shared/Config.h"

// Data satu koin
struct Coin {
    float x, y;       // posisi dunia (game)
    bool  aktif;       // masih ada atau sudah dipungut
    float spinAngle;   // sudut rotasi animasi (derajat)
};

#define MAX_COINS 5

// ----- Fungsi Gambar -----

// Untuk DEMO: gambar koin di tengah, ikut rotasi keyboard
void drawCoinDemo(float rotX, float rotY, float rotZ);

// Untuk GAME: gambar koin di posisi (px, py) dengan animasi spin
void drawCoinGame(float px, float py, float spinAngle);

// ----- Fungsi Game -----

// Isi array koin dari data map (karakter 'C' di MAP)
void initCoins(Coin coins[], int& numCoins,
               const char map[][MAZE_WIDTH], int mapW, int mapH, float tile);

// Update animasi spin semua koin aktif
void updateCoins(Coin coins[], int numCoins, float dt);

#endif
