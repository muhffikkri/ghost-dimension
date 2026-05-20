// Nama File : Config.h
// Deskripsi : Konstanta konfigurasi aplikasi dan deklarasi status game global.
// Tanggal Dibuat : 24 Maret 2026

#ifndef CONFIG_H
#define CONFIG_H

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

// Camera tuning
const float CAMERA_FOVY = 70.0f;
const float CAMERA_MOVE_SPEED = 2.6f;
const float CAMERA_TURN_SPEED = 1.8f;
const float CAMERA_PITCH_SPEED = 1.4f;
const float CAMERA_PITCH_LIMIT = 1.0471976f; // 60 deg

// Lighting tuning
const float FLASHLIGHT_CUTOFF = 18.0f;
const float TORCH_LINEAR_ATT = 1.0f;
const float TORCH_QUAD_ATT = 0.45f;

// Map Data
#define MAZE_WIDTH  20
#define MAZE_HEIGHT 20

const float MAZE_CELL_SIZE = 3.0f;

extern int mazeMatrix[MAZE_HEIGHT][MAZE_WIDTH];
void generateMaze();

// Backward-compatible alias used by older logic.
#define maze mazeMatrix

// Constants (Bisa langsung di header karena 'const')
const float PLAYER_SPEED = 0.1f;
const float GHOST_SPEED = 0.05f;

#endif