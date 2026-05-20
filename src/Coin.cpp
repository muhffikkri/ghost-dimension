// ============================================================
// Coin.cpp
// ============================================================

#include "shared/Coin.h"

static void drawCoinShape() {
    GLUquadric* q = gluNewQuadric();

    float tebal  = 0.14f;
    float radius = 0.38f;

    // sisi
    glColor3f(0.82f, 0.55f, 0.0f); // emas gelap untuk tepi
    gluCylinder(q, radius, radius, tebal, 32, 2);

    // belakang
    glPushMatrix();
        glRotatef(180, 1, 0, 0);
        glColor3f(1.0f, 0.80f, 0.12f);
        gluDisk(q, 0.0, radius, 32, 1);
    glPopMatrix();

    // depan
    glPushMatrix();
        glTranslatef(0, 0, tebal);
        glColor3f(1.0f, 0.82f, 0.15f);
        gluDisk(q, 0.0, radius, 32, 1);
    glPopMatrix();

    // cincin dalam
    glPushMatrix();
        glTranslatef(0, 0, tebal + 0.001f);
        glColor3f(0.95f, 0.88f, 0.35f);
        gluDisk(q, 0.10, 0.28, 28, 1);     // cincin tengah
    glPopMatrix();

    // titik pusat emas
    glPushMatrix();
        glTranslatef(0, 0, tebal + 0.002f);
        glColor3f(1.0f, 0.95f, 0.5f);
        gluDisk(q, 0.0, 0.10, 16, 1);
    glPopMatrix();

    gluDeleteQuadric(q);
}

void drawCoinDemo(float rotX, float rotY, float rotZ) {
    glPushMatrix();
        glTranslatef(0, 0, -0.07f);
        glRotatef(rotX, 1, 0, 0);
        glRotatef(rotY, 0, 1, 0);
        glRotatef(rotZ, 0, 0, 1);
        drawCoinShape();
    glPopMatrix();
}

void drawCoinGame(float px, float py, float spinAngle) {
    glPushMatrix();
        glTranslatef(px, 0.5f, py);
        glRotatef(spinAngle, 0, 1, 0);

        glRotatef(90, 1, 0, 0);
        glTranslatef(0, 0, -0.07f); // centerin koin
        drawCoinShape();
    glPopMatrix();
}

void initCoins(Coin coins[], int& numCoins,
               const char map[][MAZE_WIDTH], int mapW, int mapH, float tile) {
    numCoins = 0;
    for (int row = 0; row < mapH && numCoins < MAX_COINS; row++)
        for (int col = 0; col < mapW && numCoins < MAX_COINS; col++)
            if (map[row][col] == 'C') {
                coins[numCoins].x         = (col + 0.5f) * tile;
                coins[numCoins].y         = (row + 0.5f) * tile;
                coins[numCoins].aktif     = true;
                coins[numCoins].spinAngle = 0.0f;
                numCoins++;
            }
}

void updateCoins(Coin coins[], int numCoins, float dt) {
    for (int i = 0; i < numCoins; i++)
        if (coins[i].aktif) {
            coins[i].spinAngle += 90.0f * dt;
            if (coins[i].spinAngle >= 360.0f)
                coins[i].spinAngle -= 360.0f;
        }
}
