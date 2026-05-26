// ============================================================
// Key.cpp
// ============================================================

#include "../../include/Key.h"
#include <math.h>

static void drawKeyShape() {
    GLUquadric* q = gluNewQuadric();
    glColor3f(0.76f, 0.68f, 0.26f);

    float rTube     = 0.13f;
    float rRing     = 0.40f;
    float yRing     = -0.35f;

    // Batang ring
    float yBatangMulai = yRing + rRing - rTube;
    float panjangBatang = 1.5f;
    float yBatangAtas   = yBatangMulai + panjangBatang;

    // ring, yang bulet
    glPushMatrix();
        glTranslatef(0, yRing, 0);
        glutSolidTorus(rTube, rRing, 18, 36);
    glPopMatrix();

    // batang
    glPushMatrix();
        glTranslatef(0, yBatangMulai, 0);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(q, 0.11, 0.11, panjangBatang, 16, 2);
    glPopMatrix();

    // Tutup bawah batang
    glPushMatrix();
        glTranslatef(0, yBatangMulai, 0);
        glRotatef(90, 1, 0, 0);
        gluDisk(q, 0, 0.11, 16, 1);
    glPopMatrix();

    // kepala atas
    glPushMatrix();
        glTranslatef(0, yBatangAtas, 0);
        glutSolidSphere(0.14, 14, 14);
    glPopMatrix();

    // gigi kunci
    float yGigi1 = yBatangMulai + panjangBatang * 0.65f;
    float yGigi2 = yBatangMulai + panjangBatang * 0.38f;

    // Gigi 1
    glPushMatrix();
        glTranslatef(0.11f + 0.14f, yGigi1, 0);
        glScalef(1.0f, 0.18f, 0.85f);
        glutSolidCube(0.30f);
    glPopMatrix();

    // Gigi 2
    glPushMatrix();
        glTranslatef(0.11f + 0.11f, yGigi2, 0);
        glScalef(1.0f, 0.15f, 0.85f);
        glutSolidCube(0.24f);
    glPopMatrix();

    gluDeleteQuadric(q);
}

void drawKeyDemo(float rotX, float rotY, float rotZ) {
    glPushMatrix();
        glRotatef(rotX, 1, 0, 0);
        glRotatef(rotY, 0, 1, 0);
        glRotatef(rotZ, 0, 0, 1);
        drawKeyShape();
    glPopMatrix();
}

void drawKeyGame(float px, float py, float floatOffset, float spinAngle) {
    glPushMatrix();
        // glTranslatef(px, 0.65f + floatOffset, py);
        glTranslatef(px, 0.65f, py);
        glRotatef(floatOffset, 0.5f, 0.5f, 0.5f);
        // glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
        glScalef(0.45f, 0.45f, 0.45f);
        drawKeyShape();
    glPopMatrix();
}

void initKey(Key& k, const char map[][MAZE_WIDTH], int mapW, int mapH, float tile) {
    k.aktif       = false;
    k.floatOffset = 0;
    k.floatTime   = 0;
    k.spinAngle   = 0;

    for (int row = 0; row < mapH; row++) {
        for (int col = 0; col < mapW; col++) {
            if (map[row][col] == 'K') {
                k.x     = (col + 0.5f) * tile;
                k.y     = (row + 0.5f) * tile;
                k.aktif = true;
                return;
            }
        }
    }
}

// kunci kayak gerak gitu lah
void updateKey(Key& k, float dt) {
    if (!k.aktif) return;

    // k.floatTime   += 2.0f * dt;
    k.floatOffset  = 0.20f * sinf(k.floatTime * 2.1f);
    // if (dt > 0.05f)
    //     dt = 0.05f;
    // k.floatOffset = 0.22f * sinf(k.floatTime * 1.8f);
    k.spinAngle   += 0.1f * sinf(dt);
    // if (k.spinAngle >= 1.0f)
    //     k.spinAngle -= 0.1f;
}
