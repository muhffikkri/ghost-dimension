// Nama File : Entity.cpp
// Deskripsi : Implementasi logika entitas seperti item, hantu, dan status game.
// Tanggal Dibuat : 24 Maret 2026

#include "shared/Entity.h"
#include "shared/Config.h"
#include <GL/glut.h>
#include <math.h>

static const float kTileSize          = TILE_SIZE;
static const float kPlayerRadius      = PLAYER_COLLISION_RADIUS;
static const float kPickupRadius      = COIN_PICKUP_RADIUS;
static const float kGhostTriggerDist  = GHOST_TRIGGER_DISTANCE;
static const float kGhostCatchRadius  = GHOST_CATCH_DISTANCE;
static const float kWinRadius         = 1.0f;

static Coin  g_coins[MAX_COINS];
static int   g_numCoins = 0;
static Key   g_key;
static Ghost g_ghost;
static bool  g_entitiesReady = false;

static float g_playerX = 0.0f;
static float g_playerZ = 0.0f;

static char g_entityMap[MAZE_HEIGHT][MAZE_WIDTH];
static float g_exitX = 0.0f;
static float g_exitZ = 0.0f;

static float g_lastItemsTime = 0.0f;
static float g_lastGhostTime = 0.0f;

static bool isWallCell(int row, int col) {
    if (row < 0 || row >= MAZE_HEIGHT || col < 0 || col >= MAZE_WIDTH) {
        return true;
    }
    return mazeMatrix[row][col] == 1;
}

static float dist2D(float ax, float az, float bx, float bz) {
    float dx = ax - bx;
    float dz = az - bz;
    return sqrtf(dx*dx + dz*dz);
}

static void placeEntity(char marker, int row, int col) {
    if (!isWallCell(row, col)) {
        g_entityMap[row][col] = marker;
    }
}

static void buildEntityMap() {
    for (int row = 0; row < MAZE_HEIGHT; row++) {
        for (int col = 0; col < MAZE_WIDTH; col++) {
            g_entityMap[row][col] = isWallCell(row, col) ? '#' : '.';
        }
    }

    int coinSpots[][2] = {
        {1, 1}, {1, 5}, {3, 3}, {7, 1}, {9, 7}
    };
    int coinCount = (int)(sizeof(coinSpots) / sizeof(coinSpots[0]));
    for (int i = 0; i < coinCount; i++) {
        placeEntity('C', coinSpots[i][0], coinSpots[i][1]);
    }

    placeEntity('K', 8, 8);

    // Exit default dekat ujung maze
    int exitRow = MAZE_HEIGHT - 2;
    int exitCol = MAZE_WIDTH - 2;
    if (isWallCell(exitRow, exitCol)) {
        exitRow = 1;
        exitCol = 1;
    }
    g_exitX = (exitCol + 0.5f) * kTileSize;
    g_exitZ = (exitRow + 0.5f) * kTileSize;
}

void initEntities() {
    buildEntityMap();

    initCoins(g_coins, g_numCoins, g_entityMap, MAZE_WIDTH, MAZE_HEIGHT, kTileSize);
    initKey(g_key, g_entityMap, MAZE_WIDTH, MAZE_HEIGHT, kTileSize);
    initGhost(g_ghost, g_entityMap, MAZE_WIDTH, MAZE_HEIGHT, kTileSize);

    isGhostTriggered = false;
    hasKey = false;
    score = 0;
    isGameOver = false;
    isGameWin = false;

    g_entitiesReady = true;
    g_lastItemsTime = 0.0f;
    g_lastGhostTime = 0.0f;
}

void syncPlayerPosition(float x, float z) {
    g_playerX = x;
    g_playerZ = z;
}

// Cek collision pemain vs dinding
bool checkCollision(float nextX, float nextZ) {
    float r = kPlayerRadius;

    int col1 = (int)((nextX + r) / kTileSize);
    int row1 = (int)((nextZ + r) / kTileSize);
    int col2 = (int)((nextX - r) / kTileSize);
    int row2 = (int)((nextZ - r) / kTileSize);

    if (isWallCell(row1, col1) || isWallCell(row1, col2) ||
        isWallCell(row2, col1) || isWallCell(row2, col2)) {
        return true;
    }

    return false;
}

// Update AI ghost
void updateGhost(float px, float pz) {
    if (!g_entitiesReady) {
        initEntities();
    }

    syncPlayerPosition(px, pz);

    float now = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float dt = (g_lastGhostTime > 0.0f) ? (now - g_lastGhostTime) : 0.016f;
    g_lastGhostTime = now;

    if (!isGhostTriggered) {
        float dist = dist2D(px, pz, g_ghost.x, g_ghost.y);
        if (dist <= kGhostTriggerDist) {
            isGhostTriggered = true;
        } else {
            return;
        }
    }

    updateGhost(g_ghost, dt, px, pz, g_entityMap, MAZE_WIDTH, MAZE_HEIGHT, kTileSize);
}

// Update item dan render entity
void updateItems() {
    if (!g_entitiesReady) {
        initEntities();
    }

    float now = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float dt = (g_lastItemsTime > 0.0f) ? (now - g_lastItemsTime) : 0.016f;
    g_lastItemsTime = now;

    updateCoins(g_coins, g_numCoins, dt);
    updateKey(g_key, dt);

    for (int i = 0; i < g_numCoins; i++) {
        if (!g_coins[i].aktif) continue;
        float dist = dist2D(g_playerX, g_playerZ, g_coins[i].x, g_coins[i].y);
        if (dist <= kPickupRadius) {
            g_coins[i].aktif = false;
            score += COIN_SCORE_VALUE;  // Use constant from Config.h
        }
    }

    if (g_key.aktif) {
        float dist = dist2D(g_playerX, g_playerZ, g_key.x, g_key.y);
        if (dist <= kPickupRadius) {
            g_key.aktif = false;
            hasKey = true;
        }
    }

    for (int i = 0; i < g_numCoins; i++) {
        if (g_coins[i].aktif) {
            drawCoinGame(g_coins[i].x, g_coins[i].y, g_coins[i].spinAngle);
        }
    }

    if (g_key.aktif) {
        drawKeyGame(g_key.x, g_key.y, g_key.floatOffset, g_key.spinAngle);
    }

    if (isGhostTriggered) {
        drawGhostGame(g_ghost.x, g_ghost.y, g_ghost.rotY);
    }

    checkGameStatus();
}

// Logika pengecekan Win/Lose Condition
void checkGameStatus() {
    if (isGameOver || isGameWin) {
        return;
    }

    if (isGhostTriggered && ghostCatchesPlayer(g_ghost, g_playerX, g_playerZ, kGhostCatchRadius)) {
        isGameOver = true;
        return;
    }

    if (hasKey) {
        float dist = dist2D(g_playerX, g_playerZ, g_exitX, g_exitZ);
        if (dist <= kWinRadius) {
            isGameWin = true;
            // NEW: Update top score
            if (score > topScore) {
                topScore = score;
            }
        }
    }
}