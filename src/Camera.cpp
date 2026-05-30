// Nama File : Camera.cpp
// Deskripsi : Implementasi fungsi kamera dan pengaturan tampilan pemain.
// Tanggal Dibuat : 24 Maret 2026

#include "../include/Camera.h"
#include "../include/Config.h"
#include "../include/Entity.h"
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

Camera playerCam = { 3.0f, 1.6f, 3.0f, 3.9269907f, 0.0f, 0.0f };

static void drawText2D(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* p = text; *p != '\0'; ++p)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);
}

static void drawTextSmall(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* p = text; *p != '\0'; ++p)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
}

static void drawQuad2D(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
        glVertex2f(x,   y);
        glVertex2f(x+w, y);
        glVertex2f(x+w, y+h);
        glVertex2f(x,   y+h);
    glEnd();
}

static void drawPanel(float x, float y, float w, float h,
                      float r, float g, float b, float a) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, a);
    drawQuad2D(x, y, w, h);
    glDisable(GL_BLEND);
}

static int s_w2d = 0, s_h2d = 0;
static void begin2D() {
    s_w2d = glutGet(GLUT_WINDOW_WIDTH);
    s_h2d = glutGet(GLUT_WINDOW_HEIGHT);
    if (s_h2d == 0) s_h2d = 1;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, s_w2d, 0, s_h2d);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

static void end2D() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Camera::apply() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0,
                   (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT,
                   0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float lx = x + cosf(angle) * cosf(pitch);
    float ly = y + sinf(pitch);
    float lz = z + sinf(angle) * cosf(pitch);
    gluLookAt(x, y, z, lx, ly, lz, 0.0f, 1.0f, 0.0f);
}

void drawGameHUD() {
    begin2D();
    float w = (float)s_w2d;
    float h = (float)s_h2d;
    drawPanel(8, h-120, 252, 112, 0.0f, 0.0f, 0.0f, 0.55f);
    char buf[128];
    sprintf(buf, "SCORE : %d", score);
    glColor3f(1.0f, 0.85f, 0.1f);
    drawText2D(18, h-30, buf);
    int coinsLeft = 0, total = getNumCoins(), i;
    for (i = 0; i < total; i++) if (getCoinActive(i)) coinsLeft++;
    sprintf(buf, "COINS : %d left", coinsLeft);
    glColor3f(0.9f, 0.7f, 0.1f);
    drawText2D(18, h-55, buf);
    if (hasKey) glColor3f(0.2f, 1.0f, 0.4f);
    else        glColor3f(0.7f, 0.7f, 0.7f);
    drawText2D(18, h-80, hasKey ? "KEY   : [OBTAINED]" : "KEY   : find it!");
    int tMin=(int)(gameDuration/60.0f), tSec=(int)(gameDuration)%60;
    sprintf(buf, "TIME  : %02d:%02d", tMin, tSec);
    glColor3f(0.7f, 0.9f, 1.0f);
    drawText2D(18, h-105, buf);
    float cx=w*0.5f, cy=h*0.5f;
    glColor3f(1.0f,1.0f,1.0f);
    glBegin(GL_LINES);
        glVertex2f(cx-10,cy); glVertex2f(cx+10,cy);
        glVertex2f(cx,cy-10); glVertex2f(cx,cy+10);
    glEnd();
    glColor3f(0.55f,0.55f,0.55f);
    drawTextSmall(10, 10, "WASD=Move  Arrow=Look  TAB=Settings  R=Restart");
    end2D();
}

void drawGhostWarning(float ghostDist) {
    if (ghostDist > 8.0f) return;
    extern float g_time;
    begin2D();
    float w=(float)s_w2d, h=(float)s_h2d;
    float intensity=1.0f-(ghostDist/8.0f);
    float pulse=0.5f+0.5f*sinf(g_time*10.0f);
    float alpha=intensity*0.5f*(0.4f+0.6f*pulse);
    float bw=60.0f*intensity;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.9f,0.0f,0.0f,alpha);
    drawQuad2D(0,0,bw,h);
    drawQuad2D(w-bw,0,bw,h);
    drawQuad2D(0,0,w,bw);
    drawQuad2D(0,h-bw,w,bw);
    glDisable(GL_BLEND);
    if (ghostDist < 4.0f) {
        glColor3f(1.0f,0.1f,0.1f);
        drawText2D(w*0.5f-90, h*0.5f-60, "!! GHOST NEARBY !!");
    }
    end2D();
}

void drawGameOver() {
    extern float g_time;
    begin2D();
    float w=(float)s_w2d, h=(float)s_h2d;
    float cx=w*0.5f, cy=h*0.5f, bw=480.0f, bh=380.0f;
    int i;
    drawPanel(0,0,w,h,0.0f,0.0f,0.0f,0.75f);
    drawPanel(cx-bw*0.5f,cy-bh*0.5f,bw,bh,0.12f,0.0f,0.0f,0.95f);
    glColor3f(0.8f,0.0f,0.0f); glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx-bw*0.5f,cy-bh*0.5f); glVertex2f(cx+bw*0.5f,cy-bh*0.5f);
        glVertex2f(cx+bw*0.5f,cy+bh*0.5f); glVertex2f(cx-bw*0.5f,cy+bh*0.5f);
    glEnd(); glLineWidth(1.0f);
    float pulse=0.7f+0.3f*sinf(g_time*4.0f);
    glColor3f(1.0f*pulse,0.1f,0.1f);
    drawText2D(cx-90, cy+bh*0.5f-40, "!! GAME OVER !!");
    glColor3f(0.85f,0.6f,0.6f);
    drawText2D(cx-100, cy+bh*0.5f-70, "The ghost caught you...");
    char buf[128];
    glColor3f(1.0f,0.85f,0.2f);
    sprintf(buf,"Final Score    : %d", score);
    drawText2D(cx-140, cy+70, buf);
    int tMin=(int)(gameDuration/60.0f), tSec=(int)(gameDuration)%60;
    glColor3f(0.7f,0.9f,1.0f);
    sprintf(buf,"Time Survived  : %02d:%02d", tMin, tSec);
    drawText2D(cx-140, cy+45, buf);
    int coinsGot=0, total=getNumCoins();
    for (i=0;i<total;i++) if(!getCoinActive(i)) coinsGot++;
    glColor3f(0.9f,0.7f,0.1f);
    sprintf(buf,"Coins Collected: %d / %d", coinsGot, total);
    drawText2D(cx-140, cy+20, buf);
    glColor3f(0.9f,0.9f,0.9f);
    drawText2D(cx-80, cy-15, "--- TOP SCORES ---");
    for (i=0;i<historyCount&&i<5;i++) {
        int mm=(int)(scoreHistory[i].timeSurvived/60.0f);
        int ss=(int)(scoreHistory[i].timeSurvived)%60;
        sprintf(buf,"#%d  Score:%-5d  Time:%02d:%02d  Coins:%d",
                i+1,scoreHistory[i].score,mm,ss,scoreHistory[i].coinsCollected);
        float col=(i==0)?1.0f:0.75f-i*0.08f;
        glColor3f(col,col*0.75f,0.1f);
        drawText2D(cx-200, cy-40-i*22.0f, buf);
    }
    glColor3f(0.3f,1.0f,0.4f);
    drawText2D(cx-100, cy-bh*0.5f+22, "Press [R] to Restart");
    end2D();
}

void drawGameWin() {
    extern float g_time;
    begin2D();
    float w=(float)s_w2d, h=(float)s_h2d;
    float cx=w*0.5f, cy=h*0.5f, bw=480.0f, bh=400.0f;
    int i;
    drawPanel(0,0,w,h,0.0f,0.0f,0.0f,0.70f);
    drawPanel(cx-bw*0.5f,cy-bh*0.5f,bw,bh,0.0f,0.10f,0.08f,0.95f);
    glColor3f(0.0f,0.8f,0.5f); glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx-bw*0.5f,cy-bh*0.5f); glVertex2f(cx+bw*0.5f,cy-bh*0.5f);
        glVertex2f(cx+bw*0.5f,cy+bh*0.5f); glVertex2f(cx-bw*0.5f,cy+bh*0.5f);
    glEnd(); glLineWidth(1.0f);
    float pulse=0.7f+0.3f*sinf(g_time*3.0f);
    glColor3f(0.1f,1.0f*pulse,0.5f*pulse);
    drawText2D(cx-95, cy+bh*0.5f-40, "YOU ESCAPED!");
    char buf[128];
    glColor3f(1.0f,0.85f,0.2f);
    sprintf(buf,"Final Score    : %d", score);
    drawText2D(cx-140, cy+80, buf);
    int tMin=(int)(gameDuration/60.0f), tSec=(int)(gameDuration)%60;
    glColor3f(0.7f,0.9f,1.0f);
    sprintf(buf,"Time           : %02d:%02d", tMin, tSec);
    drawText2D(cx-140, cy+55, buf);
    int coinsGot=0, total=getNumCoins();
    for (i=0;i<total;i++) if(!getCoinActive(i)) coinsGot++;
    glColor3f(0.9f,0.7f,0.1f);
    sprintf(buf,"Coins Collected: %d / %d", coinsGot, total);
    drawText2D(cx-140, cy+30, buf);
    glColor3f(0.9f,0.9f,0.9f);
    drawText2D(cx-80, cy-10, "--- TOP SCORES ---");
    for (i=0;i<historyCount&&i<5;i++) {
        int mm=(int)(scoreHistory[i].timeSurvived/60.0f);
        int ss=(int)(scoreHistory[i].timeSurvived)%60;
        sprintf(buf,"#%d  Score:%-5d  Time:%02d:%02d  Coins:%d",
                i+1,scoreHistory[i].score,mm,ss,scoreHistory[i].coinsCollected);
        float col=(i==0)?1.0f:0.75f-i*0.08f;
        glColor3f(0.1f,col,col*0.55f);
        drawText2D(cx-200, cy-35-i*22.0f, buf);
    }
    glColor3f(0.3f,1.0f,0.4f);
    drawText2D(cx-100, cy-bh*0.5f+22, "Press [R] to Play Again");
    end2D();
}

void drawSettings() {
    begin2D();
    float w=(float)s_w2d, h=(float)s_h2d;
    float cx=w*0.5f, cy=h*0.5f, bw=520.0f, bh=480.0f;
    drawPanel(0,0,w,h,0.0f,0.0f,0.0f,0.65f);
    drawPanel(cx-bw*0.5f,cy-bh*0.5f,bw,bh,0.05f,0.04f,0.09f,0.97f);
    glColor3f(0.5f,0.3f,0.9f); glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx-bw*0.5f,cy-bh*0.5f); glVertex2f(cx+bw*0.5f,cy-bh*0.5f);
        glVertex2f(cx+bw*0.5f,cy+bh*0.5f); glVertex2f(cx-bw*0.5f,cy+bh*0.5f);
    glEnd(); glLineWidth(1.0f);
    glColor3f(0.7f,0.5f,1.0f);
    drawText2D(cx-70, cy+bh*0.5f-35, "SETTINGS & HELP");
    float y0=cy+bh*0.5f-70, lh=26.0f;
    glColor3f(0.9f,0.8f,1.0f); drawText2D(cx-220,y0,"--- MOVEMENT ---"); y0-=lh;
    glColor3f(0.75f,0.75f,0.75f);
    drawText2D(cx-220,y0,"W / S         : Move Forward / Backward"); y0-=lh;
    drawText2D(cx-220,y0,"A / D         : Strafe Left / Right");     y0-=lh;
    drawText2D(cx-220,y0,"Arrow Keys    : Look Around");             y0-=lh;
    y0-=8;
    glColor3f(0.9f,0.8f,1.0f); drawText2D(cx-220,y0,"--- GAMEPLAY ---"); y0-=lh;
    glColor3f(0.75f,0.75f,0.75f);
    drawText2D(cx-220,y0,"Coins [green] : +50 score each");          y0-=lh;
    drawText2D(cx-220,y0,"Key [gold]    : collect to unlock EXIT");  y0-=lh;
    drawText2D(cx-220,y0,"Exit [cyan]   : portal bawah-kanan maze");y0-=lh;
    drawText2D(cx-220,y0,"Ghost [red]   : avoid! red border=danger");y0-=lh;
    y0-=8;
    glColor3f(0.9f,0.8f,1.0f); drawText2D(cx-220,y0,"--- MINIMAP ---"); y0-=lh;
    glColor3f(1.0f,0.9f,0.1f); drawText2D(cx-220,y0,"Y");
    glColor3f(0.75f,0.75f,0.75f); drawText2D(cx-200,y0,"= Kamu");   y0-=lh;
    glColor3f(0.2f,1.0f,0.2f); drawText2D(cx-220,y0,"C");
    glColor3f(0.75f,0.75f,0.75f); drawText2D(cx-200,y0,"= Coin");   y0-=lh;
    glColor3f(1.0f,0.8f,0.0f); drawText2D(cx-220,y0,"K");
    glColor3f(0.75f,0.75f,0.75f); drawText2D(cx-200,y0,"= Kunci");  y0-=lh;
    glColor3f(1.0f,0.0f,0.0f); drawText2D(cx-220,y0,"G");
    glColor3f(0.75f,0.75f,0.75f); drawText2D(cx-200,y0,"= Ghost");  y0-=lh;
    glColor3f(0.0f,1.0f,0.9f); drawText2D(cx-220,y0,"E");
    glColor3f(0.75f,0.75f,0.75f); drawText2D(cx-200,y0,"= Exit");   y0-=lh;
    y0-=8;
    glColor3f(0.9f,0.8f,1.0f); drawText2D(cx-220,y0,"--- MODE ---"); y0-=lh;
    glColor3f(0.75f,0.75f,0.75f);
    drawText2D(cx-220,y0,"F1=Game  F2=Env Demo  F3=Entity Demo");   y0-=lh;
    drawText2D(cx-220,y0,"TAB=Settings  R=Restart  ESC=Quit");
    glColor3f(0.5f,0.5f,0.5f);
    drawText2D(cx-130, cy-bh*0.5f+20, "Press [TAB] to close");
    end2D();
}
