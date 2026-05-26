// Nama File : Entity.h
// Deskripsi : Deklarasi entitas game dan fungsi logika interaksi utama.
// Tanggal Dibuat : 24 Maret 2026

#ifndef ENTITY_H
#define ENTITY_H

#include "Config.h"

/* --- Coin --- */
struct Coin {
    float x, y;
    bool  aktif;
    float spinAngle;
   float floatOffset;
   float animSeed;
};

/* --- Key --- */
struct Key {
    float x, y;
    bool  aktif;
    float floatOffset;
    float floatTime;
    float spinAngle;
};

/* --- Ghost mode --- */
enum GhostMode { PATROL, CHASE, RETURN };

/* --- Vec2 untuk waypoint --- */
struct Vec2 { float x, y; };

/* --- Ghost --- */
struct Ghost {
    float     x, y;
    float     rotY;
    float     speedPatrol;
    float     speedChase;
    float     sightRange;
    float     loseRange;
    float     chaseTimer;
    GhostMode mode;
    Vec2      waypoints[16];
    int       numWP;
    int       targetWP;
};

/* ----------------------------------------------------------------
   Fungsi-fungsi Coin (Coin.cpp)
   ---------------------------------------------------------------- */
void drawCoinDemo(float rotX, float rotY, float rotZ);
void drawCoinGame(float px, float py, float floatOffset, float spinAngle);
void initCoins(Coin coins[], int& numCoins,
               const char map[][MAZE_WIDTH], int mapW, int mapH, float tile);
void updateCoins(Coin coins[], int numCoins, float dt);

/* ----------------------------------------------------------------
   Fungsi-fungsi Key (Key.cpp)
   ---------------------------------------------------------------- */
void drawKeyDemo(float rotX, float rotY, float rotZ);
void drawKeyGame(float px, float py, float floatOffset, float spinAngle);
void initKey(Key& k, const char map[][MAZE_WIDTH], int mapW, int mapH, float tile);
void updateKey(Key& k, float dt);

/* ----------------------------------------------------------------
   Fungsi-fungsi Ghost (Ghost.cpp)
   ---------------------------------------------------------------- */
void drawGhostDemo(float rotX, float rotY, float rotZ);
void drawGhostGame(float px, float py, float faceY);
void initGhost(Ghost& g, const char map[][MAZE_WIDTH], int mapW, int mapH, float tile);
void updateGhost(Ghost& g, float dt,
                 float playerX, float playerZ,
                 const char map[][MAZE_WIDTH], int mapW, int mapH, float tile);
bool ghostCatchesPlayer(const Ghost& g, float playerX, float playerZ, float catchRadius);

/* ----------------------------------------------------------------
   Fungsi sistem entitas (Entity.cpp)
   ---------------------------------------------------------------- */
void  initEntities();
void  syncPlayerPosition(float x, float z);
bool  checkCollision(float nextX, float nextZ);
void  updateGhost(float px, float pz);   /* wrapper Entity.cpp */
void  updateItems();
void  checkGameStatus();
void  saveScoreToHistory();
void drawExitPoint(float x, float z);

/* Getter untuk minimap & HUD */
float getGhostDist();
float getGhostX();
float getGhostZ();
float getKeyX();
float getKeyZ();
bool  isKeyActive();
int   getNumCoins();
bool  getCoinActive(int i);
float getCoinX(int i);
float getCoinZ(int i);
float getExitX();
float getExitZ();

#endif /* ENTITY_H */
