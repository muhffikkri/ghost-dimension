// Nama File : Environment.cpp
// Deskripsi : Implementasi rendering lingkungan, tekstur, dan pencahayaan.
// Tanggal Dibuat : 24 Maret 2026

#include "Environment.h"
#include "Config.h"
#include "Camera.h"
#include <GL/glut.h>
#include <math.h>

static GLuint gWallTex = 0;
static GLuint gFloorTex = 0;
static bool gTextureLoaded = false;

static const int MAZE_ROWS = 10;
static const int MAZE_COLS = 10;
static const float TILE_SIZE = 1.0f;
static const float WALL_HEIGHT = 2.3f;
static const float WALL_HALF_WIDTH = 0.38f;
static const float X_OFFSET = MAZE_COLS * 0.5f;
static const float Z_OFFSET = MAZE_ROWS * 0.5f;

static float worldXFromCol(int c) {
    return (c + 0.5f) * TILE_SIZE - X_OFFSET;
}

static float worldZFromRow(int r) {
    return (r + 0.5f) * TILE_SIZE - Z_OFFSET;
}

static void drawWallBlock(float cx, float cz, float halfW, float height) {
    const float x0 = cx - halfW;
    const float x1 = cx + halfW;
    const float z0 = cz - halfW;
    const float z1 = cz + halfW;
    const float y0 = 0.0f;
    const float y1 = height;

    glBegin(GL_QUADS);

    // Front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x0, y0, z1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x1, y0, z1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x1, y1, z1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x0, y1, z1);

    // Back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y0, z0);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x0, y0, z0);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x0, y1, z0);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x1, y1, z0);

    // Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x0, y0, z0);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x0, y0, z1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x0, y1, z1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x0, y1, z0);

    // Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y0, z1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x1, y0, z0);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x1, y1, z0);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x1, y1, z1);

    // Top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x0, y1, z0);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x1, y1, z0);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x1, y1, z1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x0, y1, z1);

    glEnd();
}

// TODO: Gunakan glTexImage2D atau library SOIL/stb_image untuk memuat gambar tekstur 
void loadTextures() {
    if (gTextureLoaded) return;

    const int texSize = 64;
    GLubyte wallData[texSize][texSize][3];
    GLubyte floorData[texSize][texSize][3];

    for (int y = 0; y < texSize; ++y) {
        for (int x = 0; x < texSize; ++x) {
            const int brick = ((x / 8) + (y / 8)) % 2;
            const int grain = ((x * 13 + y * 7) % 18) - 9;

            int wr = brick ? 62 : 48;
            int wg = brick ? 63 : 47;
            int wb = brick ? 71 : 55;
            wr += grain;
            wg += grain;
            wb += grain;

            if (wr < 20) wr = 20; if (wr > 100) wr = 100;
            if (wg < 20) wg = 20; if (wg > 100) wg = 100;
            if (wb < 24) wb = 24; if (wb > 110) wb = 110;

            wallData[y][x][0] = (GLubyte)wr;
            wallData[y][x][1] = (GLubyte)wg;
            wallData[y][x][2] = (GLubyte)wb;

            const int check = ((x / 6) + (y / 6)) % 2;
            const int dust = ((x * 5 + y * 11) % 22) - 11;

            int fr = check ? 34 : 26;
            int fg = check ? 31 : 24;
            int fb = check ? 28 : 22;
            fr += dust;
            fg += dust;
            fb += dust;

            if (fr < 8) fr = 8; if (fr > 60) fr = 60;
            if (fg < 8) fg = 8; if (fg > 52) fg = 52;
            if (fb < 8) fb = 8; if (fb > 48) fb = 48;

            floorData[y][x][0] = (GLubyte)fr;
            floorData[y][x][1] = (GLubyte)fg;
            floorData[y][x][2] = (GLubyte)fb;
        }
    }

    glGenTextures(1, &gWallTex);
    glBindTexture(GL_TEXTURE_2D, gWallTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, wallData);

    glGenTextures(1, &gFloorTex);
    glBindTexture(GL_TEXTURE_2D, gFloorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, floorData);

    glBindTexture(GL_TEXTURE_2D, 0);
    gTextureLoaded = true;
}

// TODO: Implementasikan Lighting (GL_LIGHT0 untuk Senter, GL_LIGHT1 dst untuk Obor) 
void setupLighting() {
    const float px = playerCam.x;
    const float py = playerCam.y;
    const float pz = playerCam.z;
    const float dx = cos(playerCam.angle);
    const float dz = sin(playerCam.angle);

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    const float t = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    const float flicker = 0.82f + 0.18f * sin(t * 6.0f);
    GLfloat ambientGlobal[] = {0.20f, 0.20f, 0.23f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientGlobal);

    // Senter (Spotlight) mengikuti kamera pemain.
    GLfloat spotPos[] = { px, py, pz, 1.0f };
    GLfloat spotDir[] = { dx, -0.08f, dz };
    GLfloat spotDiffuse[] = { 1.00f, 1.00f, 0.95f, 1.0f };
    GLfloat spotSpecular[] = { 0.95f, 0.95f, 0.92f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, spotPos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, spotDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spotSpecular);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 24.0f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 12.0f);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.02f);

    glEnable(GL_LIGHT0);

    // Obor kanan-kiri untuk memberi depth ambience horor.
    GLfloat torch1Pos[] = { worldXFromCol(1), 1.1f, worldZFromRow(1), 1.0f };
    GLfloat torch1Diffuse[] = { 0.92f * flicker, 0.44f * flicker, 0.24f * flicker, 1.0f };
    GLfloat torch1Spec[] = { 0.54f * flicker, 0.28f * flicker, 0.14f * flicker, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, torch1Pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, torch1Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, torch1Spec);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.22f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.12f);
    glEnable(GL_LIGHT1);

    GLfloat torch2Pos[] = { worldXFromCol(8), 1.1f, worldZFromRow(8), 1.0f };
    GLfloat torch2Diffuse[] = { 0.76f * flicker, 0.28f * flicker, 0.12f * flicker, 1.0f };
    GLfloat torch2Spec[] = { 0.45f * flicker, 0.18f * flicker, 0.09f * flicker, 1.0f };
    glLightfv(GL_LIGHT2, GL_POSITION, torch2Pos);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, torch2Diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, torch2Spec);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.24f);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.15f);
    glEnable(GL_LIGHT2);

    // Fog tipis untuk kesan ruang sempit dan menekan jarak pandang.
    GLfloat fogColor[] = { 0.05f, 0.05f, 0.08f, 1.0f };
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, 6.5f);
    glFogf(GL_FOG_END, 26.0f);
}

// TODO: Gambar labirin menggunakan primitif (GL_QUADS) dan pasang glTexCoord2f 
void drawMaze() {
    if (!gTextureLoaded) {
        loadTextures();
    }

    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);

    // Lantai
    glBindTexture(GL_TEXTURE_2D, gFloorTex);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-X_OFFSET, 0.0f, -Z_OFFSET);
    glTexCoord2f(8.0f, 0.0f); glVertex3f( X_OFFSET, 0.0f, -Z_OFFSET);
    glTexCoord2f(8.0f, 8.0f); glVertex3f( X_OFFSET, 0.0f,  Z_OFFSET);
    glTexCoord2f(0.0f, 8.0f); glVertex3f(-X_OFFSET, 0.0f,  Z_OFFSET);
    glEnd();

    // Plafon sangat gelap agar area atas "menekan" secara visual.
    glBindTexture(GL_TEXTURE_2D, gWallTex);
    glColor3f(0.28f, 0.28f, 0.32f);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-X_OFFSET, WALL_HEIGHT,  Z_OFFSET);
    glTexCoord2f(8.0f, 0.0f); glVertex3f( X_OFFSET, WALL_HEIGHT,  Z_OFFSET);
    glTexCoord2f(8.0f, 8.0f); glVertex3f( X_OFFSET, WALL_HEIGHT, -Z_OFFSET);
    glTexCoord2f(0.0f, 8.0f); glVertex3f(-X_OFFSET, WALL_HEIGHT, -Z_OFFSET);
    glEnd();

    // Dinding labirin.
    glBindTexture(GL_TEXTURE_2D, gWallTex);
    glColor3f(0.95f, 0.95f, 0.95f);
    for (int r = 0; r < MAZE_ROWS; ++r) {
        for (int c = 0; c < MAZE_COLS; ++c) {
            if (maze[r][c] != 1) continue;

            const float x = worldXFromCol(c);
            const float z = worldZFromRow(r);
            drawWallBlock(x, z, WALL_HALF_WIDTH, WALL_HEIGHT);
        }
    }

    glDisable(GL_TEXTURE_2D);

    drawShadows();

    glPopMatrix();
}

// Bayangan sederhana: proyeksi blob gelap di lantai dari setiap dinding.
void drawShadows() {
    const float lx = playerCam.x;
    const float lz = playerCam.z;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.18f);

    for (int r = 0; r < MAZE_ROWS; ++r) {
        for (int c = 0; c < MAZE_COLS; ++c) {
            if (maze[r][c] != 1) continue;

            const float x = worldXFromCol(c);
            const float z = worldZFromRow(r);
            const float vx = x - lx;
            const float vz = z - lz;
            const float dist = sqrt(vx * vx + vz * vz) + 0.0001f;

            const float dirx = vx / dist;
            const float dirz = vz / dist;
            const float shLen = 0.45f + (dist < 2.0f ? 0.9f : 1.4f);
            const float shWidth = 0.30f;

            const float px = -dirz * shWidth;
            const float pz =  dirx * shWidth;
            const float sx = x + dirx * shLen;
            const float sz = z + dirz * shLen;

            glBegin(GL_QUADS);
            glVertex3f(x - px, 0.01f, z - pz);
            glVertex3f(x + px, 0.01f, z + pz);
            glVertex3f(sx + px, 0.01f, sz + pz);
            glVertex3f(sx - px, 0.01f, sz - pz);
            glEnd();
        }
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}