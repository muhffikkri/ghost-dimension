#include "../../include/Ghost.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void drawGhostBody() {
    const float PI = 3.14159f;
    GLUquadric* q  = gluNewQuadric();

    float yBawah = -1.9f;
    float yAtas  =  0.20f;
    float rBawah =  0.82f;
    float rAtas  =  0.28f;

    #define GAUN_R(y) (rBawah + (rAtas - rBawah) * ((y) - yBawah) / (yAtas - yBawah))

    glPushMatrix();
        glTranslatef(0, -2.05f, 0);
        glScalef(1.4f, 0.08f, 1.0f);
        glColor3f(0, 0, 0);
        glutSolidSphere(0.85, 14, 14);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0, yBawah, 0);
        glRotatef(-90, 1, 0, 0);
        glColor3f(0.90f, 0.90f, 0.92f);
        gluCylinder(q, rBawah, rAtas, yAtas - yBawah, 24, 8);
    glPopMatrix();

    // Tutup bawah gaun
    glPushMatrix();
        glTranslatef(0, yBawah, 0);
        glRotatef(90, 1, 0, 0);
        glColor3f(0.87f, 0.87f, 0.89f);
        gluDisk(q, 0, rBawah, 24, 1);
    glPopMatrix();

    // kek ada robekan gaun gitu
    for (int i = 0; i < 12; i++) {
        float sudut = (float)i / 12.0f * 2.0f * PI;
        float rx = 0.72f * cosf(sudut);
        float rz = 0.72f * sinf(sudut);
        glPushMatrix();
            glTranslatef(rx, yBawah, rz);
            glRotatef(90, 1, 0, 0);
            glColor3f(0.84f, 0.84f, 0.86f);
            glutSolidCone(0.09, 0.30, 5, 2);
        glPopMatrix();
    }

    // bercak merah
    struct Bercak { float y; float sudut; float ukuran; };
    Bercak bercak[] = {
        { -0.55f,  0.25f, 0.10f },
        { -0.90f, -0.30f, 0.09f },
        { -1.25f,  0.10f, 0.08f },
        { -0.75f,  0.80f, 0.07f },
    };
    for (int i = 0; i < 4; i++) {
        float y    = bercak[i].y;
        float ang  = bercak[i].sudut;
        float r    = GAUN_R(y);
        float bx   = sinf(ang) * r;
        float bz   = cosf(ang) * r;
        glPushMatrix();
            glTranslatef(bx, y, bz);
            glColor3f(0.62f, 0.0f, 0.0f);
            glutSolidSphere(bercak[i].ukuran, 8, 8);
        glPopMatrix();
    }
    #undef GAUN_R

    // kepala
    glPushMatrix();
        glTranslatef(0, 0.52f, 0);
        glColor3f(0.07f, 0.04f, 0.04f);
        glutSolidSphere(0.55, 20, 20);
    glPopMatrix();

    // rambut depan
    glColor3f(0.04f, 0.02f, 0.02f);

    float xStart = -0.44f, xEnd = 0.44f;
    float zStart =  0.20f, zEnd = 0.52f;
    int   kolomH = 9, barisH = 4;
    for (int baris = 0; baris < barisH; baris++) {
        for (int kolom = 0; kolom < kolomH; kolom++) {
            float hx = xStart + (xEnd - xStart) * kolom / (kolomH - 1);
            float hz = zStart + (zEnd - zStart) * baris / (barisH - 1);
            float pj = 1.55f + 0.35f * (hz - zStart) / (zEnd - zStart);
            glPushMatrix();
                glTranslatef(hx, 0.56f, hz);
                glRotatef(90, 1, 0, 0);
                gluCylinder(q, 0.05f, 0.015f, pj, 5, 2);
            glPopMatrix();
        }
    }

    for (int i = 0; i < 6; i++) {
        float frac = (float)i / 5.0f;
        float hz   = 0.10f + frac * 0.35f;
        float pj   = 1.2f + frac * 0.3f;

        glPushMatrix();
            glTranslatef(-0.50f, 0.55f, hz);
            glRotatef(90, 1, 0, 0);
            gluCylinder(q, 0.045f, 0.012f, pj, 5, 2);
        glPopMatrix();

        glPushMatrix();
            glTranslatef( 0.50f, 0.55f, hz);
            glRotatef(90, 1, 0, 0);
            gluCylinder(q, 0.045f, 0.012f, pj, 5, 2);
        glPopMatrix();
    }

    // Helai belakang
    for (int i = -3; i <= 3; i++) {
        glPushMatrix();
            glTranslatef(i * 0.15f, 0.56f, -0.48f);
            glRotatef(90, 1, 0, 0);
            gluCylinder(q, 0.04f, 0.01f, 0.9f, 5, 2);
        glPopMatrix();
    }

    // tangan kiri
    glPushMatrix();
        glTranslatef(-0.28f, -0.15f, 0.15f);
        glRotatef(90, 0, 1, 0);
        glRotatef(-20, 0, 0, 1);
        glColor3f(0.88f, 0.88f, 0.90f);
        gluCylinder(q, 0.10f, 0.08f, 0.65f, 12, 3);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-0.85f, -0.25f, 0.35f);
        glColor3f(0.86f, 0.86f, 0.88f);
        glutSolidSphere(0.12f, 10, 10);
        // Jari-jari kurus panjang
        for (int j = 0; j < 4; j++) {
            float ja = -0.3f + j * 0.2f;
            glPushMatrix();
                glTranslatef(ja * 0.1f, 0, 0);
                glRotatef(-15, 1, 0, 0);
                glColor3f(0.84f, 0.84f, 0.86f);
                gluCylinder(q, 0.022f, 0.006f, 0.28f, 5, 1);
            glPopMatrix();
        }
    glPopMatrix();

    // tangan kanan
    glPushMatrix();
        glTranslatef(0.28f, -0.15f, 0.15f);
        glRotatef(-90, 0, 1, 0);
        glRotatef(-20, 0, 0, 1);
        glColor3f(0.88f, 0.88f, 0.90f);
        gluCylinder(q, 0.10f, 0.08f, 0.65f, 12, 3);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.85f, -0.25f, 0.35f);
        glColor3f(0.86f, 0.86f, 0.88f);
        glutSolidSphere(0.12f, 10, 10);
        for (int j = 0; j < 4; j++) {
            float ja = -0.3f + j * 0.2f;
            glPushMatrix();
                glTranslatef(ja * 0.1f, 0, 0);
                glRotatef(-15, 1, 0, 0);
                glColor3f(0.84f, 0.84f, 0.86f);
                gluCylinder(q, 0.022f, 0.006f, 0.28f, 5, 1);
            glPopMatrix();
        }
    glPopMatrix();

    gluDeleteQuadric(q);
}

void drawGhostDemo(float rotX, float rotY, float rotZ) {
    glPushMatrix();
        glRotatef(rotX, 1, 0, 0);
        glRotatef(rotY, 0, 1, 0);
        glRotatef(rotZ, 0, 0, 1);
        drawGhostBody();
    glPopMatrix();
}

void drawGhostGame(float px, float py, float faceY) {
    glPushMatrix();
        glTranslatef(px, 0.8f, py);
        glRotatef(faceY, 0, 1, 0);
        glScalef(0.45f, 0.45f, 0.45f);
        drawGhostBody();
    glPopMatrix();
}

// cek posisi dinding
static bool isWallGhost(float wx, float wy,
                         const char map[][MAZE_WIDTH], int mapW, int mapH, float tile) {
    int col = (int)(wx / tile);
    int row = (int)(wy / tile);
    if (col < 0 || col >= mapW || row < 0 || row >= mapH) return true;
    return map[row][col] == '#';
}

static float dist2(float ax, float ay, float bx, float by) {
    float dx = ax - bx, dy = ay - by;
    return sqrtf(dx*dx + dy*dy);
}

// bentuk awal ghost
void initGhost(Ghost& g,
               const char map[][MAZE_WIDTH], int mapW, int mapH, float tile) {
    // Spawn ghost at a random free cell (non-wall '#').
    // int tries = 0;
    // int maxTries = mapW * mapH * 2;
    // bool placed = false;
    // while (tries < maxTries) {
    //     int rr = rand() % mapH;
    //     int cc = rand() % mapW;
    //     if (map[rr][cc] != '#') {
    //         g.x = (cc + 0.5f) * tile;
    //         g.y = (rr + 0.5f) * tile;
    //         placed = true;
    //         break;
    //     }
    //     tries++;
    // }
    // if (!placed) {
    //     g.x = 7.5f * tile;
    //     g.y = 1.5f * tile;
    // }
    g.x = 7.5f * tile;
    g.y = 1.5f * tile;
    g.rotY = 0;

    g.speedPatrol = 1.2f; // jalan biasa
    g.speedChase  = 1.2f; // ngejer

    g.sightRange  = 20.0f;   //range jarak
    g.loseRange   = 8.0f;  // udah lewat
    g.chaseTimer  = 0.0f;

    g.mode = PATROL; //state awal

    float T = tile;
    Vec2 wp[] = {
        {7.5f*T, 1.5f*T},
        {7.5f*T, 3.5f*T},
        {9.5f*T, 3.5f*T},
        {9.5f*T, 5.5f*T},
        {7.5f*T, 5.5f*T},
        {5.5f*T, 5.5f*T},
        {5.5f*T, 3.5f*T},
        {7.5f*T, 1.5f*T}
    };
    g.numWP    = 8;
    g.targetWP = 1;
    for (int i = 0; i < g.numWP; i++)
        g.waypoints[i] = wp[i];
}

// ghost ke satu titik ngejer
static void moveGhostToward(Ghost& g, float tx, float ty, float speed, float dt,
                             const char map[][MAZE_WIDTH], int mapW, int mapH, float tile) {
    float dx    = tx - g.x;
    float dy    = ty - g.y;
    float jarak = sqrtf(dx*dx + dy*dy);
    if (jarak < 0.05f) return;

    float nx   = dx / jarak;
    float ny   = dy / jarak;
    float step = speed * dt;
    float rad  = 0.4f;

    float nx2 = g.x + nx * step;
    if (!isWallGhost(nx2 + rad * nx, g.y, map, mapW, mapH, tile) &&
        !isWallGhost(nx2 - rad * nx, g.y, map, mapW, mapH, tile))
        g.x = nx2;

    float ny2 = g.y + ny * step;
    if (!isWallGhost(g.x, ny2 + rad * ny, map, mapW, mapH, tile) &&
        !isWallGhost(g.x, ny2 - rad * ny, map, mapW, mapH, tile))
        g.y = ny2;

    g.rotY = atan2f(ny, nx) * 180.0f / 3.14159f - 90.0f;
}

void updateGhost(Ghost& g, float dt,
                 float playerX, float playerZ,
                 const char map[][MAZE_WIDTH], int mapW, int mapH, float tile) {
    float playerDist = dist2(g.x, g.y, playerX, playerZ);

    if (g.mode == PATROL) {
        if (playerDist < g.sightRange) {
            g.mode = CHASE;
        } else {
            Vec2 target = g.waypoints[g.targetWP];
            moveGhostToward(g, target.x, target.y, g.speedPatrol, dt, map, mapW, mapH, tile);
            if (dist2(g.x, g.y, target.x, target.y) < 0.3f) {
                g.targetWP = (g.targetWP + 1) % g.numWP;
            }
        }
    } else if (g.mode == CHASE) {
        moveGhostToward(g, playerX, playerZ, g.speedChase, dt, map, mapW, mapH, tile);
        if (playerDist > g.loseRange) {
            g.chaseTimer += dt;
            if (g.chaseTimer > 2.0f) {
                g.mode = RETURN;
            }
        } else {
            g.chaseTimer = 0.0f;
        }
    } else if (g.mode == RETURN) {
        Vec2 target = g.waypoints[0];
        moveGhostToward(g, target.x, target.y, g.speedPatrol, dt, map, mapW, mapH, tile);
        if (dist2(g.x, g.y, target.x, target.y) < 0.3f) {
            g.mode = PATROL;
            g.targetWP = 1;
        }
    }
}

bool ghostCatchesPlayer(const Ghost& g,
                        float playerX, float playerZ,
                        float catchRadius) {
    float d = dist2(g.x, g.y, playerX, playerZ);
    return d < catchRadius;
}
