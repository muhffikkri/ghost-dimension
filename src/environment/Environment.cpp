/*
    Environment.cpp 
    - Dinding bata + lantai ubin bertekstur
    - GL_LIGHT0: spotlight senter (cone dari kamera)
    - GL_LIGHT1-6: obor merah flicker di dinding
    - Fog tipis + volumetric cone visual
    - Minimap HUD kanan atas
*/

#include <GL/glut.h>
#include <cmath>
#include "../../include/Config.h"
#include "../../include/Environment.h"
#include "../../include/Entity.h"  

GLuint g_texWall  = 0;
GLuint g_texFloor = 0;
float  g_time     = 0.0f;

float camX=3.0f, camY=1.6f, camZ=3.0f;
float lookX=5.0f, lookY=1.6f, lookZ=3.0f;

static float tX[6], tY[6], tZ[6];
static int   tCount = 0;

float hashf(unsigned int s) {
    s ^= s<<13; s ^= s>>17; s ^= s<<5;
    return (float)(s & 0xFFFF) / 65536.0f;
}

void genTextures() {
    const int W=64, H=64;
    unsigned char wData[W*H*4], fData[W*H*4];
    for (int y=0;y<H;y++) {
        for (int x=0;x<W;x++) {
            int i=(y*W+x)*4;
            int brow=y/10;
            int bx=(brow%2==0)?(x%18):((x+9)%18);
            int by=y%10;
            int mort=(bx<2)||(by<2);
            unsigned char bW=mort?55:(unsigned char)(100+hashf((unsigned)(x*31+y*97))*50.0f);
            wData[i+0]=(bW+40>255)?255:bW+40;
            wData[i+1]=bW;
            wData[i+2]=(bW>10)?bW-10:0;
            wData[i+3]=255;
            if(!mort && hashf((unsigned)(x*211+y*97+0xDEAD))>0.93f)
                {wData[i+0]=160;wData[i+1]=20;wData[i+2]=20;}
            if(!mort && hashf((unsigned)(x*503+y*251+0xFADE))>0.97f){
                wData[i+0]=(unsigned char)(wData[i+0]*0.5f+30);
                wData[i+1]=(unsigned char)(wData[i+1]*0.5f+80);
                wData[i+2]=(unsigned char)(wData[i+2]*0.5f+40);
            }
            int grt=(x%32==0)||(y%32==0);
            unsigned char bF=grt?35:(unsigned char)(65+hashf((unsigned)(x*151+y*199))*40.0f);
            fData[i+0]=bF; fData[i+1]=bF;
            fData[i+2]=(bF+20>255)?255:bF+20;
            fData[i+3]=255;
            if(!grt && hashf((unsigned)(x*337+y*421+0xB10D))>0.96f)
                {fData[i+0]=120;fData[i+1]=15;fData[i+2]=15;}
            if(!grt && hashf((unsigned)(x*613+y*719+0xF1A9))>0.985f)
                {fData[i+0]=220;fData[i+1]=185;fData[i+2]=30;}
        }
    }
    glGenTextures(1,&g_texWall);
    glBindTexture(GL_TEXTURE_2D,g_texWall);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,W,H,0,GL_RGBA,GL_UNSIGNED_BYTE,wData);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glGenTextures(1,&g_texFloor);
    glBindTexture(GL_TEXTURE_2D,g_texFloor);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,W,H,0,GL_RGBA,GL_UNSIGNED_BYTE,fData);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

// ===================== BUG FIX 1: SPOTLIGHT TIDAK MENEMBUS DINDING =====================
// setupFlashlight() WAJIB dipanggil SEBELUM gluLookAt (di eye-space).
// Dengan pasang posisi di (0,0,0) eye-space, lampu selalu di mata kamera.
// Depth buffer sudah memblok cahaya dari menembus dinding secara visual.
// Tambahan: spot_cutoff dikecilkan agar berbentuk kerucut jelas.
void setupFlashlight() {
    glEnable(GL_LIGHT0);

    // FIXED: Set di eye-space ? posisi ikut kamera otomatis, tidak menembus dinding
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity(); // identitas = eye-space

    GLfloat pos[4] = { 0.0f, 0.0f,  0.0f, 1.0f }; // tepat di mata
    GLfloat dir[3] = { 0.0f, 0.0f, -1.0f };         // lurus ke depan

    glLightfv(GL_LIGHT0, GL_POSITION,       pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
    glPopMatrix();

    GLfloat amb[]  = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat diff[] = { 1.8f,  1.6f,  1.3f,  1.0f }; // lebih terang
    GLfloat spec[] = { 0.8f,  0.8f,  0.8f,  1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    // FIXED: Cutoff 45� membentuk kerucut nyata, exponent 5 = gradasi lembut
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,          45.0f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT,         5.0f);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.02f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.005f);
}

// Fungsi obor tidak berubah signifikan, tetap ada occlusion check
void drawTorchFlame(float flickVal) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glColor3f(0.45f,0.30f,0.18f);
    glPushMatrix(); glScalef(0.04f,0.18f,0.04f); glutSolidCube(1.0f); glPopMatrix();
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPushMatrix(); glTranslatef(0,0.18f,0); glScalef(1,flickVal,1);
        glColor4f(1.0f,0.35f,0.0f,0.55f);
        glPushMatrix(); glRotatef(-90,1,0,0); glutSolidCone(0.07f,0.22f,8,4); glPopMatrix();
        glColor4f(1.0f,0.80f,0.1f,0.70f);
        glPushMatrix(); glRotatef(-90,1,0,0); glutSolidCone(0.04f,0.18f,8,4); glPopMatrix();
        glColor4f(1.0f,1.0f,0.85f,0.85f);
        glPushMatrix(); glRotatef(-90,1,0,0); glutSolidCone(0.015f,0.10f,6,2); glPopMatrix();
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void drawTorch(float x, float y, float z, int id) {
    float f = 0.5f*(sinf(g_time*10.0f+id)*0.5f+0.5f)
            + 0.3f*(sinf(g_time*25.0f+id*2)*0.5f+0.5f)
            + 0.2f*(sinf(g_time*50.0f)*0.5f+0.5f);
    f = 0.4f + 0.6f*f;

    GLenum lid = (GLenum)(GL_LIGHT1+id);
    float dx=camX-x, dz=camZ-z;
    float dist=sqrtf(dx*dx+dz*dz);

    // Occlusion check tetap ada
    const float S=2.0f;
    float occlude=1.0f;
    int steps=(int)(dist/0.4f)+1;
    for (int s=1;s<=steps;s++) {
        float t=(float)s/(float)steps;
        float rx=x+dx*t, rz=z+dz*t;
        int ci=(int)(rx/S), cj=(int)(rz/S);
        if (cj>=0&&cj<MAZE_HEIGHT&&ci>=0&&ci<MAZE_WIDTH&&mazeMatrix[cj][ci]==1)
            { occlude=0.04f; break; }
    }

    if (dist > 3.4f || occlude < 0.25f) {
        glDisable(lid);
        return;
    }

    glEnable(lid);

    GLfloat lPos[]  = {x,y+0.22f,z,1.0f};
    GLfloat lDiff[] = {0.8f*f*occlude,0.3f*f*occlude,0.05f*f*occlude,1.0f};
    GLfloat lAmb[]  = {0.2f*f*occlude,0.08f*f*occlude,0,1};
    GLfloat lSpec[] = {0.5f*f*occlude,0.15f*f*occlude,0,1};
    glLightfv(lid,GL_POSITION,lPos);
    glLightfv(lid,GL_DIFFUSE, lDiff);
    glLightfv(lid,GL_AMBIENT, lAmb);
    glLightfv(lid,GL_SPECULAR,lSpec);
    glLightf(lid,GL_SPOT_CUTOFF,         180.0f);
    glLightf(lid,GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(lid,GL_LINEAR_ATTENUATION,    0.5f);
    glLightf(lid,GL_QUADRATIC_ATTENUATION, 0.15f);

    glPushMatrix(); glTranslatef(x,y,z); drawTorchFlame(0.8f+0.4f*f); glPopMatrix();
}

void drawWallCube(float wx, float wz, float S, float WH) {
    float x0=wx,x1=wx+S, z0=wz,z1=wz+S, y0=0,y1=WH;
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,g_texWall);
    GLfloat mat[]={1,1,1,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat);
    glColor4f(1,1,1,1);
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);  glTexCoord2f(0,0);glVertex3f(x0,y0,z1); glTexCoord2f(1,0);glVertex3f(x1,y0,z1); glTexCoord2f(1,1);glVertex3f(x1,y1,z1); glTexCoord2f(0,1);glVertex3f(x0,y1,z1);
        glNormal3f(0,0,-1); glTexCoord2f(0,0);glVertex3f(x1,y0,z0); glTexCoord2f(1,0);glVertex3f(x0,y0,z0); glTexCoord2f(1,1);glVertex3f(x0,y1,z0); glTexCoord2f(0,1);glVertex3f(x1,y1,z0);
        glNormal3f(-1,0,0); glTexCoord2f(0,0);glVertex3f(x0,y0,z0); glTexCoord2f(1,0);glVertex3f(x0,y0,z1); glTexCoord2f(1,1);glVertex3f(x0,y1,z1); glTexCoord2f(0,1);glVertex3f(x0,y1,z0);
        glNormal3f(1,0,0);  glTexCoord2f(0,0);glVertex3f(x1,y0,z1); glTexCoord2f(1,0);glVertex3f(x1,y0,z0); glTexCoord2f(1,1);glVertex3f(x1,y1,z0); glTexCoord2f(0,1);glVertex3f(x1,y1,z1);
        glNormal3f(0,1,0);  glTexCoord2f(0,0);glVertex3f(x0,y1,z0); glTexCoord2f(1,0);glVertex3f(x1,y1,z0); glTexCoord2f(1,1);glVertex3f(x1,y1,z1); glTexCoord2f(0,1);glVertex3f(x0,y1,z1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawTexturedFloor(float x, float z, float s) {
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,g_texFloor);
    GLfloat mat[]={1,1,1,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat);
    glColor4f(1,1,1,1);
    glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glTexCoord2f(0,0);glVertex3f(x,  0,z  );
        glTexCoord2f(1,0);glVertex3f(x+s,0,z  );
        glTexCoord2f(1,1);glVertex3f(x+s,0,z+s);
        glTexCoord2f(0,1);glVertex3f(x,  0,z+s);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// ===================== FIXED: drawExitPoint() =====================
// Portal keluar bercahaya cyan berputar, muncul di ujung maze.
// Jika belum ada key, portal ditampilkan memudar/abu-abu.
void drawExitPoint(float ex, float ez) {
    extern float g_time;
    extern bool  hasKey;

    float spin = g_time * 80.0f;
    float pulse = 0.5f + 0.5f * sinf(g_time * 3.0f);

    glPushMatrix();
    glTranslatef(ex, 0.01f, ez);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Warna: cyan jika punya kunci, abu jika belum
    if (hasKey)
        glColor4f(0.0f, 1.0f, 0.9f, 0.35f + 0.15f * pulse);
    else
        glColor4f(0.3f, 0.3f, 0.3f, 0.15f);

    // Lingkaran halo di lantai
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0.02f, 0);
        for (int i = 0; i <= 32; i++) {
            float a = i * 2.0f * 3.14159f / 32;
            glVertex3f(cosf(a)*0.8f, 0.02f, sinf(a)*0.8f);
        }
    glEnd();

    // Ring berputar
    glColor4f(0.1f, 1.0f, 1.0f, 0.7f + 0.3f * pulse);
    glRotatef(spin, 0, 1, 0);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 32; i++) {
            float a = i * 2.0f * 3.14159f / 32;
            glVertex3f(cosf(a)*0.7f, 0.1f, sinf(a)*0.7f);
        }
    glEnd();
    glRotatef(spin * 1.5f, 0, 1, 0);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 24; i++) {
            float a = i * 2.0f * 3.14159f / 24;
            glVertex3f(cosf(a)*0.5f, 0.3f, sinf(a)*0.5f);
        }
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    // Light dari portal (hanya jika punya key)
    if (hasKey) {
        glEnable(GL_LIGHT7);
        GLfloat lp[] = {ex, 1.0f, ez, 1.0f};
        GLfloat ld[] = {0.0f, 1.0f, 0.9f, 1.0f};
        GLfloat la[] = {0.0f, 0.15f, 0.12f, 1.0f};
        glLightfv(GL_LIGHT7, GL_POSITION, lp);
        glLightfv(GL_LIGHT7, GL_DIFFUSE,  ld);
        glLightfv(GL_LIGHT7, GL_AMBIENT,  la);
        glLightf(GL_LIGHT7, GL_SPOT_CUTOFF,         180.0f);
        glLightf(GL_LIGHT7, GL_CONSTANT_ATTENUATION,  1.5f);
        glLightf(GL_LIGHT7, GL_LINEAR_ATTENUATION,    0.4f);
        glLightf(GL_LIGHT7, GL_QUADRATIC_ATTENUATION, 0.1f);
    } else {
        glDisable(GL_LIGHT7);
    }

    glPopMatrix();
}

void drawShadows() {
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (int k=0;k<tCount;k++) {
        float sr=1.0f;
        float f=0.45f+0.55f*(0.5f+0.5f*sinf(g_time*3.8f+(float)k*1.3f));
        glColor4f(0,0,0,0.5f*f);
        glBegin(GL_QUADS);
            glVertex3f(tX[k]-sr,0.02f,tZ[k]-sr);
            glVertex3f(tX[k]+sr,0.02f,tZ[k]-sr);
            glVertex3f(tX[k]+sr,0.02f,tZ[k]+sr);
            glVertex3f(tX[k]-sr,0.02f,tZ[k]+sr);
        glEnd();
    }
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND); glEnable(GL_LIGHTING);
}

void setupFog() {
    GLfloat fc[]={0.1f,0.1f,0.1f,1};
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE,GL_LINEAR);
    glFogfv(GL_FOG_COLOR,fc);
    glFogf(GL_FOG_START,5.0f);
    glFogf(GL_FOG_END,25.0f);
}

void initEnvironment() {
    genTextures();
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING); glEnable(GL_NORMALIZE); glShadeModel(GL_SMOOTH);
    GLfloat gAmb[]={0.15f,0.15f,0.15f,1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,gAmb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glClearColor(0.01f,0.0f,0.02f,1);
}

void renderEnvironment() {
    const float S=2.0f, WH=2.5f;
    for (int i=0;i<MAZE_HEIGHT;i++)
        for (int j=0;j<MAZE_WIDTH;j++)
            drawTexturedFloor((float)j*S,(float)i*S,S);

    tCount=0;
    for (int i=0;i<MAZE_HEIGHT;i++) {
        for (int j=0;j<MAZE_WIDTH;j++) {
            if (mazeMatrix[i][j]==1) {
                float wx=(float)j*S, wz=(float)i*S;
                drawWallCube(wx,wz,S,WH);
                if ((i+j)%15==0&&tCount<6&&j>0&&mazeMatrix[i][j-1]==0) {
                    tX[tCount]=wx+0.1f; tY[tCount]=WH*0.6f; tZ[tCount]=wz+S*0.5f;
                    drawTorch(tX[tCount],tY[tCount],tZ[tCount],tCount);
                    tCount++;
                }
            }
        }
    }
    drawShadows();
}

// ===================== FIXED: drawMinimap() dengan semua entitas =====================
void drawMinimap() {
    // Tidak perlu extern apapun � semua via getter dari Entity.h

    int winW=glutGet(GLUT_WINDOW_WIDTH);
    int winH=glutGet(GLUT_WINDOW_HEIGHT);
    if (winH==0) winH=1;

    const int CELL=5, MARGIN=10;
    const int MMAP_W=MAZE_WIDTH*CELL, MMAP_H=MAZE_HEIGHT*CELL;
    const int ORIG_X=winW-MMAP_W-MARGIN, ORIG_Y=winH-MMAP_H-MARGIN;
    const float S=2.0f;

    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG); glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0,winW,0,winH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Background panel
    glColor4f(0,0,0,0.65f);
    glBegin(GL_QUADS);
        glVertex2i(ORIG_X-4,ORIG_Y-4); glVertex2i(ORIG_X+MMAP_W+4,ORIG_Y-4);
        glVertex2i(ORIG_X+MMAP_W+4,ORIG_Y+MMAP_H+20); glVertex2i(ORIG_X-4,ORIG_Y+MMAP_H+20);
    glEnd();

    // Label "MINIMAP"
    glColor4f(0.7f,0.7f,0.7f,1);
    glRasterPos2i(ORIG_X, ORIG_Y+MMAP_H+6);
    const char* lbl="MINIMAP";
    for (int i=0;lbl[i];i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,lbl[i]);

    float playerCellX=camX/S, playerCellZ=camZ/S;
    float sdx=lookX-camX, sdz=lookZ-camZ;
    float slen=sqrtf(sdx*sdx+sdz*sdz);
    if(slen<0.001f) slen=0.001f;
    sdx/=slen; sdz/=slen;

    const float SPOTLIGHT_RANGE=5.0f;
    const float SPOTLIGHT_COS=cosf(45.0f*3.14159265f/180.0f);

    // Gambar cell maze
    for (int row=0;row<MAZE_HEIGHT;row++) {
        for (int col=0;col<MAZE_WIDTH;col++) {
            float tcx=(float)col+0.5f-playerCellX;
            float tcz=(float)row+0.5f-playerCellZ;
            float tlen=sqrtf(tcx*tcx+tcz*tcz);
            bool visible=false;
            if (tlen<SPOTLIGHT_RANGE) {
                if (tlen<1.5f) visible=true;
                else {
                    float dot=(tcx/tlen)*sdx+(tcz/tlen)*sdz;
                    if (dot>=SPOTLIGHT_COS) visible=true;
                }
            }
            int px=ORIG_X+col*CELL;
            int py=ORIG_Y+(MAZE_HEIGHT-1-row)*CELL;
            if      (!visible)             glColor4f(0.0f,0.0f,0.0f,0.9f);
            else if (mazeMatrix[row][col]) glColor4f(0.45f,0.22f,0.12f,0.95f);
            else                           glColor4f(0.55f,0.53f,0.50f,0.90f);
            glBegin(GL_QUADS);
                glVertex2i(px,py); glVertex2i(px+CELL,py);
                glVertex2i(px+CELL,py+CELL); glVertex2i(px,py+CELL);
            glEnd();
        }
    }

    // Exit point (cyan) � pakai getter
    {
        float ex = getExitX(), ez = getExitZ();
        int ex2=(int)(ex/S), ez2=(int)(ez/S);
        int px=ORIG_X+ex2*CELL+CELL/2;
        int py=ORIG_Y+(MAZE_HEIGHT-1-ez2)*CELL+CELL/2;
        float pulse=0.5f+0.5f*sinf(g_time*4.0f);
        glColor4f(0.0f,1.0f,0.9f,0.6f+0.4f*pulse);
        glBegin(GL_QUADS);
            glVertex2i(px-3,py-3); glVertex2i(px+3,py-3);
            glVertex2i(px+3,py+3); glVertex2i(px-3,py+3);
        glEnd();
    }

    // Coin aktif (hijau) � pakai getter
    {
        int numCoins = getNumCoins();
        for (int i=0;i<numCoins;i++) {
            if (!getCoinActive(i)) continue;
            int cx2=(int)(getCoinX(i)/S), cz2=(int)(getCoinZ(i)/S);
            int px=ORIG_X+cx2*CELL+CELL/2;
            int py=ORIG_Y+(MAZE_HEIGHT-1-cz2)*CELL+CELL/2;
            glColor4f(0.2f,1.0f,0.2f,0.9f);
            glBegin(GL_QUADS);
                glVertex2i(px-1,py-1); glVertex2i(px+2,py-1);
                glVertex2i(px+2,py+2); glVertex2i(px-1,py+2);
            glEnd();
        }
    }

    // Key (emas) � pakai getter
    if (isKeyActive()) {
        int kx=(int)(getKeyX()/S), kz=(int)(getKeyZ()/S);
        int px=ORIG_X+kx*CELL+CELL/2;
        int py=ORIG_Y+(MAZE_HEIGHT-1-kz)*CELL+CELL/2;
        glColor4f(1.0f,0.8f,0.0f,1.0f);
        glBegin(GL_QUADS);
            glVertex2i(px-2,py-2); glVertex2i(px+2,py-2);
            glVertex2i(px+2,py+2); glVertex2i(px-2,py+2);
        glEnd();
    }

    // Ghost (merah berkedip) � pakai getter
    if (isGhostTriggered) {
        int gx=(int)(getGhostX()/S), gz=(int)(getGhostZ()/S);
        int px=ORIG_X+gx*CELL+CELL/2;
        int py=ORIG_Y+(MAZE_HEIGHT-1-gz)*CELL+CELL/2;
        float pulse=0.5f+0.5f*sinf(g_time*8.0f);
        glColor4f(1.0f,0.0f,0.0f,0.6f+0.4f*pulse);
        glBegin(GL_QUADS);
            glVertex2i(px-3,py-3); glVertex2i(px+3,py-3);
            glVertex2i(px+3,py+3); glVertex2i(px-3,py+3);
        glEnd();
    }

    // Titik pemain (kuning)
    int pdx=ORIG_X+(int)(playerCellX*CELL);
    int pdz=ORIG_Y+(MAZE_HEIGHT-1-(int)playerCellZ)*CELL;
    glColor4f(1.0f,0.9f,0.1f,1.0f);
    glBegin(GL_QUADS);
        glVertex2i(pdx-3,pdz-3); glVertex2i(pdx+3,pdz-3);
        glVertex2i(pdx+3,pdz+3); glVertex2i(pdx-3,pdz+3);
    glEnd();

    // Arah pandang (segitiga)
    glColor4f(1.0f,1.0f,0.3f,0.8f);
    float ax=pdx+sdx*8, az=pdz-sdz*8;
    float px1=ax-sdz*3, pz1=az-sdx*3;
    float px2=ax+sdz*3, pz2=az+sdx*3;
    glBegin(GL_TRIANGLES);
        glVertex2f(pdx,pdz); glVertex2f(px1,pz1); glVertex2f(px2,pz2);
    glEnd();

    glDisable(GL_BLEND);

    // Legend
    float ly = ORIG_Y - 18;
    glColor3f(1.0f,0.9f,0.1f); glRasterPos2f(ORIG_X,ly);    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,'Y');
    glColor3f(0.2f,1.0f,0.2f); glRasterPos2f(ORIG_X+12,ly); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,'C');
    glColor3f(1.0f,0.8f,0.0f); glRasterPos2f(ORIG_X+24,ly); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,'K');
    glColor3f(1.0f,0.0f,0.0f); glRasterPos2f(ORIG_X+36,ly); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,'G');
    glColor3f(0.0f,1.0f,0.9f); glRasterPos2f(ORIG_X+48,ly); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,'E');

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING);
}


