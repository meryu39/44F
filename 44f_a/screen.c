#define _CRT_SECURE_NO_WARNINGS
#include "screen.h"
#include "ingame.h"

#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>


#define WIDTH 20
#define HEIGHT 20

#define MONSTER_SPEED 1000 // 몬스터 이동 간격

#define BUFFER_SIZE 1024
static char buffer[1024];
static int g_nScreenIndex;
static HANDLE g_hScreen[2];
clock_t CurTime, OldTime;
char FPSTextInfo[128];

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
                ScreenPrint(45, 9,"바리게이트가 설치되지 않았습니다");
                ScreenFlipping();
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

        switch (selectedTurret) {
        case 0:
            ScreenPrint(57, 7, "  AR  ");
            map[player_y][player_x] = 4;
            break;
        case 1:
            ScreenPrint(57, 7, "  SMG ");
            map[player_y][player_x] = 5;
            break;
        case 2:
            ScreenPrint(57, 7, "  SR  ");
            map[player_y][player_x] = 6;
            break;
        case 3:
            ScreenPrint(57, 7, "  SG  ");
            map[player_y][player_x] = 7;
            break;
        default:
            break;
        }
        ScreenFlipping();


    }
    turretSelected = false;
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

void ScreenPrint(int x, int y, const char* format, ...) {
    DWORD dw;
    COORD CursorPosition = { x, y };

    // 포맷된 문자열을 저장할 버퍼 생성
    // 필요에 따라 크기 조절

    // va_list 및 vsprintf_s를 사용하여 문자열을 포맷
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 커서 위치 설정 및 포맷된 문자열 작성
    SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], CursorPosition);
    WriteFile(g_hScreen[g_nScreenIndex], buffer, strlen(buffer), &dw, NULL);
}

void SetColor(int color) {
    SetConsoleTextAttribute(g_hScreen[g_nScreenIndex], color);
}


void Render() {
    ScreenClear();

    if (CurTime - OldTime >= 1000) {
        int index = 0;

        for (int j = 0; j < HEIGHT; j++) {
            for (int i = 0; i < WIDTH; i++) {
                if (j == player_y && i == player_x) {
                
                    sprintf(FPSTextInfo + index, "★ ");
                }
                else if (j == monster_y && i == monster_x) {
                    sprintf(FPSTextInfo + index, "M ");
                }
                else if (map[j][i] == 0) {
                    sprintf(FPSTextInfo + index, "□ ");
                }
                else if (map[j][i] == 3) {
                    sprintf(FPSTextInfo + index, "※ ");
                }
                else if (map[j][i] == 4) {
                    sprintf(FPSTextInfo + index, "4 ");
                }
                else if (map[j][i] == 5) {
                    sprintf(FPSTextInfo + index, "5 ");
                }
                else if (map[j][i] == 6) {
                    sprintf(FPSTextInfo + index, "6 ");
                }
                else if (map[j][i] == 7) {
                    sprintf(FPSTextInfo + index, "7 ");
                }

                index += 2;  
            }

            sprintf(FPSTextInfo + index, "\n");
            index++;  
        }

        OldTime = CurTime;
    }

    ScreenPrint(0, 0, FPSTextInfo);
    ScreenFlipping();
}





void UI() {
    ScreenPrint(45, 1, "WAVE %d", wave);
    ScreenPrint(45, 3, "현재 골드 %d", money);

    if (!wavetime) {
        ScreenPrint(45, 8, "포탑을 선택하려면 'e' 키를 누르고, 화살표 키로 종류를 선택하세요.");
    }
    
    ScreenPrint(45, 6, "AR  SMG  SR  SG");
    ScreenPrint(45, 7, "선택한 포탑: ");
}

int main() {
    CurTime = OldTime = clock();
    memset(FPSTextInfo, '\0', 128);
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
