/*
    Environment.cpp  -  Abyssal Maze  (Yu Ming Jie)
    - Dinding: kubus 4 sisi (terlihat dari atas di ortho)
    - Spotlight GL_LIGHT0: senter pemain, ikut kamera
    - Obor/lilin GL_LIGHT1-6: point light merah flicker di dinding
    - Shadow sederhana: quad hitam transparan di lantai (shadow matrix)
    - GL_DEPTH_TEST aktif
*/

#include <GL/glut.h>
#include <cmath>
#include "Config.h"
#include "Environment.h"

/* ------------------------------------------------------------------ */
/*  Globals                                                            */
/* ------------------------------------------------------------------ */
GLuint g_texWall  = 0;
GLuint g_texFloor = 0;
float  g_time     = 0.0f;

float camX  = 3.0f, camY = 1.6f, camZ = 3.0f;
float lookX = 5.0f, lookY = 1.6f, lookZ = 5.0f;

/* Posisi obor (diisi saat render pertama) */
static float tX[6], tY[6], tZ[6];
static int   tCount = 0;

/* ------------------------------------------------------------------ */
/*  Hash noise                                                         */
/* ------------------------------------------------------------------ */
float hashf(unsigned int s) {
    s ^= s << 13; s ^= s >> 17; s ^= s << 5;
    return (float)(s & 0xFFFF) / 65536.0f;
}

/* ------------------------------------------------------------------ */
/*  genTextures()                                                      */
/* ------------------------------------------------------------------ */
void genTextures() {
    const int W = 64, H = 64;
    unsigned char wData[W*H*4], fData[W*H*4];
    int x, y;

    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            int i = (y*W+x)*4;

            /* WALL: bata Han Dynasty */
            int brow  = y/10;
            int bx    = (brow%2==0)?(x%18):((x+9)%18);
            int by    = y%10;
            int mort  = (bx<2)||(by<2);
            unsigned char bW = mort ? 22
                : (unsigned char)(40 + hashf((unsigned)(x*31+y*97))*18.0f);
            wData[i+0] = bW+22; wData[i+1] = bW;
            wData[i+2] = (bW>4)?bW-4:0; wData[i+3] = 255;
            /* noda darah */
            if (!mort && hashf((unsigned)(x*211+y*97+0xDEAD))>0.93f)
                { wData[i+0]=118; wData[i+1]=8; wData[i+2]=8; }
            /* jimat hijau */
            if (!mort && hashf((unsigned)(x*503+y*251+0xFADE))>0.97f) {
                wData[i+0]=(unsigned char)(wData[i+0]*0.5f+15);
                wData[i+1]=(unsigned char)(wData[i+1]*0.5f+44);
                wData[i+2]=(unsigned char)(wData[i+2]*0.5f+22);
            }

            /* FLOOR: ubin hitam dingin */
            int grt = (x%32==0)||(y%32==0);
            unsigned char bF = grt ? 10
                : (unsigned char)(26+hashf((unsigned)(x*151+y*199))*14.0f);
            fData[i+0]=bF; fData[i+1]=bF; fData[i+2]=bF+12; fData[i+3]=255;
            if (!grt && hashf((unsigned)(x*337+y*421+0xB10D))>0.96f)
                { fData[i+0]=90; fData[i+1]=5; fData[i+2]=5; }
            if (!grt && hashf((unsigned)(x*613+y*719+0xF1A9))>0.985f)
                { fData[i+0]=210; fData[i+1]=172; fData[i+2]=18; }
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

/* ==================================================================
   setupFlashlight()
   GL_LIGHT0: spotlight senter pemain, posisi = kamera
   Dipanggil main.cpp SETELAH gluLookAt() setiap frame
   ================================================================== */
void setupFlashlight() {
    glEnable(GL_LIGHT0);

    GLfloat pos[4] = { camX, camY, camZ, 1.0f };
    GLfloat dir[3] = { lookX-camX, lookY-camY, lookZ-camZ };

    GLfloat amb[]  = { 0.10f, 0.18f, 0.15f, 1.0f };
    GLfloat diff[] = { 1.8f, 1.8f, 1.6f, 1.0f };
    GLfloat spec[] = { 0.60f, 1.00f, 0.90f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION,       pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
    glLightfv(GL_LIGHT0, GL_AMBIENT,        amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,        diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR,       spec);

    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,          30.0f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT,          25.0f);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,   1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,     0.02f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION,  0.001f);
}
    
    
    
    

/* ==================================================================
   drawTorchFlame()
   Obor/lilin kecil: tiga segitiga kuning-oranye di atas sumbu
   Digambar tanpa lighting (emissive), berkedip scale Y
   ================================================================== */
void drawTorchFlame(float flickVal) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);   /* selalu di atas sumbu agar terlihat */

    /* Tangkai lilin (silinder pendek coklat tua) */
    glColor3f(0.35f, 0.22f, 0.12f);
    glPushMatrix();
    glScalef(0.04f, 0.18f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();

    /* Nyala api: 3 kerucut transparan bertumpuk, scale Y berkedip */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPushMatrix();
    glTranslatef(0.0f, 0.18f, 0.0f);          /* naik ke atas tangkai */
    glScalef(1.0f, flickVal, 1.0f);             /* scale Y = flicker   */

    /* kerucut besar luar: oranye */
    glColor4f(1.0f, 0.35f, 0.0f, 0.55f);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.07f, 0.22f, 8, 4);
    glPopMatrix();

    /* kerucut tengah: kuning */
    glColor4f(1.0f, 0.80f, 0.1f, 0.70f);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.04f, 0.18f, 8, 4);
    glPopMatrix();

    /* inti putih kecil */
    glColor4f(1.0f, 1.0f, 0.85f, 0.85f);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.15f, 0.10f, 6, 2);
    glPopMatrix();
    
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}
    


/* ==================================================================
   drawTorch()
   Satu obor: point light GL_LIGHT(1+id) + mesh lilin + nyala
   Dipasang di samping dinding, setinggi mata
   ================================================================== */
void drawTorch(float x, float y, float z, int id) {
    // Pakai kombinasi sin yang lebih acak
    float f = 0.5f * (sinf(g_time * 10.0f + id) * 0.5f + 0.5f) +
              0.3f * (sinf(g_time * 25.0f + id * 2) * 0.5f + 0.5f) +
              0.2f * (sinf(g_time * 50.0f) * 0.5f + 0.5f);
    
    // Rentang kedipan: 0.4 (redup) sampai 1.0 (terang)
    f = 0.4f + 0.6f * f; 

    GLenum lid = (GLenum)(GL_LIGHT1 + id);
    float dx = camX - x;
	float dz = camZ - z;
	float dist = sqrt(dx*dx + dz*dz);
	
	// Aktifkan lampu hanya jika dekat
	if (dist < 6.0f)
	    glEnable(lid);
	else {
	    glDisable(lid);
	    return; //tidak lanjut render cahaya
	}

    GLfloat lPos[]  = { x, y + 0.22f, z, 1.0f };
    // Efek flicker diaplikasikan ke warna Diffuse dan Ambient
    GLfloat lDiff[] = { 0.6f * f, 0.15f * f, 0.03f * f, 1.0f }; 
    GLfloat lAmb[]  = { 0.15f * f, 0.05f * f, 0.0f, 1.0f };
    GLfloat lSpec[] = { 0.4f*f, 0.1f*f, 0.0f, 1.0f };

	glLightfv(lid, GL_POSITION, lPos);
    glLightfv(lid, GL_DIFFUSE,  lDiff);
    glLightfv(lid, GL_AMBIENT,  lAmb);
    glLightfv(lid, GL_SPECULAR, lSpec);
    
    glLightf(lid, GL_SPOT_CUTOFF,          180.0f);
    glLightf(lid, GL_CONSTANT_ATTENUATION,   1.0f);
   glLightf(lid, GL_LINEAR_ATTENUATION,     1.2f);
	glLightf(lid, GL_QUADRATIC_ATTENUATION,  0.8f);
    
    // Mesh apinya juga harus ikut mengecil/membesar
    glPushMatrix();
        glTranslatef(x, y, z);
        drawTorchFlame(0.8f + 0.4f * f); 
    glPopMatrix();
}

/* ==================================================================
   drawWallCube()
   Dinding 1 sel = kubus penuh 4 sisi vertikal + atap
   Semua sisi bertekstur ? terlihat dari SEMUA sudut pandang
   ================================================================== */
void drawWallCube(float wx, float wz, float S, float WH) {
    float x0=wx, x1=wx+S;
    float z0=wz, z1=wz+S;
    float y0=0.0f, y1=WH;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_texWall);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);

    /* Sisi DEPAN (Z+) */
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(x0,y0,z1);
    glTexCoord2f(1,0); glVertex3f(x1,y0,z1);
    glTexCoord2f(1,1); glVertex3f(x1,y1,z1);
    glTexCoord2f(0,1); glVertex3f(x0,y1,z1);

    /* Sisi BELAKANG (Z-) */
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(x1,y0,z0);
    glTexCoord2f(1,0); glVertex3f(x0,y0,z0);
    glTexCoord2f(1,1); glVertex3f(x0,y1,z0);
    glTexCoord2f(0,1); glVertex3f(x1,y1,z0);

    /* Sisi KIRI (X-) */
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(x0,y0,z0);
    glTexCoord2f(1,0); glVertex3f(x0,y0,z1);
    glTexCoord2f(1,1); glVertex3f(x0,y1,z1);
    glTexCoord2f(0,1); glVertex3f(x0,y1,z0);

    /* Sisi KANAN (X+) */
    glNormal3f( 1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(x1,y0,z1);
    glTexCoord2f(1,0); glVertex3f(x1,y0,z0);
    glTexCoord2f(1,1); glVertex3f(x1,y1,z0);
    glTexCoord2f(0,1); glVertex3f(x1,y1,z1);

    /* ATAP (Y+) */
    glNormal3f( 0, 1, 0);
    glTexCoord2f(0,0); glVertex3f(x0,y1,z0);
    glTexCoord2f(1,0); glVertex3f(x1,y1,z0);
    glTexCoord2f(1,1); glVertex3f(x1,y1,z1);
    glTexCoord2f(0,1); glVertex3f(x0,y1,z1);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

/* ==================================================================
   drawFloorTile()
   Satu quad lantai bertekstur
   ================================================================== */
void drawTexturedFloor(float x, float z, float s) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_texFloor);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(x,   0,z  );
    glTexCoord2f(1,0); glVertex3f(x+s, 0,z  );
    glTexCoord2f(1,1); glVertex3f(x+s, 0,z+s);
    glTexCoord2f(0,1); glVertex3f(x,   0,z+s);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}




/* ==================================================================
   drawShadow()
   Shadow sederhana: quad hitam transparan di lantai, sedikit di
   bawah posisi obor, mensimulasikan bayangan titik cahaya.
   Technique: disable depth write, blend multiply-ish
   ================================================================== */
void drawShadows() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);              /* tidak tulis depth */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int k;
    for (k=0; k<tCount; k++) {
        /* Radius shadow: semakin jauh dari sumber = makin besar */
        float sr = 1.0f;
        float sx = tX[k], sz = tZ[k];
        float sy = 0.02f;               /* sedikit di atas lantai */

        float f  = 0.45f + 0.55f*(0.5f+0.5f*sinf(g_time*3.8f+(float)k*1.3f));
        float alpha = 0.7f * f;

        glColor4f(0.0f, 0.0f, 0.0f, alpha);
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

/* ==================================================================
   setupFog()
   ================================================================== */
void setupFog() {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    GLfloat fc[] = { 0.04f, 0.01f, 0.04f, 1.0f };
    glFogfv(GL_FOG_COLOR, fc);
    glFogf(GL_FOG_DENSITY, 0.07f+0.005f*sinf(g_time*0.35f));
    glHint(GL_FOG_HINT, GL_NICEST);
}

/* ==================================================================
   initEnvironment()
   ================================================================== */
void initEnvironment() {
    genTextures();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    GLfloat gAmb[] = { 0.03f, 0.02f, 0.02f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gAmb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glClearColor(0.01f, 0.0f, 0.02f, 1.0f);
}

/* ==================================================================
   drawVolumetricSpotlight()
   Kerucut cahaya visual dari posisi kamera ke arah pandang.
   Arah dinormalisasi agar panjang selalu konsisten.
   ================================================================== */
void drawVolumetricSpotlight() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Efek glow
    glDisable(GL_LIGHTING); 

    glPushMatrix();
        // Senter nempel di posisi kamera
        glTranslatef(camX, camY, camZ);
        
        glColor4f(1.3f, 1.2f, 0.9f, 0.9f); 
        
        glBegin(GL_TRIANGLE_FAN);
            // Pangkal senter di mata
            glVertex3f(0, 0, 0); 
            
            float radius = 1.5f;    // Lebar sorot ujung
            float distance = 15.0f; // Jauh sorotan
            
            // Hitung arah pandang
            float dx = lookX - camX;
            float dy = lookY - camY;
            float dz = lookZ - camZ;

            // Bikin lingkaran di ujung sorotan
            for(int i = 0; i <= 24; i++) {
                float angle = i * 2.0f * 3.14159f / 24.0f;
                float ox = cos(angle) * radius;
                float oy = sin(angle) * radius;
                
                // Nembak ke depan sesuai arah pandang
                glVertex3f(dx * distance + ox, dy * distance + oy, dz * distance);
            }
        glEnd();
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}
 


/* ==================================================================
   renderEnvironment()
   Urutan render:
     1. Lantai semua sel
     2. Dinding kubus 5-sisi setiap sel=1
     3. Obor di sel dinding tertentu (isi tX/tY/tZ)
     4. Shadow quad di lantai bawah tiap obor
   main.cpp harus set cam/look, lalu panggil setupFlashlight() 
   SEBELUM memanggil renderEnvironment().
  ================================================================== */
  void renderEnvironment() {
    const float S  = 2.0f;
    const float WH = 2.5f;
    int i, j;

    /* ---- Pass 1: Lantai ---- */
    for (int i=0; i<MAZE_HEIGHT; i++){
        for (int j=0; j<MAZE_WIDTH; j++) {
            drawTexturedFloor((float)j*S, (float)i*S, S);
        }
	}
    /* ---- Pass 2: Dinding kubus + kumpulkan posisi obor ---- */
    tCount = 0;
    for (i=0; i<MAZE_HEIGHT; i++) {
        for (j=0; j<MAZE_WIDTH; j++) {
            if (mazeMatrix[i][j]==1) {
                float wx=(float)j*S, wz=(float)i*S;
                drawWallCube(wx, wz, S, WH);

                /* Obor */
				if ((i + j) % 15 == 0 && tCount < 3 &&
				    j > 0 && mazeMatrix[i][j-1] == 0)
				{
				    tX[tCount] = wx + 0.1f;
				    tY[tCount] = WH * 0.6f;
				    tZ[tCount] = wz + S * 0.5f;
				
				    drawTorch(tX[tCount], tY[tCount], tZ[tCount], tCount);
				    tCount++;
				}
            }
        }
    }

    /* ---- Pass 3: Shadow quad di lantai (setelah semua geometri) ---- */
    drawShadows();
    
    
    drawVolumetricSpotlight();
}


