// ============================================================
// Coin.cpp
// ============================================================

#include "../../include/Coin.h"
#include <math.h>
#include <time.h>

static void drawCoinShape() {
    GLUquadric* q = gluNewQuadric();
    float tebal  = 0.14f;
    float radius = 0.38f;

    glColor3f(0.82f, 0.55f, 0.0f);
    gluCylinder(q, radius, radius, tebal, 32, 2);

    glPushMatrix(); glRotatef(180,1,0,0);
        glColor3f(1.0f, 0.80f, 0.12f);
        gluDisk(q, 0.0, radius, 32, 1);
    glPopMatrix();

    glPushMatrix(); glTranslatef(0,0,tebal);
        glColor3f(1.0f, 0.82f, 0.15f);
        gluDisk(q, 0.0, radius, 32, 1);
    glPopMatrix();

    glPushMatrix(); glTranslatef(0,0,tebal+0.001f);
        glColor3f(0.95f, 0.88f, 0.35f);
        gluDisk(q, 0.10, 0.28, 28, 1);
    glPopMatrix();

    glPushMatrix(); glTranslatef(0,0,tebal+0.002f);
        glColor3f(1.0f, 0.95f, 0.5f);
        gluDisk(q, 0.0, 0.10, 16, 1);
    glPopMatrix();

    gluDeleteQuadric(q);
}

void drawCoinDemo(float rotX, float rotY, float rotZ) {
    glPushMatrix();
        glTranslatef(0,0,-0.07f);
        glRotatef(rotX,1,0,0); glRotatef(rotY,0,1,0); glRotatef(rotZ,0,0,1);
        drawCoinShape();
    glPopMatrix();
}

void drawCoinGame(float px, float py, float floatOffset, float spinAngle) {
    glPushMatrix();
        glTranslatef(px, 0.95f + floatOffset, py);
        glRotatef(spinAngle, 0.0f, 1.0f, 0.0f);
        glRotatef(90.0f, 0.0f,1.0f,0.0f);
        glTranslatef(0,0,-0.07f);
        drawCoinShape();
    glPopMatrix();
}

// FIXED: initCoins() sekarang baca dari map char[][] yang sudah diisi 'C'
void initCoins(Coin coins[], int& numCoins,
               const char map[][MAZE_WIDTH], int mapW, int mapH, float tile) {
    numCoins = 0;
    float baseSeed = (float)(time(NULL) % 1000);
    for (int row = 0; row < mapH && numCoins < MAX_COINS; row++)
        for (int col = 0; col < mapW && numCoins < MAX_COINS; col++)
            if (map[row][col] == 'C') {
                coins[numCoins].x         = (col + 0.5f) * tile;
                coins[numCoins].y         = (row + 0.5f) * tile;
                coins[numCoins].aktif     = true;
                coins[numCoins].floatOffset = 0.0f;
                coins[numCoins].animSeed  = baseSeed + (float)(row * 17 + col * 11) * 0.37f;
                coins[numCoins].spinAngle = 0.0f;
                numCoins++;
            }
}

void updateCoins(Coin coins[], int numCoins, float dt) {
    float now = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    for (int i = 0; i < numCoins; i++)
        if (coins[i].aktif) {
            float phase = now * 1.5f + coins[i].animSeed;
            coins[i].floatOffset = 0.18f * sinf(phase * 2.1f);
            coins[i].spinAngle   = fmodf((now * (140.0f + fmodf(coins[i].animSeed, 40.0f))) + coins[i].animSeed * 57.0f, 360.0f);
        }
}


