#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#define INF 999
#define N 6

const char *kota[N] = {
    "Jakarta", "Bandung", "Surabaya",
    "Semarang", "Medan", "Makassar"
};

int graph[N][N] = {
    {0,   20,  80,  40,  60, 100},
    {20,  0,   90,  30, INF, INF},
    {80,  90,  0,   40, INF, 30 },
    {40,  30,  40,  0,  INF, 30 },
    {60, INF, INF, INF, 0,  INF},
    {100, INF, 30, 70, 120, 0}
};

int dist[N][N];
int parent[N][N];

// ------------------------ CLEAR SCREEN ------------------------
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    printf("\x1b[2J\x1b[H");
    fflush(stdout);
#endif
}

// cross-platform millisecond sleep
void sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

// simple loading animation for given seconds
void animate_loading(int seconds) {
    const char *spin = "|/-\\";
    int len = 4;
    int interval = 100; // ms
    int steps = (seconds * 1000) / interval;
    for (int i = 0; i < steps; ++i) {
        printf("\rMenghitung rute tercepat... %c", spin[i % len]);
        fflush(stdout);
        sleep_ms(interval);
    }
    printf("\rMenghitung rute tercepat... selesai.   \n");
}




void printInitialMatrix() {

    int max_name = 0;
    int max_num_width = 0;
    for (int i = 0; i < N; ++i) {
        int ln = (int)strlen(kota[i]);
        if (ln > max_name) max_name = ln;
        // check numeric widths
        for (int j = 0; j < N; ++j) {
            if (graph[i][j] < INF) {
                char buf[32];
                snprintf(buf, sizeof(buf), "%d", graph[i][j]);
                int l = (int)strlen(buf);
                if (l > max_num_width) max_num_width = l;
            }
        }
    }
    if (max_num_width < 3) max_num_width = 3;
    int colw = (max_name > max_num_width) ? max_name + 2 : max_num_width + 4;
    if (colw < 7) colw = 7;

    int labelw = max_name + 2;

    // Header: city names (no numeric indices)
    printf("%*s", labelw, "");
    for (int j = 0; j < N; ++j) {
        char shortname[128];
        if ((int)strlen(kota[j]) > colw-1) {
            strncpy(shortname, kota[j], colw-4);
            shortname[colw-4] = '\0';
            strcat(shortname, "..");
        } else strcpy(shortname, kota[j]);
        int pad = (colw - (int)strlen(shortname)) / 2;
        printf("%*s%*s", pad, "", (int)strlen(shortname), shortname);
        if (pad + (int)strlen(shortname) < colw) printf("%*s", colw - pad - (int)strlen(shortname), "");
    }
    printf("\n");

    // Separator
    for (int i = 0; i < labelw + colw * N; ++i) putchar('-');
    printf("\n");

    // Rows (show only city name on the left, no numeric index)
    for (int i = 0; i < N; ++i) {
        printf("%-*s|", max_name + 1, kota[i]);
        for (int j = 0; j < N; ++j) {
            if (graph[i][j] >= INF)
                printf("%*s", colw, "INF");
            else
                printf("%*d", colw, graph[i][j]);
        }
        printf("\n");
    }
}

// ------------------------ PRINT PATH ------------------------
void printPath(int i, int j) {
    if (i == j) {
        printf("%s", kota[i]);
        return;
    }
    if (parent[i][j] == -1) {
        printf("Tidak ada jalur");
        return;
    }
    printPath(i, parent[i][j]);
    printf(" -> %s", kota[j]);
}

// ------------------------ FLOYD WARSHALL ------------------------
void floydWarshall() {
    // Inisialisasi
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            dist[i][j] = graph[i][j];
            if (i == j || graph[i][j] == INF)
                parent[i][j] = -1;
            else
                parent[i][j] = i;
        }
    }

    // Algoritma Floyd–Warshall
    for (int k = 0; k < N; k++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {

                    dist[i][j] = dist[i][k] + dist[k][j];
                    parent[i][j] = parent[k][j];
                }
            }
        }
    }
}

// ------------------------ MENU ------------------------
void menu() {
    int pilihan;
    int asal, tujuan;
    do {
        clear_screen();
        printf("\n==============================\n");
        printf("   APLIKASI RUTE PENGANTARAN BARANG\n");
        printf("      FLOYD-WARSHALL\n");
        printf("==============================\n");
        printInitialMatrix();
        printf("1. Cari Jalur Tercepat (menggunakan hasil Floyd-Warshall)\n");
        printf("2. Keluar\n");
        printf("Pilih menu: ");
        if (scanf("%d", &pilihan) != 1) {
            // invalid input, clear and continue
            int c; while ((c = getchar()) != '\n' && c != EOF);
            printf("Pilihan tidak valid! Tekan ENTER untuk lanjut...");
            getchar();
            continue;
        }

        // clear rest of line
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch (pilihan) {
            case 1:
                printInitialMatrix();
                printf("\nDaftar Kota:\n");
                for (int i = 0; i < N; i++)
                    printf("%d. %s\n", i+1, kota[i]);

                // ask for origin/destination with validation
                while (1) {
                    printf("Masukkan kota asal (1-%d): ", N);
                    if (scanf("%d", &asal) != 1) { int ch; while ((ch=getchar())!='\n' && ch!=EOF); printf("Input tidak valid.\n"); continue; }
                    if (asal < 1 || asal > N) { printf("Nomor harus antara 1 dan %d.\n", N); continue; }
                    break;
                }
                while (1) {
                    printf("Masukkan kota tujuan (1-%d): ", N);
                    if (scanf("%d", &tujuan) != 1) { int ch; while ((ch=getchar())!='\n' && ch!=EOF); printf("Input tidak valid.\n"); continue; }
                    if (tujuan < 1 || tujuan > N) { printf("Nomor harus antara 1 dan %d.\n", N); continue; }
                    break;
                }
                // convert to 0-based
                asal--; tujuan--;

                floydWarshall();
                animate_loading(5);

                if (dist[asal][tujuan] >= INF) {
                    printf("Tidak ada jalur dari %s ke %s\n", kota[asal], kota[tujuan]);
                } else {
                    printf("\nJalur terpendek dari %s ke %s:\n", kota[asal], kota[tujuan]);
                    printf("Biaya: %d\n", dist[asal][tujuan]);
                    printf("Rute : ");
                    printPath(asal, tujuan);
                    printf("\n");
                }
                printf("Tekan ENTER untuk kembali ke menu...");
                // consume leftover newline after last scanf
                while ((c = getchar()) != '\n' && c != EOF);
                getchar();
                break;

            case 2:
                printf("Keluar...\n");
                break;

            default:
                printf("Pilihan tidak valid! Tekan ENTER untuk lanjut...");
                getchar();
        }

    } while (pilihan != 2);
}

// ------------------------ MAIN ------------------------
int main() {
    menu();
    return 0;
}
