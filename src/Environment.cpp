/*
    Environment.cpp - Abyssal Maze (Yu Ming Jie)
    - Dinding bata + lantai ubin bertekstur
    - GL_LIGHT0: spotlight senter (cone dari kamera)
    - GL_LIGHT1-6: obor merah flicker di dinding
    - Fog tipis + volumetric cone visual
    - Minimap HUD kanan atas
*/

#include <GL/glut.h>
#include <cmath>
#include "shared/Config.h"
#include "shared/Environment.h"

/* ===================== GLOBALS ===================== */
GLuint g_texWall  = 0;
GLuint g_texFloor = 0;
float  g_time     = 0.0f;

/* Posisi kamera & arah pandang (diupdate dari main.cpp) */
float camX  = 3.0f, camY = 1.6f, camZ = 3.0f;
float lookX = 5.0f, lookY = 1.6f, lookZ = 3.0f;

/* Data posisi obor (max 6) */
static float tX[6], tY[6], tZ[6];
static int   tCount = 0;

/* ===================== HASH NOISE ===================== */
/* Fungsi noise sederhana untuk variasi tekstur */
float hashf(unsigned int s) {
    s ^= s << 13; s ^= s >> 17; s ^= s << 5;
    return (float)(s & 0xFFFF) / 65536.0f;
}

/* ===================== TEKSTUR ===================== */
void genTextures() {
    const int W = 64, H = 64;
    unsigned char wData[W*H*4], fData[W*H*4];

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int i = (y*W+x)*4;

            /* --- TEKSTUR DINDING: bata Han Dynasty --- */
            int brow = y/10;
            int bx   = (brow%2==0) ? (x%18) : ((x+9)%18);
            int by   = y%10;
            int mort = (bx<2)||(by<2); /* 1 = area mortar (sela bata) */

            /* Warna dasar bata */
            unsigned char bW = mort ? 55
                : (unsigned char)(100 + hashf((unsigned)(x*31+y*97))*50.0f);
            wData[i+0] = (bW+40 > 255) ? 255 : bW+40; /* R */
            wData[i+1] = bW;                            /* G */
            wData[i+2] = (bW>10) ? bW-10 : 0;          /* B */
            wData[i+3] = 255;

            /* Noda darah merah acak */
            if (!mort && hashf((unsigned)(x*211+y*97+0xDEAD)) > 0.93f)
                { wData[i+0]=160; wData[i+1]=20; wData[i+2]=20; }

            /* Jimat hijau acak */
            if (!mort && hashf((unsigned)(x*503+y*251+0xFADE)) > 0.97f) {
                wData[i+0] = (unsigned char)(wData[i+0]*0.5f+30);
                wData[i+1] = (unsigned char)(wData[i+1]*0.5f+80);
                wData[i+2] = (unsigned char)(wData[i+2]*0.5f+40);
            }

            /* --- TEKSTUR LANTAI: ubin gelap --- */
            int grt = (x%32==0)||(y%32==0); /* 1 = garis grid */
            unsigned char bF = grt ? 35
                : (unsigned char)(65 + hashf((unsigned)(x*151+y*199))*40.0f);
            fData[i+0] = bF;
            fData[i+1] = bF;
            fData[i+2] = (bF+20>255) ? 255 : bF+20;
            fData[i+3] = 255;

            /* Noda darah di lantai */
            if (!grt && hashf((unsigned)(x*337+y*421+0xB10D)) > 0.96f)
                { fData[i+0]=120; fData[i+1]=15; fData[i+2]=15; }
            /* Bintik emas di lantai */
            if (!grt && hashf((unsigned)(x*613+y*719+0xF1A9)) > 0.985f)
                { fData[i+0]=220; fData[i+1]=185; fData[i+2]=30; }
        }
    }

    /* Upload tekstur dinding ke GPU */
    glGenTextures(1, &g_texWall);
    glBindTexture(GL_TEXTURE_2D, g_texWall);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, wData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Upload tekstur lantai ke GPU */
    glGenTextures(1, &g_texFloor);
    glBindTexture(GL_TEXTURE_2D, g_texFloor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, fData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/* ===================== SPOTLIGHT SENTER ===================== */
/* GL_LIGHT0: cahaya cone dari posisi kamera ke arah pandang */
void setupFlashlight() {
    glEnable(GL_LIGHT0);

    GLfloat pos[4] = { camX, camY, camZ, 1.0f };
    GLfloat dir[3] = { lookX-camX, lookY-camY, lookZ-camZ };

    /* Extremely dark ambient for horror atmosphere */
    GLfloat amb[]  = { AMBIENT_BRIGHTNESS, AMBIENT_BRIGHTNESS, AMBIENT_BRIGHTNESS, 1.0f };
    /* Bright diffuse for clear cone */
    GLfloat diff[] = { 1.0f, 1.0f, 0.95f, 1.0f };
    GLfloat spec[] = { 1.0f, 1.0f, 0.9f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION,       pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
    glLightfv(GL_LIGHT0, GL_AMBIENT,        amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,        diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR,       spec);

    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,          FLASHLIGHT_CUTOFF); /* narrow cone for horror */
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT,        32.0f); /* sharp edge */
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05f);
}

/* ===================== API OBOR ===================== */
/* Gambar nyala api dengan 3 cone bertumpuk (flicker dari flickVal) */
void drawTorchFlame(float flickVal) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    /* Tangkai coklat */
    glColor3f(0.45f, 0.30f, 0.18f);
    glPushMatrix();
        glScalef(0.04f, 0.18f, 0.04f);
        glutSolidCube(1.0f);
    glPopMatrix();

    /* Nyala api dengan blending additive */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPushMatrix();
    glTranslatef(0.0f, 0.18f, 0.0f);
    glScalef(1.0f, flickVal, 1.0f); /* skala Y berubah = flicker */

        /* Cone luar: oranye */
        glColor4f(1.0f, 0.35f, 0.0f, 0.55f);
        glPushMatrix();
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.07f, 0.22f, 8, 4);
        glPopMatrix();

        /* Cone tengah: kuning */
        glColor4f(1.0f, 0.80f, 0.1f, 0.70f);
        glPushMatrix();
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.04f, 0.18f, 8, 4);
        glPopMatrix();

        /* Cone dalam: putih panas */
        glColor4f(1.0f, 1.0f, 0.85f, 0.85f);
        glPushMatrix();
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.015f, 0.10f, 6, 2);
        glPopMatrix();

    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

/* ===================== OBOR + POINT LIGHT ===================== */
/* Gambar obor di posisi (x,y,z) dan set GL_LIGHT1+id */
void drawTorch(float x, float y, float z, int id) {
    /* Hitung nilai flicker dari kombinasi sine berbeda frekuensi */
    float f = 0.5f * (sinf(g_time*10.0f + id)    * 0.5f + 0.5f)
            + 0.3f * (sinf(g_time*25.0f + id*2)   * 0.5f + 0.5f)
            + 0.2f * (sinf(g_time*50.0f)           * 0.5f + 0.5f);
    f = 0.4f + 0.6f * f;

    GLenum lid  = (GLenum)(GL_LIGHT1 + id);
    float  dx   = camX - x;
    float  dz   = camZ - z;
    float  dist = sqrtf(dx*dx + dz*dz);

    /* Hanya aktifkan light jika pemain cukup dekat */
    if (dist < 6.0f)
        glEnable(lid);
    else {
        glDisable(lid);
        return;
    }

    /* Cek apakah ada dinding menghalangi antara obor dan pemain */
    const float S = MAZE_CELL_SIZE;
    float occlude = 1.0f;
    int steps = (int)(dist / 0.4f) + 1;
    for (int s = 1; s <= steps; s++) {
        float t  = (float)s / (float)steps;
        float rx = x + dx*t;
        float rz = z + dz*t;
        int   ci = (int)(rx/S);
        int   cj = (int)(rz/S);
        if (cj>=0 && cj<MAZE_HEIGHT && ci>=0 && ci<MAZE_WIDTH) {
            if (mazeMatrix[cj][ci] == 1) {
                occlude = 0.04f; /* dinding menghalangi, redupkan */
                break;
            }
        }
    }

    /* Set properti point light merah flicker */
    GLfloat lPos[]  = { x, y+0.22f, z, 1.0f };
    GLfloat lDiff[] = { 0.8f*f*occlude, 0.3f*f*occlude, 0.05f*f*occlude, 1.0f };
    GLfloat lAmb[]  = { 0.2f*f*occlude, 0.08f*f*occlude, 0.0f, 1.0f };
    GLfloat lSpec[] = { 0.5f*f*occlude, 0.15f*f*occlude, 0.0f, 1.0f };

    glLightfv(lid, GL_POSITION, lPos);
    glLightfv(lid, GL_DIFFUSE,  lDiff);
    glLightfv(lid, GL_AMBIENT,  lAmb);
    glLightfv(lid, GL_SPECULAR, lSpec);
    glLightf(lid, GL_SPOT_CUTOFF,          180.0f);
    glLightf(lid, GL_CONSTANT_ATTENUATION,   1.0f);
    glLightf(lid, GL_LINEAR_ATTENUATION,     TORCH_LINEAR_ATT);
    glLightf(lid, GL_QUADRATIC_ATTENUATION,  TORCH_QUAD_ATT);

    /* Gambar model obor */
    glPushMatrix();
        glTranslatef(x, y, z);
        drawTorchFlame(0.8f + 0.4f*f);
    glPopMatrix();
}

/* ===================== DINDING ===================== */
/* Gambar kubus dinding bertekstur di posisi (wx, wz) */
void drawWallCube(float wx, float wz, float S, float WH) {
    float x0=wx, x1=wx+S;
    float z0=wz, z1=wz+S;
    float y0=0.0f, y1=WH;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_texWall);

    /* Material putih = warna tekstur tidak terdistorsi */
    GLfloat mat[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  mat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat);
    glColor4f(1,1,1,1);

    glBegin(GL_QUADS);
        /* Depan (Z+) */
        glNormal3f(0,0,1);
        glTexCoord2f(0,0); glVertex3f(x0,y0,z1);
        glTexCoord2f(1,0); glVertex3f(x1,y0,z1);
        glTexCoord2f(1,1); glVertex3f(x1,y1,z1);
        glTexCoord2f(0,1); glVertex3f(x0,y1,z1);

        /* Belakang (Z-) */
        glNormal3f(0,0,-1);
        glTexCoord2f(0,0); glVertex3f(x1,y0,z0);
        glTexCoord2f(1,0); glVertex3f(x0,y0,z0);
        glTexCoord2f(1,1); glVertex3f(x0,y1,z0);
        glTexCoord2f(0,1); glVertex3f(x1,y1,z0);

        /* Kiri (X-) */
        glNormal3f(-1,0,0);
        glTexCoord2f(0,0); glVertex3f(x0,y0,z0);
        glTexCoord2f(1,0); glVertex3f(x0,y0,z1);
        glTexCoord2f(1,1); glVertex3f(x0,y1,z1);
        glTexCoord2f(0,1); glVertex3f(x0,y1,z0);

        /* Kanan (X+) */
        glNormal3f(1,0,0);
        glTexCoord2f(0,0); glVertex3f(x1,y0,z1);
        glTexCoord2f(1,0); glVertex3f(x1,y0,z0);
        glTexCoord2f(1,1); glVertex3f(x1,y1,z0);
        glTexCoord2f(0,1); glVertex3f(x1,y1,z1);

        /* Atas (Y+) */
        glNormal3f(0,1,0);
        glTexCoord2f(0,0); glVertex3f(x0,y1,z0);
        glTexCoord2f(1,0); glVertex3f(x1,y1,z0);
        glTexCoord2f(1,1); glVertex3f(x1,y1,z1);
        glTexCoord2f(0,1); glVertex3f(x0,y1,z1);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

/* ===================== LANTAI ===================== */
/* Gambar satu tile lantai bertekstur */
void drawTexturedFloor(float x, float z, float s) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_texFloor);

    GLfloat mat[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  mat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat);
    glColor4f(1,1,1,1);

    glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glTexCoord2f(0,0); glVertex3f(x,   0, z  );
        glTexCoord2f(1,0); glVertex3f(x+s, 0, z  );
        glTexCoord2f(1,1); glVertex3f(x+s, 0, z+s);
        glTexCoord2f(0,1); glVertex3f(x,   0, z+s);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

/* ===================== BAYANGAN ===================== */
/* Gambar quad hitam transparan di lantai bawah obor */
void drawShadows() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE); /* jangan tulis depth, hanya warna */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int k = 0; k < tCount; k++) {
        float sr    = 1.0f; /* radius bayangan */
        float sx    = tX[k], sz = tZ[k];
        float sy    = 0.02f; /* sedikit di atas lantai agar tidak z-fight */
        float f     = 0.45f + 0.55f*(0.5f + 0.5f*sinf(g_time*3.8f + (float)k*1.3f));
        float alpha = 0.5f * f;

        glColor4f(0,0,0, alpha);
        glBegin(GL_QUADS);
            glVertex3f(sx-sr, sy, sz-sr);
            glVertex3f(sx+sr, sy, sz-sr);
            glVertex3f(sx+sr, sy, sz+sr);
            glVertex3f(sx-sr, sy, sz+sr);
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

/* ===================== FOG ===================== */
/* Kabut tipis ungu gelap untuk suasana misterius */
void setupFog() {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    GLfloat fc[] = { 0.02f, 0.0f, 0.02f, 1.0f };
    glFogfv(GL_FOG_COLOR, fc);
    /* Density rendah supaya dinding tetap terlihat */
    glFogf(GL_FOG_DENSITY, 0.025f + 0.002f*sinf(g_time*0.35f));
    glHint(GL_FOG_HINT, GL_NICEST);
}

/* ===================== INIT ===================== */
void initEnvironment() {
    genTextures();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

/* Ambient global sangat gelap supaya area di luar spotlight benar gelap */
    GLfloat gAmb[] = { AMBIENT_BRIGHTNESS, AMBIENT_BRIGHTNESS, AMBIENT_BRIGHTNESS, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gAmb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,     GL_TRUE);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    /* Background hitam gelap */
    glClearColor(0.01f, 0.0f, 0.02f, 1.0f);
}

/* ===================== VOLUMETRIC SPOTLIGHT ===================== */
/* Gambar segitiga cahaya transparan dari kamera ke depan (efek visual cone) */
void drawVolumetricSpotlight() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    /* Hitung arah pandang ternormalisasi */
    float dx = lookX - camX;
    float dy = lookY - camY;
    float dz = lookZ - camZ;
    float dlen = sqrtf(dx*dx + dy*dy + dz*dz);
    if (dlen < 0.001f) dlen = 0.001f;
    dx /= dlen; dy /= dlen; dz /= dlen;

    /* Vektor tegak lurus di bidang XZ untuk membentuk lingkaran ujung cone */
    float rx = -dz;
    float rz =  dx;

    glPushMatrix();
    glTranslatef(camX, camY, camZ);

    glColor4f(0.9f, 0.85f, 0.7f, 0.035f); /* sangat transparan */

    float radius   = 2.0f;  /* radius ujung cone */
    float distance = 10.0f; /* panjang cone */

    /* Triangle fan: titik pusat (kamera) ke lingkaran ujung cone */
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, 0.0f, 0.0f);
        for (int i = 0; i <= 24; i++) {
            float angle = i * 2.0f * 3.14159f / 24.0f;
            float cs = cosf(angle), sn = sinf(angle);
            /* Kombinasi vektor tegak lurus untuk membuat lingkaran */
            float ox = (rx*cs) * radius;
            float oy = sn * radius;
            float oz = (rz*cs) * radius;
            glVertex3f(dx*distance + ox,
                       dy*distance + oy,
                       dz*distance + oz);
        }
    glEnd();

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

/* ===================== RENDER DUNIA ===================== */
void renderEnvironment() {
    const float S  = TILE_SIZE;        /* ukuran tiap cell maze dalam world unit */
    const float WH = WALL_HEIGHT;      /* tinggi dinding */

    /* Pass 1: Gambar lantai semua cell */
    for (int i = 0; i < MAZE_HEIGHT; i++)
        for (int j = 0; j < MAZE_WIDTH; j++)
            drawTexturedFloor((float)j*S, (float)i*S, S);

    /* Pass 2: Gambar dinding (cell=1) + letakkan obor */
    tCount = 0;
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            if (mazeMatrix[i][j] == 1) {
                float wx = (float)j*S;
                float wz = (float)i*S;
                drawWallCube(wx, wz, S, WH);

                if (tCount < 6) {
                    bool corridorNear = false;
                    if (i > 0 && mazeMatrix[i - 1][j] == 0) corridorNear = true;
                    if (i + 1 < MAZE_HEIGHT && mazeMatrix[i + 1][j] == 0) corridorNear = true;
                    if (j > 0 && mazeMatrix[i][j - 1] == 0) corridorNear = true;
                    if (j + 1 < MAZE_WIDTH && mazeMatrix[i][j + 1] == 0) corridorNear = true;

                    if (corridorNear && ((i * 3 + j * 5) % 4 == 0)) {
                        tX[tCount] = wx + 0.1f;
                        tY[tCount] = WH * 0.6f;
                        tZ[tCount] = wz + S * 0.5f;
                        drawTorch(tX[tCount], tY[tCount], tZ[tCount], tCount);
                        tCount++;
                    }
                }
            }
        }
    }

    /* Pass 3: Efek bayangan dan cone cahaya */
    drawShadows();
    drawVolumetricSpotlight();
}

/* ===================== MINIMAP ===================== */
/* HUD minimap kanan atas dengan fog-of-war berdasarkan arah spotlight */
void drawMinimap() {
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    if (winH == 0) winH = 1;

    const int   CELL   = 5;   /* ukuran tiap cell di minimap (pixel) */
    const int   MARGIN = 10;
    const int   MMAP_W = MAZE_WIDTH  * CELL;
    const int   MMAP_H = MAZE_HEIGHT * CELL;
    const int   ORIG_X = winW - MMAP_W - MARGIN;
    const int   ORIG_Y = winH - MMAP_H - MARGIN;
    const float S      = TILE_SIZE;

    /* Switch ke 2D orthographic untuk HUD */
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    /* Background panel gelap */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0, 0.60f);
    glBegin(GL_QUADS);
        glVertex2i(ORIG_X-3,          ORIG_Y-3);
        glVertex2i(ORIG_X+MMAP_W+3,   ORIG_Y-3);
        glVertex2i(ORIG_X+MMAP_W+3,   ORIG_Y+MMAP_H+3);
        glVertex2i(ORIG_X-3,          ORIG_Y+MMAP_H+3);
    glEnd();

    /* Posisi pemain dalam satuan cell */
    float playerCellX = camX / S;
    float playerCellZ = camZ / S;

    /* Arah spotlight ternormalisasi */
    float sdx  = lookX - camX;
    float sdz  = lookZ - camZ;
    float slen = sqrtf(sdx*sdx + sdz*sdz);
    if (slen < 0.001f) slen = 0.001f;
    sdx /= slen; sdz /= slen;

    const float SPOTLIGHT_RANGE = 14.0f;
    const float SPOTLIGHT_COS   = cosf(40.0f * 3.14159265f / 180.0f);

    /* Gambar tiap cell minimap */
    for (int row = 0; row < MAZE_HEIGHT; row++) {
        for (int col = 0; col < MAZE_WIDTH; col++) {
            /* Cek apakah cell ini dalam cone spotlight */
            float tcx  = (float)col + 0.5f - playerCellX;
            float tcz  = (float)row + 0.5f - playerCellZ;
            float tlen = sqrtf(tcx*tcx + tcz*tcz);

            bool visible = false;
            if (tlen < SPOTLIGHT_RANGE) {
                if (tlen < 1.2f) {
                    visible = true; /* cell di sekitar pemain selalu visible */
                } else {
                    float nx  = tcx/tlen, nz = tcz/tlen;
                    float dot = nx*sdx + nz*sdz;
                    if (dot >= SPOTLIGHT_COS) visible = true;
                }
            }

            int px = ORIG_X + col * CELL;
            int py = ORIG_Y + (MAZE_HEIGHT-1-row) * CELL;

            /* Warna cell: gelap=tidak terlihat, coklat=dinding, abu=lorong */
            if      (!visible)              glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
            else if (mazeMatrix[row][col])  glColor4f(0.45f,0.22f,0.12f,0.95f);
            else                            glColor4f(0.60f,0.58f,0.54f,0.90f);

            glBegin(GL_QUADS);
                glVertex2i(px,        py);
                glVertex2i(px+CELL,   py);
                glVertex2i(px+CELL,   py+CELL);
                glVertex2i(px,        py+CELL);
            glEnd();
        }
    }

    /* Titik kuning = posisi pemain */
    int pdx = ORIG_X + (int)(playerCellX * CELL);
    int pdz = ORIG_Y + (MAZE_HEIGHT-1-(int)playerCellZ) * CELL;
    glColor4f(1.0f, 0.9f, 0.1f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2i(pdx-2, pdz-2);
        glVertex2i(pdx+2, pdz-2);
        glVertex2i(pdx+2, pdz+2);
        glVertex2i(pdx-2, pdz+2);
    glEnd();

    glDisable(GL_BLEND);

    /* Kembalikan ke mode 3D */
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}
