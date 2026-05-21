// Nama File : Entity.cpp
// Deskripsi : Implementasi logika entitas seperti item, hantu, dan status game.
// Tanggal Dibuat : 24 Maret 2026

// Nama File : Entity.cpp
// Deskripsi : Implementasi logika entitas seperti item, hantu, dan status game.
// Tanggal Dibuat : 24 Maret 2026

#include "../include/Entity.h"
#include "../include/Config.h"
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <cstdio>

// --- Konstanta ---------------------------------------------------------------
static const float kTileSize         = 2.0f;
static const float kPlayerRadius     = 0.35f;
static const float kPickupRadius     = 0.6f;
static const float kGhostTriggerDist = 6.0f;
static const float kGhostCatchRadius = 0.8f;
static const float kWinRadius        = 0.8f;

// --- Variabel Global / Static ------------------------------------------------
Coin  g_coins_arr[MAX_COINS];
int   g_numCoins = 0;
static Key   g_key;
static Ghost g_ghost;
static bool  g_entitiesReady = false;

static float g_playerX = 0.0f;
static float g_playerZ = 0.0f;

static char  g_entityMap[MAZE_HEIGHT][MAZE_WIDTH];
float        g_exitX = 0.0f;
float        g_exitZ = 0.0f;

static float g_lastItemsTime = 0.0f;
static float g_lastGhostTime = 0.0f;

// Alias agar kode lama tetap jalan
Coin* g_coins = g_coins_arr;

// --- Getter (harus SETELAH deklarasi variabel di atas) -----------------------
float getKeyX()      { return g_key.x; }
float getKeyZ()      { return g_key.y; }
bool  isKeyActive()  { return g_key.aktif; }   // bool, sesuai Entity.h
float getGhostX()    { return g_ghost.x; }
float getGhostZ()    { return g_ghost.y; }
float getGhostDist() {
    float dx = g_playerX - g_ghost.x;
    float dz = g_playerZ - g_ghost.y;
    return sqrtf(dx*dx + dz*dz);
}

// --- Helper ------------------------------------------------------------------
static bool isWallCell(int row, int col) {
    if (row < 0 || row >= MAZE_HEIGHT || col < 0 || col >= MAZE_WIDTH) return true;
    return mazeMatrix[row][col] == 1;
}

static float dist2D(float ax, float az, float bx, float bz) {
    float dx = ax - bx, dz = az - bz;
    return sqrtf(dx*dx + dz*dz);
}

static void placeEntity(char marker, int row, int col) {
    if (!isWallCell(row, col)) g_entityMap[row][col] = marker;
}

int   getNumCoins()          { return g_numCoins; }
bool  getCoinActive(int i)   { return g_coins_arr[i].aktif; }
float getCoinX(int i)        { return g_coins_arr[i].x; }
float getCoinZ(int i)        { return g_coins_arr[i].y; }  // y = world Z
float getExitX()             { return g_exitX; }
float getExitZ()             { return g_exitZ; }

// --- Build Entity Map ---------------------------------------------------------
static void buildEntityMap() {
    for (int row = 0; row < MAZE_HEIGHT; row++)
        for (int col = 0; col < MAZE_WIDTH; col++)
            g_entityMap[row][col] = isWallCell(row, col) ? '#' : '.';

    int coinSpots[][2] = {
        {1,1},{1,5},{1,9},{1,13},
        {3,3},{3,7},{3,15},
        {5,1},{5,5},{5,9},
        {7,3},{7,7},{7,13},
        {9,1},{9,9},{9,13},
        {11,3},{11,7},{11,11},
        {13,3},{13,9},{13,15},
        {15,1},{15,7},{15,13},
        {17,1},{17,5},{17,9},{17,13}
    };
    int coinCount = (int)(sizeof(coinSpots) / sizeof(coinSpots[0]));
    for (int i = 0; i < coinCount; i++)
        placeEntity('C', coinSpots[i][0], coinSpots[i][1]);

    placeEntity('K', 9, 9);

    int exitRow = MAZE_HEIGHT - 2, exitCol = MAZE_WIDTH - 2;
    while (isWallCell(exitRow, exitCol) && exitRow > 1) exitRow--;
    while (isWallCell(exitRow, exitCol) && exitCol > 1) exitCol--;
    g_exitX = (exitCol + 0.5f) * kTileSize;
    g_exitZ = (exitRow + 0.5f) * kTileSize;
}

// --- Init ---------------------------------------------------------------------
void initEntities() {
    buildEntityMap();

    initCoins(g_coins_arr, g_numCoins, g_entityMap, MAZE_WIDTH, MAZE_HEIGHT, kTileSize);
    initKey(g_key, g_entityMap, MAZE_WIDTH, MAZE_HEIGHT, kTileSize);
    initGhost(g_ghost, g_entityMap, MAZE_WIDTH, MAZE_HEIGHT, kTileSize);

    isGhostTriggered = false;
    hasKey           = false;
    score            = 0;
    isGameOver       = false;
    isGameWin        = false;
    gameDuration     = 0.0f;

    g_entitiesReady = true;
    g_lastItemsTime = 0.0f;
    g_lastGhostTime = 0.0f;
}

// --- Sync & Collision ---------------------------------------------------------
void syncPlayerPosition(float x, float z) {
    g_playerX = x;
    g_playerZ = z;
}

bool checkCollision(float nextX, float nextZ) {
    float r = kPlayerRadius;
    int col1 = (int)((nextX + r) / kTileSize), row1 = (int)((nextZ + r) / kTileSize);
    int col2 = (int)((nextX - r) / kTileSize), row2 = (int)((nextZ - r) / kTileSize);
    return (isWallCell(row1, col1) || isWallCell(row1, col2) ||
            isWallCell(row2, col1) || isWallCell(row2, col2));
}

// --- Update Ghost -------------------------------------------------------------
void updateGhost(float px, float pz) {
    if (!g_entitiesReady) initEntities();
    syncPlayerPosition(px, pz);

    float now = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float dt  = (g_lastGhostTime > 0.0f) ? (now - g_lastGhostTime) : 0.016f;
    g_lastGhostTime = now;

    gameDuration += dt;

    if (!isGhostTriggered) {
        if (dist2D(px, pz, g_ghost.x, g_ghost.y) <= kGhostTriggerDist)
            isGhostTriggered = true;
        else return;
    }
    updateGhost(g_ghost, dt, px, pz, g_entityMap, MAZE_WIDTH, MAZE_HEIGHT, kTileSize);
}

// --- Update Items -------------------------------------------------------------
void updateItems() {
    if (!g_entitiesReady) initEntities();

    float now = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float dt  = (g_lastItemsTime > 0.0f) ? (now - g_lastItemsTime) : 0.016f;
    g_lastItemsTime = now;

    updateCoins(g_coins_arr, g_numCoins, dt);
    updateKey(g_key, dt);

    for (int i = 0; i < g_numCoins; i++) {
        if (!g_coins_arr[i].aktif) continue;
        if (dist2D(g_playerX, g_playerZ, g_coins_arr[i].x, g_coins_arr[i].y) <= kPickupRadius) {
            g_coins_arr[i].aktif = false;
            score += 50;
        }
    }

    if (g_key.aktif && dist2D(g_playerX, g_playerZ, g_key.x, g_key.y) <= kPickupRadius) {
        g_key.aktif = false;
        hasKey = true;
        score += 200;
    }

    for (int i = 0; i < g_numCoins; i++)
        if (g_coins_arr[i].aktif)
            drawCoinGame(g_coins_arr[i].x, g_coins_arr[i].y, g_coins_arr[i].spinAngle);

    if (g_key.aktif)
        drawKeyGame(g_key.x, g_key.y, g_key.floatOffset, g_key.spinAngle);

    if (isGhostTriggered)
        drawGhostGame(g_ghost.x, g_ghost.y, g_ghost.rotY);

    drawExitPoint(g_exitX, g_exitZ);   // pastikan deklarasi ada di Entity.h

    checkGameStatus();
}

// --- Score & Status -----------------------------------------------------------
void saveScoreToHistory() {
    int coinsGot = 0;
    for (int i = 0; i < g_numCoins; i++) if (!g_coins_arr[i].aktif) coinsGot++;

    if (historyCount == 5) {
        for (int i = 4; i > 0; i--) scoreHistory[i] = scoreHistory[i - 1];
    } else {
        historyCount++;
    }
    scoreHistory[0].score          = score;
    scoreHistory[0].timeSurvived   = gameDuration;
    scoreHistory[0].coinsCollected = coinsGot;
    sprintf(scoreHistory[0].timestamp, "%02d:%02d",
            (int)(gameDuration / 60), (int)(gameDuration) % 60);

    if (score > topScore) topScore = score;
}

void checkGameStatus() {
    if (isGameOver || isGameWin) return;

    if (isGhostTriggered && ghostCatchesPlayer(g_ghost, g_playerX, g_playerZ, kGhostCatchRadius)) {
        isGameOver = true;
        saveScoreToHistory();
        return;
    }

    if (hasKey) {
        if (dist2D(g_playerX, g_playerZ, g_exitX, g_exitZ) <= kWinRadius) {
            isGameWin = true;
            score += 500;
            saveScoreToHistory();
        }
    }
}
