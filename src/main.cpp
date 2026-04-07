// Nama File : main.cpp
// Deskripsi : Titik masuk aplikasi dan loop utama rendering game.
// Tanggal Dibuat : 24 Maret 2026

#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Config.h"
#include "Camera.h"
#include "Environment.h"
#include "Entity.h"

// Inisialisasi Objek Global
Camera playerCam = {-3.5f, 1.7f, -3.5f, 0.2f, 0.0f};

static bool gKeys[256];
static bool gSpecialKeys[256];

static bool isWallAt(float wx, float wz) {
    const float xOffset = 10.0f * 0.5f;
    const float zOffset = 10.0f * 0.5f;

    const int c = (int)floor(wx + xOffset);
    const int r = (int)floor(wz + zOffset);

    if (r < 0 || r >= 10 || c < 0 || c >= 10) return true;
    return maze[r][c] == 1;
}

static bool canMoveTo(float nx, float nz) {
    const float radius = 0.14f;
    if (isWallAt(nx + radius, nz + radius)) return false;
    if (isWallAt(nx - radius, nz + radius)) return false;
    if (isWallAt(nx + radius, nz - radius)) return false;
    if (isWallAt(nx - radius, nz - radius)) return false;
    return true;
}

static void updateMovement() {
    if (isGameOver || isGameWin) return;

    const float moveStep = PLAYER_SPEED;
    const float turnStep = 0.05f;

    if (gKeys['a'] || gKeys['A'] || gSpecialKeys[GLUT_KEY_LEFT]) {
        playerCam.angle -= turnStep;
    }
    if (gKeys['d'] || gKeys['D'] || gSpecialKeys[GLUT_KEY_RIGHT]) {
        playerCam.angle += turnStep;
    }

    float move = 0.0f;
    if (gKeys['w'] || gKeys['W'] || gSpecialKeys[GLUT_KEY_UP]) move += moveStep;
    if (gKeys['s'] || gKeys['S'] || gSpecialKeys[GLUT_KEY_DOWN]) move -= moveStep;

    if (fabs(move) > 0.0001f) {
        const float dx = cos(playerCam.angle) * move;
        const float dz = sin(playerCam.angle) * move;

        const float nextX = playerCam.x + dx;
        const float nextZ = playerCam.z + dz;

        if (canMoveTo(nextX, playerCam.z)) {
            playerCam.x = nextX;
        }
        if (canMoveTo(playerCam.x, nextZ)) {
            playerCam.z = nextZ;
        }
    }
}

static void keyDown(unsigned char key, int, int) {
    gKeys[(unsigned char)key] = true;
    if (key == 27) {
        exit(0);
    }
}

static void keyUp(unsigned char key, int, int) {
    gKeys[(unsigned char)key] = false;
}

static void specialDown(int key, int, int) {
    if (key >= 0 && key < 256) {
        gSpecialKeys[key] = true;
    }
}

static void specialUp(int key, int, int) {
    if (key >= 0 && key < 256) {
        gSpecialKeys[key] = false;
    }
}

static void setupScene() {
    glClearColor(0.06f, 0.06f, 0.09f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

static void reshape(int w, int h) {
    if (h <= 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    playerCam.apply();
    setupLighting(); // Render Lighting 
    
    drawMaze();      // Render Lingkungan
    updateItems();   // Render Koin & Kunci
    drawHUD();
    
    glutSwapBuffers();
}

void timer(int v) {
    updateMovement();
    glutPostRedisplay();
    glutTimerFunc(1000/60, timer, 0);
}

int main(int argc, char** argv) {
    memset(gKeys, 0, sizeof(gKeys));
    memset(gSpecialKeys, 0, sizeof(gSpecialKeys));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(80, 50);
    glutCreateWindow("GTI A1 Maze Runner 2026");

    setupScene();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}