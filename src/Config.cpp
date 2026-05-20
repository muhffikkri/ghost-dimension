// Nama File : Config.cpp
// Deskripsi : Implementasi variabel global dan konfigurasi runtime game.
// Tanggal Dibuat : 24 Maret 2026

#include "shared/Config.h"

// Inisialisasi variabel global yang dideklarasikan di Config.h
bool isGameOver = false;
bool isGameWin = false;
bool hasKey = false;
bool isGhostTriggered = false;
int score = 0;
int topScore = 0;  // NEW: Track highest score

// Implementasi Map Labirin (0: Jalan, 1: Dinding)
int mazeMatrix[MAZE_HEIGHT][MAZE_WIDTH] = {
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,1},
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
    // maze sudah hardcoded di atas
}