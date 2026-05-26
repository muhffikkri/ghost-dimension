// Nama File : Config.h
// Deskripsi : Konstanta konfigurasi aplikasi dan deklarasi status game global.
// Tanggal Dibuat : 24 Maret 2026

#ifndef CONFIG_H
#define CONFIG_H

#include <GL/glut.h>
#include <stdio.h>

/* ============================================================
   Config.h  -  Semua konstanta, struct, dan extern global
   C++98 COMPATIBLE: tidak ada constexpr, tidak ada brace-init
   ============================================================ */

/* Konstanta pakai #define agar kompatibel C++98 */
#define WINDOW_WIDTH    1280
#define WINDOW_HEIGHT   720
#define MAZE_WIDTH      20
#define MAZE_HEIGHT     20
#define MAX_COINS       64
#define PLAYER_SPEED    0.3f

/* ----------------------------------------------------------------
   ScoreEntry: riwayat skor pemain
   Tambahan field: coinsCollected, fastestTime
   ---------------------------------------------------------------- */
struct ScoreEntry {
    int   score;
    float timeSurvived;
    int   coinsCollected;
    char  timestamp[16];
};

/* ----------------------------------------------------------------
   Variabel global (definisi ada di Config.cpp)
   ---------------------------------------------------------------- */
extern bool       isGameOver;
extern bool       isGameWin;
extern bool       hasKey;
extern bool       isGhostTriggered;
extern int        score;
extern int        topScore;
extern ScoreEntry scoreHistory[5];
extern int        historyCount;
extern float      gameDuration;
extern int        mazeMatrix[MAZE_HEIGHT][MAZE_WIDTH];

void generateMaze();

#endif /* CONFIG_H */


