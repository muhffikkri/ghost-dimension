// Nama File : main.cpp
// Deskripsi : Titik masuk aplikasi dan loop utama rendering game.
// Tanggal Dibuat : 24 Maret 2026

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "shared/Config.h"
#include "shared/Camera.h"
#include "shared/Environment.h"
#include "shared/Entity.h"

// Inisialisasi Objek Global
// playerCam didefinisikan di Camera.cpp

enum AppMode {
    MODE_GAME,
    MODE_ENV_DEMO,
    MODE_ENTITY_DEMO
};

static AppMode g_mode = MODE_GAME;

static void clearGameInput() {
    playerCam.moveUp = false;
    playerCam.moveDown = false;
    playerCam.moveLeft = false;
    playerCam.moveRight = false;
    playerCam.lookUp = false;
    playerCam.lookDown = false;
    playerCam.lookLeft = false;
    playerCam.lookRight = false;
}

/* ===================== ENV DEMO STATE ===================== */
static float g_envYaw  = 0.0f;
static float g_envX    = 3.0f;
static float g_envZ    = 3.0f;

/* ===================== ENTITY DEMO STATE ===================== */
static float g_rotX = 20.0f;
static float g_rotY = -30.0f;
static float g_rotZ = 0.0f;
static int   g_viewMode   = 7;
static int   g_entityMode = 0; // 0=Hantu, 1=Koin, 2=Kunci

static void drawText2D(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* p = text; *p != '\0'; ++p) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);
    }
}

static void drawModeBanner(const char* text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 0.9f, 0.2f);
    drawText2D(16.0f, WINDOW_HEIGHT - 20.0f, text);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

static void resetEntityDemoState() {
    g_rotX = 0.0f;
    g_rotY = 0.0f;
    g_rotZ = 0.0f;
    g_viewMode = 7;
    g_entityMode = 0;
}

static void applyEntityDemoCamera() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    float aspect = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (g_viewMode == 7) {
        gluPerspective(45.0, aspect, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 5,   0, 0, 0,   0, 1, 0);
    } else {
        float s = 2.5f;
        glOrtho(-s*aspect, s*aspect, -s, s, -20, 20);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        switch (g_viewMode) {
            case 1: gluLookAt( 0,  0,  5,  0, 0, 0,  0, 1,  0); break;
            case 2: gluLookAt( 0,  0, -5,  0, 0, 0,  0, 1,  0); break;
            case 3: gluLookAt(-5,  0,  0,  0, 0, 0,  0, 1,  0); break;
            case 4: gluLookAt( 5,  0,  0,  0, 0, 0,  0, 1,  0); break;
            case 5: gluLookAt( 0,  5,  0,  0, 0, 0,  0, 0, -1); break;
            case 6: gluLookAt( 0, -5,  0,  0, 0, 0,  0, 0,  1); break;
        }
    }
}

static void drawEntityDemoHud() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const char* namaEntity[] = {"HANTU (G)", "KOIN (C)", "KUNCI (K)"};
    const char* namaView[]   = {"", "Depan(1)", "Belakang(2)", "Kiri(3)",
                                 "Kanan(4)", "Atas(5)", "Bawah(6)", "Perspektif(7)"};
    char info[128];
    sprintf(info, "Entity: %s  |  View: %s  |  Rot X=%.0f Y=%.0f Z=%.0f",
            namaEntity[g_entityMode], namaView[g_viewMode], g_rotX, g_rotY, g_rotZ);

    glColor3f(1.0f, 1.0f, 0.0f);
    glRasterPos2i(10, h - 22);
    for (int i = 0; info[i]; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[i]);

    glColor3f(0.65f, 0.65f, 0.65f);
    glRasterPos2i(10, 10);
    const char* help = "Arrow=Rot XY | Z/X=Rot Z | 1-7=View | G/C/K=Entity | R=Reset | F1/F2/F3=Mode";
    for (int i = 0; help[i]; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help[i]);
}

static void displayGame() {
    playerCam.apply();

    camX = playerCam.x;
    camY = playerCam.y;
    camZ = playerCam.z;
    float cp = cosf(playerCam.pitch);
    float sp = sinf(playerCam.pitch);
    lookX = playerCam.x + cosf(playerCam.angle) * cp;
    lookY = playerCam.y + sp;
    lookZ = playerCam.z + sinf(playerCam.angle) * cp;

    setupFlashlight();
    setupFog();
    renderEnvironment();
    drawMinimap();

    syncPlayerPosition(playerCam.x, playerCam.z);
    updateItems();
    drawHUD();
    drawModeBanner("MODE: GAME (F1=GAME | F2=ENV DEMO | F3=ENTITY DEMO)");
}

static void displayEnvDemo() {
    float rad = g_envYaw * 3.14159265f / 180.0f;
    camX  = g_envX;
    camY  = 1.6f;
    camZ  = g_envZ;
    lookX = g_envX + cosf(rad);
    lookY = 1.6f;
    lookZ = g_envZ + sinf(rad);

    gluLookAt(camX, camY, camZ,
              lookX, lookY, lookZ,
              0, 1, 0);

    setupFlashlight();
    setupFog();
    renderEnvironment();
    drawMinimap();
    drawModeBanner("MODE: ENV DEMO (W/S/A/D move, F1/F2/F3=Mode)");
}

static void displayEntityDemo() {
    applyEntityDemoCamera();

    glRotatef(g_rotX, 1, 0, 0);
    glRotatef(g_rotY, 0, 1, 0);
    glRotatef(g_rotZ, 0, 0, 1);

    if      (g_entityMode == 0) drawGhostDemo(0, 0, 0);
    else if (g_entityMode == 1) drawCoinDemo(0, 0, 0);
    else if (g_entityMode == 2) drawKeyDemo(0, 0, 0);

    drawEntityDemoHud();
}

static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (g_mode == MODE_GAME) {
        displayGame();
    } else if (g_mode == MODE_ENV_DEMO) {
        displayEnvDemo();
    } else {
        displayEntityDemo();
    }

    glutSwapBuffers();
}

static void timer(int v) {
    static float lastTime = 0.0f;
    float now = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float dt = (lastTime > 0.0f) ? (now - lastTime) : 0.016f;
    if (dt < 0.0f) dt = 0.0f;
    if (dt > 0.1f) dt = 0.1f;
    g_time += dt;
    lastTime = now;

    if (g_mode == MODE_GAME) {
        playerCam.handleInput(dt);
        updateGhost(playerCam.x, playerCam.z);
    }

    // TODO: Update semua logika game (Ghost AI, Collision) setiap frame
    glutPostRedisplay();
    glutTimerFunc(1000/60, timer, 0);
}

static void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;

    if (key == 27) {
        exit(0);
    }

    if (g_mode == MODE_ENTITY_DEMO) {
        switch (key) {
            case 'g': case 'G': g_entityMode = 0; resetEntityDemoState(); break;
            case 'c': case 'C': g_entityMode = 1; resetEntityDemoState(); break;
            case 'k': case 'K': g_entityMode = 2; resetEntityDemoState(); break;

            case '1': g_viewMode = 1; g_rotX = g_rotY = g_rotZ = 0.0f; break;
            case '2': g_viewMode = 2; g_rotX = g_rotY = g_rotZ = 0.0f; break;
            case '3': g_viewMode = 3; g_rotX = g_rotY = g_rotZ = 0.0f; break;
            case '4': g_viewMode = 4; g_rotX = g_rotY = g_rotZ = 0.0f; break;
            case '5': g_viewMode = 5; g_rotX = g_rotY = g_rotZ = 0.0f; break;
            case '6': g_viewMode = 6; g_rotX = g_rotY = g_rotZ = 0.0f; break;
            case '7': g_viewMode = 7; break;

            case 'z': case 'Z': g_rotZ += 5.0f; g_viewMode = 7; break;
            case 'x': case 'X': g_rotZ -= 5.0f; g_viewMode = 7; break;

            case 'r': case 'R': g_rotX = g_rotY = g_rotZ = 0.0f; break;
        }
    } else if (g_mode == MODE_ENV_DEMO) {
        const float STEP = 0.15f;
        float rad = g_envYaw * 3.14159265f / 180.0f;

        switch (key) {
            case 'w': case 'W': {
                float nx = g_envX + cosf(rad) * STEP;
                float nz = g_envZ + sinf(rad) * STEP;
                if (!checkCollision(nx, nz)) {
                    g_envX = nx; g_envZ = nz;
                }
                break;
            }
            case 's': case 'S': {
                float nx = g_envX - cosf(rad) * STEP;
                float nz = g_envZ - sinf(rad) * STEP;
                if (!checkCollision(nx, nz)) {
                    g_envX = nx; g_envZ = nz;
                }
                break;
            }
            case 'a': case 'A': g_envYaw -= 3.0f; break;
            case 'd': case 'D': g_envYaw += 3.0f; break;
        }
    } else {
        switch (key) {
            case 'w': case 'W': playerCam.moveUp = true; break;
            case 's': case 'S': playerCam.moveDown = true; break;
            case 'a': case 'A': playerCam.moveLeft = true; break;
            case 'd': case 'D': playerCam.moveRight = true; break;
        }
    }
}

static void keyboardUp(unsigned char key, int x, int y) {
    (void)x; (void)y;

    if (g_mode != MODE_GAME) {
        return;
    }

    switch (key) {
        case 'w': case 'W': playerCam.moveUp = false; break;
        case 's': case 'S': playerCam.moveDown = false; break;
        case 'a': case 'A': playerCam.moveLeft = false; break;
        case 'd': case 'D': playerCam.moveRight = false; break;
    }
}

static void specialKey(int key, int x, int y) {
    (void)x; (void)y;

    if (key == GLUT_KEY_F1) {
        g_mode = MODE_GAME;
        clearGameInput();
        return;
    }
    if (key == GLUT_KEY_F2) {
        g_mode = MODE_ENV_DEMO;
        clearGameInput();
        return;
    }
    if (key == GLUT_KEY_F3) {
        g_mode = MODE_ENTITY_DEMO;
        resetEntityDemoState();
        clearGameInput();
        return;
    }

    if (g_mode == MODE_ENTITY_DEMO) {
        switch (key) {
            case GLUT_KEY_UP:    g_rotX -= 5.0f; break;
            case GLUT_KEY_DOWN:  g_rotX += 5.0f; break;
            case GLUT_KEY_LEFT:  g_rotY -= 5.0f; break;
            case GLUT_KEY_RIGHT: g_rotY += 5.0f; break;
        }
    } else if (g_mode == MODE_GAME) {
        switch (key) {
            case GLUT_KEY_UP:    playerCam.lookUp = true; break;
            case GLUT_KEY_DOWN:  playerCam.lookDown = true; break;
            case GLUT_KEY_LEFT:  playerCam.lookLeft = true; break;
            case GLUT_KEY_RIGHT: playerCam.lookRight = true; break;
        }
    }
}

static void specialKeyUp(int key, int x, int y) {
    (void)x; (void)y;

    if (g_mode != MODE_GAME) {
        return;
    }

    switch (key) {
        case GLUT_KEY_UP:    playerCam.lookUp = false; break;
        case GLUT_KEY_DOWN:  playerCam.lookDown = false; break;
        case GLUT_KEY_LEFT:  playerCam.lookLeft = false; break;
        case GLUT_KEY_RIGHT: playerCam.lookRight = false; break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("GTI A1 Maze Runner 2026");

    generateMaze();
    initEnvironment();
    initEntities();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
