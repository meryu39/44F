#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctime>
#include <iostream>

#define WIDTH 20
#define HEIGHT 20

#define MONSTER_SPEED 1000 // 몬스터 이동 간격

static int g_nScreenIndex;
static HANDLE g_hScreen[2];
int g_numofFPS;
clock_t CurTime, OldTime;

char buffer1[HEIGHT][WIDTH];
char buffer2[HEIGHT][WIDTH];
char(*front_buffer)[WIDTH] = buffer1;
char(*back_buffer)[WIDTH] = buffer2;

int player_x = 0;
int player_y = 0;

int monster_x = 0;
int monster_y = 0;

int money = 500;
int HP = 100;

bool wavetime = false;
int wave = 0;

int selectedTurret = 0;
bool turretSelected = false;

int map[HEIGHT][WIDTH] = {
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
};

void gotoxy(int x, int y) {
    COORD Pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void swap_buffers() {
    char(*temp)[WIDTH] = front_buffer;
    front_buffer = back_buffer;
    back_buffer = temp;
}

void selectTurret();

void move() {
    if (_kbhit()) {
        int key = _getch();
        switch (key) {
        case 32: // Spacebar
            if (money >= 100) {
                map[player_y][player_x] = 3;
                money -= 100;
            }
            break;
        case 101: // 'e' key
            if (map[player_y][player_x] == 3) {
                selectTurret();
                break;
            }
            else {
                gotoxy(45, 9);
                printf("바리게이트가 설치되지 않았습니다");
            }
            break;
        case 224: // Arrow keys
            key = _getch();
            switch (key) {
            case 72: // Up
                if (player_y != 0) {
                    player_y--;
                }
                break;
            case 75: // Left
                if (player_x != 0) {
                    player_x--;
                }
                break;
            case 77: // Right
                if (player_x != (WIDTH - 1)) {
                    player_x++;
                }
                break;
            case 80: // Down
                if (player_y != (HEIGHT - 1)) {
                    player_y++;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void move_monster() {
    static DWORD previousTime = 0;
    DWORD currentTime = GetTickCount64();

    if (currentTime - previousTime >= MONSTER_SPEED) {
        previousTime = currentTime;

        monster_x++;

        if (map[monster_y][monster_x + 1] != 0) {
            monster_y++;
        }
        else {
            monster_x++;
        }

        if (monster_x >= WIDTH) {
            monster_x = WIDTH - 1;
        }

        if (monster_y >= HEIGHT) {
            monster_y = HEIGHT - 1;
        }
    }
}

void selectTurret() {
    while (!turretSelected) {
        int key = _getch();
        if (key == 32) { // Spacebar
            turretSelected = true;
        }
        else if (key == 224) { // Arrow keys
            key = _getch();
            switch (key) {
            case 75: // Left
                if (selectedTurret > 0) {
                    selectedTurret--;
                }
                break;
            case 77: // Right
                if (selectedTurret < 3) {
                    selectedTurret++;
                }
                break;
            default:
                break;
            }
        }

        gotoxy(45, 7);
        printf("선택한 포탑: ");
        switch (selectedTurret) {
        case 0:
            printf(" AR ");
            map[player_y][player_x] = 4;
            break;
        case 1:
            printf("SMG");
            map[player_y][player_x] = 5;
            break;
        case 2:
            printf(" SR ");
            map[player_y][player_x] = 6;
            break;
        case 3:
            printf(" SG ");
            map[player_y][player_x] = 7;
            break;
        default:
            break;
        }
    }
    turretSelected = false;
}

void ScreenInit() {
    CONSOLE_CURSOR_INFO cci;
    g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    cci.dwSize = 1;
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(g_hScreen[0], &cci);
    SetConsoleCursorInfo(g_hScreen[1], &cci);
}

void ScreenFlipping() {
    SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);
    g_nScreenIndex = !g_nScreenIndex;
}

void ScreenClear() {
    COORD Coor = { 0, 0 };
    DWORD dw;
    FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', WIDTH * HEIGHT, Coor, &dw);
}

void ScreenRelease() {
    CloseHandle(g_hScreen[0]);
    CloseHandle(g_hScreen[1]);
}

void ScreenPrint(int x, int y, const char* string) {
    DWORD dw;
    COORD CursorPosition = { x, y };
    SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], CursorPosition);
    WriteFile(g_hScreen[g_nScreenIndex], string, strlen(string), &dw, NULL);
}

void Render() {
    ScreenClear();

    if (CurTime - OldTime >= 1000) {
        for (int i = 0; i < WIDTH; i++) {
            for (int j = 0; j < HEIGHT; j++) {
                gotoxy(i * 2, j);
                if (j == player_y && i == player_x) {
                    printf("★");
                }
                else if (j == monster_y && i == monster_x) {
                    printf("M ");
                }
                else if (map[j][i] == 0) {
                    printf("□");
                }
                else if (map[j][i] == 3) {
                    printf("※");
                }
                else if (map[j][i] == 4) {
                    printf("4 ");
                }
                else if (map[j][i] == 5) {
                    printf("5");
                }
                else if (map[j][i] == 6) {
                    printf("6 ");
                }
                else if (map[j][i] == 7) {
                    printf("7 ");
                }
            }
        }
        OldTime = CurTime;
    }

    g_numofFPS++;

    ScreenFlipping();
}

void UI() {
    gotoxy(45, 1);
    printf("WAVE %d", wave);

    gotoxy(45, 3);
    printf("현재 골드 %d", money);

    gotoxy(45, 5);
    if (!wavetime) {
        gotoxy(45, 8);
        printf("포탑을 선택하려면 'e' 키를 누르고, 화살표 키로 종류를 선택하세요.");
    }
    gotoxy(45, 6);
    printf("AR  SMG  SR  SG");
    gotoxy(45, 7);
}

int main() {
    g_numofFPS = 0;
    CurTime = OldTime = clock();
    ScreenInit();

    while (1) {
        CurTime = clock();
        Render();
        UI();
        move();
        move_monster();
    }

    ScreenRelease();
    return 0;
}
