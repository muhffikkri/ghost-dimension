// Nama File : Entity.h
// Deskripsi : Deklarasi entitas game dan fungsi logika interaksi utama.
// Tanggal Dibuat : 24 Maret 2026

// Logic & Interaction

#ifndef ENTITY_H
#define ENTITY_H

#include "shared/Ghost.h"
#include "shared/Coin.h"
#include "shared/Key.h"

struct Entity {
    float x, y, z;
    bool isActive;
};

// Inisialisasi data entity (coin/key/ghost)
void initEntities();

// Sinkronkan posisi pemain untuk interaksi item/ghost
void syncPlayerPosition(float x, float z);

// Cek collision pemain vs dinding
bool checkCollision(float nextX, float nextZ);

// TODO: Implementasi pergerakan Hantu mengejar pemain (Translasi) 
void updateGhost(float playerX, float playerZ);

// TODO: Implementasi Rotasi Koin dan deteksi tabrakan 
void updateItems();

// TODO: Logika pengecekan Win/Lose Condition 
void checkGameStatus();

#endif