# Ghost Dimension - GTI 2026

**Ghost Dimension** adalah game _first-person survival horror_ berbasis **OpenGL** dan **GLUT**. Pemain berada di dalam labirin gelap, harus mengumpulkan koin, menemukan kunci, dan mencapai pintu keluar sambil menghindari hantu yang mulai aktif setelah pemain menjauh dari titik awal. Proyek ini dibuat untuk tugas besar praktikum Grafika dan Teknik Interaktif (GTI) Informatika 2025/2026.

## Deskripsi Game

Ghost Dimension menempatkan pemain dalam suasana labirin kuno yang sunyi, gelap, dan penuh ancaman. Pemain hanya dibekali sudut pandang _first-person_, sehingga orientasi ruang menjadi tantangan utama. Di sepanjang maze terdapat koin untuk skor, kunci untuk membuka akses keluar, serta sumber cahaya terbatas berupa senter pemain dan obor di beberapa titik dinding.

Ancaman utama datang dari hantu yang akan aktif setelah pemain melewati jarak tertentu dari titik spawn. Setelah aktif, hantu terus mengejar pemain hingga permainan selesai. Kombinasi navigasi, pengambilan item, dan manajemen jarak dari musuh menjadi inti gameplay.

## Mekanisme Game

### Kondisi Awal

- Pemain memulai permainan di titik spawn yang aman.
- Kamera berada pada posisi _eye level_ agar sudut pandang terasa imersif.
- Senter aktif sejak awal untuk membantu navigasi di area gelap.

### Eksplorasi dan Interaksi

- Pemain bergerak menggunakan kontrol keyboard dalam mode _first-person_.
- Koin tersebar di sepanjang lorong maze dan dapat diambil untuk menambah skor.
- Kunci harus ditemukan sebelum pemain dapat menggunakan pintu keluar.
- Objek item dan lingkungan dibuat untuk mendukung eksplorasi dengan pencahayaan yang terbatas.
- Deteksi tabrakan mencegah pemain menembus dinding.

### Entitas Musuh

- Hantu tidak aktif sejak awal permainan.
- Setelah pemain bergerak melewati jarak tertentu dari titik awal, hantu mulai aktif.
- Setelah aktif, hantu bergerak mengejar pemain secara terus-menerus.
- Mekanisme ini menjaga ketegangan sepanjang permainan dan memaksa pemain bergerak efisien.

### Kondisi Menang

- Pemain memenangkan permainan jika berhasil mengambil kunci dan mencapai pintu keluar.

### Kondisi Kalah

- Pemain kalah jika tertangkap oleh hantu.
- Pemain juga kalah jika durasi permainan habis.

## Teknik Grafika dan Interaktif yang Digunakan

Ghost Dimension mengimplementasikan beberapa teknik grafika komputer dan interaksi dasar yang relevan dengan tugas:

- **Primitif Drawing**: digunakan untuk membangun dinding, lantai, coin, key, ghost, dan elemen UI sederhana.
- **Translasi dan Rotasi**: dipakai untuk pergerakan kamera, animasi koin, animasi key, pergerakan hantu, dan orientasi objek di dunia 3D.
- **Animasi**: coin dibuat berputar dan mengambang, torch dibuat flicker, dan hantu bergerak mengejar pemain dan head bobbing pada pemain.
- **Proyeksi Perspektif dan Ortografis**: perspektif dipakai untuk dunia 3D, sedangkan ortografis dipakai untuk HUD dan minimap.
- **Kamera First-Person**: menggunakan `gluLookAt` untuk memberi pengalaman pandang mata pemain.
- **Depth Testing**: menjaga urutan render objek agar tampak realistis.
- **Lighting**: menggunakan spotlight untuk senter pemain dan point light / obor untuk pencahayaan lingkungan.
- **Texture Mapping**: diterapkan pada dinding dan lantai agar lingkungan terlihat lebih hidup.
- **Collision Detection**: digunakan untuk membatasi pergerakan pemain terhadap dinding dan untuk deteksi pengambilan item.
- **Trigger-Based Gameplay**: hantu aktif berdasarkan jarak pemain dari titik awal.

## Struktur Proyek

Struktur folder proyek disusun sebagai berikut:

- `include/`: file header untuk Camera, Config, Entity, Environment, Coin, Key, dan Ghost.
- `src/`: source code utama proyek.
- `src/entity/`: implementasi entitas seperti coin, key, dan ghost.
- `src/environment/`: implementasi environment, pencahayaan, minimap, dan elemen pendukung dunia.
- `build/`: file hasil build sementara.
- `obj/`: file objek hasil kompilasi.
- `README.md`: dokumentasi proyek.

## Rincian Peran Modul

- `main.cpp`: titik masuk program dan pengendali loop utama.
- `Camera.cpp`: mengatur kamera first-person dan HUD.
- `Entity.cpp`: mengelola inisialisasi item, status permainan, pickup, dan kondisi menang/kalah.
- `Environment.cpp`: mengatur dinding, lantai, obor, dan elemen visual lingkungan.
- `Coin.cpp`: menggambar dan mengatur perilaku koin.
- `Key.cpp`: menggambar dan mengatur perilaku kunci.
- `Ghost.cpp`: menggambar dan mengatur perilaku hantu.
- `Config.cpp`: konfigurasi dasar dan data maze.

## Output yang Diharapkan

Sesuai kebutuhan pengumpulan tugas, hasil akhir proyek mencakup:

1. Source code lengkap yang diunggah ke repositori tim.
2. Video demonstrasi permainan.
3. Laporan resmi yang menjelaskan konsep, implementasi, dan hasil pengujian.
