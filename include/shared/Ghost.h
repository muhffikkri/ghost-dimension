#ifndef GHOST_H
#define GHOST_H

#include <GL/glut.h>
#include "shared/Config.h"

#define MAX_WAYPOINTS 12

struct Vec2 {
    float x, y;
};

enum GhostMode { PATROL, CHASE, RETURN };

struct Ghost {
    float x, y;
    float rotY;
    float speedPatrol;
    float speedChase;
    Vec2  waypoints[MAX_WAYPOINTS];
    int   numWP;
    int   targetWP;
    GhostMode mode;
    float sightRange;
    float loseRange;
    float chaseTimer;
};

// Gambar hantu di demo (ikut rotasi keyboard)
void drawGhostDemo(float rotX, float rotY, float rotZ);

// Gambar hantu di posisi dunia game
void drawGhostGame(float px, float py, float faceY);

void initGhost(Ghost& g,
               const char map[][MAZE_WIDTH], int mapW, int mapH, float tile);

void updateGhost(Ghost& g, float dt,
                 float playerX, float playerZ,
                 const char map[][MAZE_WIDTH], int mapW, int mapH, float tile);

bool ghostCatchesPlayer(const Ghost& g,
                        float playerX, float playerZ,
                        float catchRadius);

#endif
