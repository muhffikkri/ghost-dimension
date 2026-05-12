/*
    main.cpp - Abyssal Maze (Yu Ming Jie)
    Kontrol: W/S = maju/mundur | A/D = putar | ESC = keluar
*/

#include <GL/glut.h>
#include <cmath>
#include "Config.h"
#include "Environment.h"

/* ===================== STATE ===================== */
static float g_lastTime = 0.0f;
static float g_yaw  = 0.0f;   /* sudut pandang horizontal (derajat) */
static float g_mcx  = 3.0f;   /* posisi pemain X (center cell 1,1) */
static float g_mcz  = 3.0f;   /* posisi pemain Z */

/* ===================== HUD TEXT ===================== */
static void drawHUD(float px, float py, const char* str) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0) h = 1;

    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);      glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    glColor3f(0.95f, 0.82f, 0.12f); /* warna teks kuning */
    glRasterPos2f(px, py);
    while (*str) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)(*str));
        ++str;
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

/* ===================== PROYEKSI ===================== */
static void setPerspective() {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0) h = 1;
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(65.0f, (float)w/(float)h, 0.1f, 120.0f);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
}

/* ===================== COLLISION ===================== */
/* Return true jika posisi (wx, wz) berada di dalam dinding */
static bool isWall(float wx, float wz) {
    const float S = 2.0f;
    int col = (int)(wx / S);
    int row = (int)(wz / S);
    /* Anggap di luar batas = dinding */
    if (row < 0 || row >= MAZE_HEIGHT || col < 0 || col >= MAZE_WIDTH)
        return true;
    return mazeMatrix[row][col] == 1;
}

/* ===================== RENDER ===================== */
static void showMaze() {
    setPerspective();

    /* Update posisi kamera global dari state pemain */
    float rad = g_yaw * 3.14159265f / 180.0f;
    camX  = g_mcx;
    camY  = 1.6f;   /* tinggi mata pemain */
    camZ  = g_mcz;
    lookX = g_mcx + cosf(rad);
    lookY = 1.6f;
    lookZ = g_mcz + sinf(rad);

    gluLookAt(camX, camY, camZ,
              lookX, lookY, lookZ,
              0, 1, 0);

    setupFlashlight();  /* spotlight cone dari kamera */
    setupFog();         /* fog tipis */
    renderEnvironment(); /* lantai + dinding + obor + bayangan */

    /* Teks HUD atas dan bawah */
    int wh = glutGet(GLUT_WINDOW_HEIGHT);
    drawHUD(12, (float)(wh-28), "Abyssal Maze  |  W/S maju  A/D putar  ESC keluar");
    drawHUD(12, 14, "GL_LIGHT0 spotlight + GL_LIGHT1-6 obor flicker + shadow lantai");

    drawMinimap(); /* minimap kanan atas */
}

/* ===================== GLUT CALLBACKS ===================== */
static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    showMaze();
    glutSwapBuffers();
}

static void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
}

static void idle() {
    /* Update timer global untuk animasi flicker & fog */
    float now  = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    g_time    += now - g_lastTime;
    g_lastTime = now;
    glutPostRedisplay();
}

static void keyboard(unsigned char key, int mx, int my) {
    (void)mx; (void)my;

    const float STEP = 0.15f;  /* jarak langkah per keypress */
    const float RAD  = 0.4f;   /* radius collision pemain */

    float rad = g_yaw * 3.14159265f / 180.0f;

    switch (key) {
        case 'w': case 'W': {
            /* Maju: coba gerak ke depan, cek 4 titik collision */
            float nx = g_mcx + cosf(rad) * STEP;
            float nz = g_mcz + sinf(rad) * STEP;
            if (!isWall(nx+RAD, nz)     && !isWall(nx-RAD, nz) &&
                !isWall(nx,     nz+RAD) && !isWall(nx,     nz-RAD))
            { g_mcx = nx; g_mcz = nz; }
            break;
        }
        case 's': case 'S': {
            /* Mundur */
            float nx = g_mcx - cosf(rad) * STEP;
            float nz = g_mcz - sinf(rad) * STEP;
            if (!isWall(nx+RAD, nz)     && !isWall(nx-RAD, nz) &&
                !isWall(nx,     nz+RAD) && !isWall(nx,     nz-RAD))
            { g_mcx = nx; g_mcz = nz; }
            break;
        }
        case 'a': case 'A': g_yaw -= 3.0f; break; /* putar kiri */
        case 'd': case 'D': g_yaw += 3.0f; break; /* putar kanan */
        case 27: exit(0); break; /* ESC = keluar */
    }
}

/* ===================== MAIN ===================== */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1024, 640);
    glutCreateWindow("Ghost Dimension");

    generateMaze();    /* inisialisasi maze (hardcoded di Config.cpp) */
    initEnvironment(); /* setup tekstur, lighting, fog */

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
