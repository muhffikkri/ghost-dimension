// Nama File : Camera.cpp
// Deskripsi : Implementasi fungsi kamera dan pengaturan tampilan pemain.
// Tanggal Dibuat : 24 Maret 2026

#include "shared/Camera.h"
#include "shared/Config.h"
#include "shared/Entity.h"
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

Camera playerCam = {
    2.0f, 1.6f, 2.0f,
    0.0f, 0.0f,
    0.0f,
    false, false, false, false,
    false, false, false, false
};

void Camera::handleInput(float dt) {
    if (dt <= 0.0f) return;

    const float moveStep = CAMERA_MOVE_SPEED * dt;
    const float turnStep = CAMERA_TURN_SPEED * dt;
    const float pitchStep = CAMERA_PITCH_SPEED * dt;

    // Calculate movement direction based on input state
    float moveX = 0.0f, moveZ = 0.0f;
    const float forwardX = cosf(angle);
    const float forwardZ = sinf(angle);
    const float rightX = cosf(angle + 1.5707963f);
    const float rightZ = sinf(angle + 1.5707963f);

    if (moveW) { moveX += forwardX * moveStep; moveZ += forwardZ * moveStep; }
    if (moveS) { moveX -= forwardX * moveStep; moveZ -= forwardZ * moveStep; }
    if (moveA) { moveX -= rightX * moveStep;   moveZ -= rightZ * moveStep; }
    if (moveD) { moveX += rightX * moveStep;   moveZ += rightZ * moveStep; }

    // Apply collision detection
    if (moveX != 0.0f || moveZ != 0.0f) {
        float nextX = x + moveX;
        float nextZ = z + moveZ;
        if (!checkCollision(nextX, z)) x = nextX;
        if (!checkCollision(x, nextZ)) z = nextZ;
        bobbingTimer += dt * 3.0f;  // Bobbing when moving
    }

    // Look rotation (yaw)
    if (lookLeft)  angle += turnStep;
    if (lookRight) angle -= turnStep;

    // Pitch rotation (up/down)
    if (lookUp)   pitch = fminf(pitch + pitchStep, CAMERA_PITCH_LIMIT);
    if (lookDown) pitch = fmaxf(pitch - pitchStep, -CAMERA_PITCH_LIMIT);
}

void Camera::update(float dt) {
    // Head bobbing only when moving
    if (moveW || moveA || moveS || moveD) {
        y = 1.6f + sinf(bobbingTimer) * 0.1f;
    } else {
        y = 1.6f;
        bobbingTimer = 0.0f;
    }
}

void Camera::apply() {
    // Calculate pitch-aware look direction
    float cp = cosf(pitch);
    float sp = sinf(pitch);
    float lookX = x + sinf(angle) * cp;
    float lookY = y + sp;
    float lookZ = z + cosf(angle) * cp;

    // Setup projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;
    gluPerspective(CAMERA_FOVY, aspect, 0.1f, 150.0f);  // zFar = 150.0f

    // Setup modelview with head bobbing
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    float boboffset = sinf(bobbingTimer) * 0.05f;
    gluLookAt(x, y + boboffset, z,
              lookX, lookY + boboffset, lookZ,
              0.0f, 1.0f, 0.0f);
}

// Switch ke proyeksi ortografis untuk menampilkan HUD.
void drawHUD() {
    extern bool isGameOver, isGameWin;
    extern int score, topScore;
    extern bool hasKey;
    extern bool isGhostTriggered;

    // Save matrices
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable lighting for HUD
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    if (!isGameOver && !isGameWin) {
        // In-game HUD
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(10, 20);
        char scoreText[64];
        sprintf(scoreText, "Score: %d", score);
        for (char* c = scoreText; *c; c++)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
        
        glRasterPos2f(10, 45);
        char statusText[64];
        sprintf(statusText, "Key: %s  Ghost: %s", 
                hasKey ? "YES" : "NO", 
                isGhostTriggered ? "ACTIVE" : "IDLE");
        for (char* c = statusText; *c; c++)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    } else {
        // Game Over Pop-up
        glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glVertex2f(120, 120);
        glVertex2f(680, 120);
        glVertex2f(680, 480);
        glVertex2f(120, 480);
        glEnd();
        glDisable(GL_BLEND);
        
        // Border
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(120, 120);
        glVertex2f(680, 120);
        glVertex2f(680, 480);
        glVertex2f(120, 480);
        glEnd();
        glLineWidth(1.0f);
        
        // Title
        glColor3f(1.0f, 1.0f, 0.0f);
        glRasterPos2f(220, 180);
        if (isGameWin) {
            const char* msg = "**VICTORY**";
            for (const char* c = msg; *c; c++)
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
        } else {
            const char* msg = "**GAME OVER**";
            for (const char* c = msg; *c; c++)
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
        }
        
        // Scores
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(180, 250);
        char finalScore[64];
        sprintf(finalScore, "Final Score: %d", score);
        for (char* c = finalScore; *c; c++)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
        
        glRasterPos2f(180, 300);
        char topScoreText[64];
        sprintf(topScoreText, "Top Score: %d", topScore);
        for (char* c = topScoreText; *c; c++)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
        
        // Instructions
        glRasterPos2f(150, 380);
        const char* hint = "Press 'R' to Restart  |  ESC to Exit";
        for (const char* c = hint; *c; c++)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
    
    // Restore state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}