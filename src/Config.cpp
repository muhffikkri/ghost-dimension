// Nama File : Config.cpp
// Deskripsi : Implementasi variabel global dan konfigurasi runtime game.
// Tanggal Dibuat : 24 Maret 2026

#include "../include/Config.h"
#include <string.h>

bool       isGameOver      = false;
bool       isGameWin       = false;
bool       hasKey          = false;
bool       isGhostTriggered = false;
int        score           = 0;
int        topScore        = 0;
float      gameDuration    = 0.0f;
int        historyCount    = 0;

/* C++98: inisialisasi struct array pakai memset */
ScoreEntry scoreHistory[5];

int mazeMatrix[MAZE_HEIGHT][MAZE_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,0,1,1,0,1,1,1,1,0,1,1,0,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,1},
    {1,0,1,0,1,1,1,1,1,0,0,1,0,1,1,0,1,0,1,1},
    {1,0,0,0,1,0,0,0,1,0,1,1,0,0,1,0,0,0,0,1},
    {1,1,1,0,1,0,1,0,1,0,0,0,0,1,1,0,1,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1},
    {1,1,1,0,1,1,1,0,1,1,1,0,1,1,0,1,0,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,0,1,1,0,0,1,1,1,0,0,1,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,0,1},
    {1,1,1,0,1,0,1,1,0,1,1,0,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void generateMaze() {
    memset(scoreHistory, 0, sizeof(scoreHistory));
}
