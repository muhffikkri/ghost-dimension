// Nama File : Config.h
// Deskripsi : Konstanta konfigurasi aplikasi dan deklarasi status game global.
// Tanggal Dibuat : 24 Maret 2026

#ifndef CONFIG_H
#define CONFIG_H


//Maze settings
const int MAZE_WIDTH = 11;
const int MAZE_HEIGHT = 11;
const int MAZE_COLS = 11;
const int MAZE_ROWS = 11;

//map data
extern int mazeMatrix[MAZE_ROWS][MAZE_COLS];

// Screen Settings
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Game States
extern bool isGameOver;
extern bool isGameWin;
extern bool hasKey;
extern int score;
extern bool isGhostTriggered;

// TODO: Tambahkan konstanta untuk kecepatan gerak dan sensitivitas mouse
// TODO: Tambahkan threshold jarak untuk trigger munculnya hantu


// Constants (Bisa langsung di header karena 'const')
const float PLAYER_SPEED = 0.1f;
const float GHOST_SPEED = 0.05f;

#endif