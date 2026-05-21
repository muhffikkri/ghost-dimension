// Nama File : main.cpp
// Deskripsi : Titik masuk aplikasi dan loop utama rendering game.
// Tanggal Dibuat : 24 Maret 2026

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/Entity.h"
#include "../include/Config.h"
#include "../include/Environment.h"
#include "../include/Camera.h"

enum AppMode { MODE_GAME, MODE_ENV_DEMO, MODE_ENTITY_DEMO };
static AppMode g_mode = MODE_GAME;

bool g_keyW=false, g_keyA=false, g_keyS=false, g_keyD=false;
bool g_keyUp=false, g_keyDown=false, g_keyLeft=false, g_keyRight=false;

// FIXED: Settings overlay toggle
static bool g_showSettings = false;

static float g_envYaw=0.0f, g_envX=3.0f, g_envZ=3.0f;
static float g_rotX=20.0f, g_rotY=-30.0f, g_rotZ=0.0f;
static int   g_viewMode=7, g_entityMode=0;

static void drawText2D(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* p=text;*p;++p) glutBitmapCharacter(GLUT_BITMAP_9_BY_15,*p);
}

static void drawModeBanner(const char* text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    glOrtho(0,WINDOW_WIDTH,0,WINDOW_HEIGHT,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING);
    glColor3f(1.0f,0.9f,0.2f);
    drawText2D(16,WINDOW_HEIGHT-20,text);
    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

static void resetEntityDemoState() { g_rotX=0;g_rotY=0;g_rotZ=0;g_viewMode=7;g_entityMode=0; }

static void applyEntityDemoCamera() {
    int w=glutGet(GLUT_WINDOW_WIDTH), h=glutGet(GLUT_WINDOW_HEIGHT);
    float asp=(float)w/h;
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    if (g_viewMode==7) {
        gluPerspective(45,asp,0.1,100);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        gluLookAt(0,0,5,0,0,0,0,1,0);
    } else {
        float s=2.5f;
        glOrtho(-s*asp,s*asp,-s,s,-20,20);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        switch(g_viewMode){
            case 1:gluLookAt(0,0,5,0,0,0,0,1,0);break;
            case 2:gluLookAt(0,0,-5,0,0,0,0,1,0);break;
            case 3:gluLookAt(-5,0,0,0,0,0,0,1,0);break;
            case 4:gluLookAt(5,0,0,0,0,0,0,1,0);break;
            case 5:gluLookAt(0,5,0,0,0,0,0,0,-1);break;
            case 6:gluLookAt(0,-5,0,0,0,0,0,0,1);break;
        }
    }
}

static void displayGame() {
    playerCam.apply();

    camX = playerCam.x; camY = playerCam.y; camZ = playerCam.z;
    lookX = playerCam.x + cosf(playerCam.angle) * cosf(playerCam.pitch);
    lookY = playerCam.y + sinf(playerCam.pitch);
    lookZ = playerCam.z + sinf(playerCam.angle) * cosf(playerCam.pitch);

    setupFlashlight();
    setupFog();
    renderEnvironment();

    // FIXED: sync posisi dulu
    syncPlayerPosition(playerCam.x, playerCam.z);

    // FIXED: update logika (pickup, status) � TANPA render
    // render entitas dilakukan di updateItems() tapi pastikan
    // matrix state sudah benar (GL_MODELVIEW Identity dari apply())
    glMatrixMode(GL_MODELVIEW);  // pastikan di modelview sebelum render entitas
    updateItems();
    updateGhost(playerCam.x, playerCam.z);

    float ghostDist = getGhostDist();

    // HUD � semua di bawah ini sudah pakai begin2D/end2D di Camera.cpp
    drawMinimap();

    if (!isGameOver && !isGameWin) {
        drawGameHUD();
        drawGhostWarning(ghostDist);
        if (g_showSettings) drawSettings();
    } else if (isGameOver) {
        drawGameOver();
    } else {
        drawGameWin();
    }

    // drawModeBanner("MODE: GAME  |  F1=Game  F2=EnvDemo  F3=EntityDemo  TAB=Settings");
}

static void displayEnvDemo() {
    float rad=g_envYaw*3.14159265f/180.0f;
    camX=g_envX; camY=1.6f; camZ=g_envZ;
    lookX=g_envX+cosf(rad); lookY=1.6f; lookZ=g_envZ+sinf(rad);
    gluLookAt(camX,camY,camZ,lookX,lookY,lookZ,0,1,0);
    setupFlashlight(); setupFog(); renderEnvironment();
    drawMinimap();
    // drawModeBanner("MODE: ENV DEMO  |  W/S/A/D move  |  F1/F2/F3=Mode");
}

static void displayEntityDemo() {
    applyEntityDemoCamera();
    glRotatef(g_rotX,1,0,0); glRotatef(g_rotY,0,1,0); glRotatef(g_rotZ,0,0,1);
    if      (g_entityMode==0) drawGhostDemo(0,0,0);
    else if (g_entityMode==1) drawCoinDemo(0,0,0);
    else if (g_entityMode==2) drawKeyDemo(0,0,0);

    // HUD entity demo
    int w=glutGet(GLUT_WINDOW_WIDTH), h=glutGet(GLUT_WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0,w,0,h);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
    const char* nameE[]={"GHOST (G)","COIN (C)","KEY (K)"};
    char info[160];
    sprintf(info,"Entity:%s  Rot X=%.0f Y=%.0f Z=%.0f  | Arrow=RotXY  Z/X=RotZ  1-7=View  G/C/K=Entity  R=Reset",
            nameE[g_entityMode],g_rotX,g_rotY,g_rotZ);
    glColor3f(1,1,0); glRasterPos2i(10,h-22);
    for(int i=0;info[i];i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,info[i]);
}

static void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if      (g_mode==MODE_GAME)        displayGame();
    else if (g_mode==MODE_ENV_DEMO)    displayEnvDemo();
    else                               displayEntityDemo();
    glutSwapBuffers();
}

static void timer(int v) {
    static float lastTime    = 0.0f;
    static float bobbingSpeed = 0.0f;
    static float bobbingAmp   = 0.0f;

    float now = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float dt  = now - lastTime;
    lastTime  = now;

    if (dt < 0.0f)  dt = 0.0f;
    if (dt > 0.05f) dt = 0.05f;

    extern float g_time;
    g_time += dt;

    if (g_mode == MODE_GAME && !isGameOver && !isGameWin) {
        // -- Look ----------------------------------------------
        float lookSpeed = 1.8f * dt;
        if (g_keyLeft)  playerCam.angle -= lookSpeed;
        if (g_keyRight) playerCam.angle += lookSpeed;
        if (g_keyUp)    playerCam.pitch += lookSpeed;
        if (g_keyDown)  playerCam.pitch -= lookSpeed;
        if (playerCam.pitch >  1.2f) playerCam.pitch =  1.2f;
        if (playerCam.pitch < -1.2f) playerCam.pitch = -1.2f;

        // -- Movement ------------------------------------------
        float speed = PLAYER_SPEED * 20.0f * dt;
        float fx =  cosf(playerCam.angle) * cosf(playerCam.pitch);
        float fz =  sinf(playerCam.angle) * cosf(playerCam.pitch);
        float rx = -sinf(playerCam.angle);
        float rz =  cosf(playerCam.angle);

        float mvX = 0.0f, mvZ = 0.0f;
        if (g_keyW) { mvX += fx; mvZ += fz; }
        if (g_keyS) { mvX -= fx; mvZ -= fz; }
        if (g_keyA) { mvX -= rx; mvZ -= rz; }
        if (g_keyD) { mvX += rx; mvZ += rz; }

        float isMoving = 0.0f;
        if (mvX != 0.0f || mvZ != 0.0f) {
            float len = sqrtf(mvX*mvX + mvZ*mvZ);
            mvX /= len; mvZ /= len;
            float nx = playerCam.x + mvX * speed;
            float nz = playerCam.z + mvZ * speed;
            if (!checkCollision(nx, playerCam.z)) playerCam.x = nx;
            if (!checkCollision(playerCam.x, nz)) playerCam.z = nz;
            isMoving = 1.0f;
        }

        // -- Head bobbing smooth -------------------------------
        float targetSpeed = isMoving * 5.5f;
        bobbingSpeed += (targetSpeed - bobbingSpeed) * 10.0f * dt;
        playerCam.bobbingTimer += bobbingSpeed * dt;

        float targetAmp = isMoving * 0.035f;
        bobbingAmp += (targetAmp - bobbingAmp) * 8.0f * dt;

        float targetY = 1.6f + bobbingAmp * sinf(playerCam.bobbingTimer);
        playerCam.y  += (targetY - playerCam.y) * 12.0f * dt;
    }

    glutPostRedisplay();
    glutTimerFunc(1000/60, timer, 0);
}

static void keyboard(unsigned char key, int x, int y) {
    (void)x;(void)y;
    if(key==27) exit(0);

    // FIXED: TAB toggle settings (mode game)
    if (key=='\t' && g_mode==MODE_GAME) {
        g_showSettings=!g_showSettings;
        return;
    }

    if (g_mode==MODE_ENTITY_DEMO) {
        switch(key){
            case 'g':case 'G':g_entityMode=0;resetEntityDemoState();break;
            case 'c':case 'C':g_entityMode=1;resetEntityDemoState();break;
            case 'k':case 'K':g_entityMode=2;resetEntityDemoState();break;
            case '1':g_viewMode=1;g_rotX=g_rotY=g_rotZ=0;break;
            case '2':g_viewMode=2;g_rotX=g_rotY=g_rotZ=0;break;
            case '3':g_viewMode=3;g_rotX=g_rotY=g_rotZ=0;break;
            case '4':g_viewMode=4;g_rotX=g_rotY=g_rotZ=0;break;
            case '5':g_viewMode=5;g_rotX=g_rotY=g_rotZ=0;break;
            case '6':g_viewMode=6;g_rotX=g_rotY=g_rotZ=0;break;
            case '7':g_viewMode=7;break;
            case 'z':case 'Z':g_rotZ+=5;g_viewMode=7;break;
            case 'x':case 'X':g_rotZ-=5;g_viewMode=7;break;
            case 'r':case 'R':resetEntityDemoState();break;
        }
    } else if (g_mode==MODE_GAME) {
        switch(key){
            case 'w':case 'W':g_keyW=true;break;
            case 'a':case 'A':g_keyA=true;break;
            case 's':case 'S':g_keyS=true;break;
            case 'd':case 'D':g_keyD=true;break;
            case 'r':case 'R':
                if(isGameOver||isGameWin){
                    initEntities();
                    playerCam.x=3.0f;playerCam.y=1.6f;playerCam.z=3.0f;
                    playerCam.angle=3.9269907f;playerCam.pitch=0;
                    playerCam.bobbingTimer=0;
                    g_showSettings=false;
                }
                break;
        }
    } else if (g_mode==MODE_ENV_DEMO) {
        const float STEP=0.15f;
        float rad=g_envYaw*3.14159265f/180.0f;
        switch(key){
            case 'w':case 'W':{float nx=g_envX+cosf(rad)*STEP,nz=g_envZ+sinf(rad)*STEP;if(!checkCollision(nx,nz)){g_envX=nx;g_envZ=nz;}break;}
            case 's':case 'S':{float nx=g_envX-cosf(rad)*STEP,nz=g_envZ-sinf(rad)*STEP;if(!checkCollision(nx,nz)){g_envX=nx;g_envZ=nz;}break;}
            case 'a':case 'A':g_envYaw-=3;break;
            case 'd':case 'D':g_envYaw+=3;break;
        }
    }
}

static void keyboardUp(unsigned char key, int x, int y) {
    (void)x;(void)y;
    switch(key){
        case 'w':case 'W':g_keyW=false;break;
        case 'a':case 'A':g_keyA=false;break;
        case 's':case 'S':g_keyS=false;break;
        case 'd':case 'D':g_keyD=false;break;
    }
}

static void specialKey(int key, int x, int y) {
    (void)x;(void)y;
    if(key==GLUT_KEY_F1){g_mode=MODE_GAME;return;}
    if(key==GLUT_KEY_F2){g_mode=MODE_ENV_DEMO;return;}
    if(key==GLUT_KEY_F3){g_mode=MODE_ENTITY_DEMO;resetEntityDemoState();return;}
    if(g_mode==MODE_ENTITY_DEMO){
        switch(key){
            case GLUT_KEY_UP:g_rotX-=5;break;case GLUT_KEY_DOWN:g_rotX+=5;break;
            case GLUT_KEY_LEFT:g_rotY-=5;break;case GLUT_KEY_RIGHT:g_rotY+=5;break;
        }
    } else if(g_mode==MODE_GAME){
        switch(key){
            case GLUT_KEY_UP:g_keyUp=true;break;case GLUT_KEY_DOWN:g_keyDown=true;break;
            case GLUT_KEY_LEFT:g_keyLeft=true;break;case GLUT_KEY_RIGHT:g_keyRight=true;break;
        }
    } else if(g_mode==MODE_ENV_DEMO){
        switch(key){
            case GLUT_KEY_LEFT:g_envYaw-=3;break;case GLUT_KEY_RIGHT:g_envYaw+=3;break;
        }
    }
}

static void specialKeyUp(int key, int x, int y) {
    (void)x;(void)y;
    switch(key){
        case GLUT_KEY_UP:g_keyUp=false;break;case GLUT_KEY_DOWN:g_keyDown=false;break;
        case GLUT_KEY_LEFT:g_keyLeft=false;break;case GLUT_KEY_RIGHT:g_keyRight=false;break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
    glutCreateWindow("GTI A1 - Ghost Dimension Maze  |  F1=Game  F2=Env  F3=Entity");

    srand((unsigned)time(NULL));
    generateMaze();
    initEnvironment();
    initEntities();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
    glutTimerFunc(0,timer,0);
    glutMainLoop();
    return 0;
}


