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
extern int topScore;
extern bool isGhostTriggered;

// Maze Size & Environment
#define MAZE_WIDTH  20
#define MAZE_HEIGHT 20
#define TILE_SIZE   4.0f
#define WALL_HEIGHT 5.5f

// Camera tuning
const float CAMERA_FOVY = 70.0f;
const float CAMERA_MOVE_SPEED = 2.6f;
const float CAMERA_TURN_SPEED = 1.8f;
const float CAMERA_PITCH_SPEED = 1.4f;
const float CAMERA_PITCH_LIMIT = 1.0471976f;

// Lighting tuning
const float AMBIENT_BRIGHTNESS = 0.06f;
const float FLASHLIGHT_CUTOFF = 20.0f;
const float TORCH_LINEAR_ATT = 1.0f;
const float TORCH_QUAD_ATT = 0.45f;

// Collision & Entity
const float PLAYER_COLLISION_RADIUS = 0.35f;
const float COIN_PICKUP_RADIUS = 0.5f;
const float KEY_PICKUP_RADIUS = 0.5f;
const float GHOST_TRIGGER_DISTANCE = 6.0f;
const float GHOST_CATCH_DISTANCE = 0.8f;
const int COIN_SCORE_VALUE = 50;
const float GHOST_SPEED = 1.8f;
const float GHOST_BOBBING_SPEED = 2.0f;
const float GHOST_BOBBING_HEIGHT = 0.3f;
const float MAZE_CELL_SIZE = TILE_SIZE;

extern int mazeMatrix[MAZE_HEIGHT][MAZE_WIDTH];
void generateMaze();

// Backward-compatible alias used by older logic.
#define maze mazeMatrix

// Constants (Bisa langsung di header karena 'const')
const float PLAYER_SPEED = 0.1f;
//const float GHOST_SPEED = 0.05f;

#endif
