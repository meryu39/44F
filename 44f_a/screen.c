#define _CRT_SECURE_NO_WARNINGS
#include "screen.h"
#include "ingame.h"

#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>

#define EMPTY 0 //타일의 정보
#define WALL 1

#define INF 9999 //이어지지 않은 정점은 무한대로

#define X 0
#define Y 1

#define WIDTH 20
#define HEIGHT 20
#define MAX HEIGHT*WIDTH

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

//여기서부터 알고리즘 관련 변수 선언

int map[HEIGHT][WIDTH];
int map_vertex[HEIGHT][WIDTH]; //정점만 담은 이차원 배열

typedef struct Node {
    int x;
    int y;
    int type;
} node; //정점의 위치와 타입를 담은 자료형

enum NodeType {
    start = 1, vertex = 2, end = 9
};

int found[MAX];     //다익스트라 찾은 정점 
int distance[MAX];  //선택한 정점에서부터 다른 정점까지의 거리

int graph[MAX][MAX];//현재 맵을 그래프화 시킨것

int direction[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

//이동가능한지 판단하는 함수
bool IsMove(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        // 이동 가능한 경우
        return map[y][x] == 0; 
    }
    // 범위를 벗어나거나 이동 불가능한 경우
    return false;
}

void PrintDijkstra(int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", found[i]);
    }
    printf("\n");
}

//그냥 다익스트라
void ShortestPath(int n, int start) {
    int i, j;
    int vertex = start;
    int min;

    for (i = 0; i < n; i++) {
        distance[i] = graph[start][i];
        found[i] = 0;
    }

    found[vertex] = true;
    printDijkstra(n);

    for (i = 0; i < n; i++) {

        min = INF;
        for (j = 0; j < n; j++) {
            if (min > distance[j] && distance[j] != 0) {
                min = distance[j];
                vertex = j;
            }
        }
        distance[vertex] = 0;
        found[vertex] = true;

        for (j = 0; j < n; j++) {
            if (distance[j] > graph[vertex][j] + min) {
                distance[j] = graph[vertex][j] + min;
            }
        }
        printDijkstra(n);
    }
}

void MonsterPath(){
    int y, x;
    int dirX, dirY;

    int bothsides = 0;
    int updown = 0;

    int i, j, k; //반복문에 쓰는 배열임다

    int idx = 0;
    int weight = 0;

    //지도 초기화
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            map[y][x] = 0;
            map_vertex[y][x] = 0;
        }
    }

    //스테이지 전부 돌면서 정점을 선택하는 반복문
    //1. 시작지점
    //2. 끝지점
    //3. 일자(좌우만 혹은 상하만 뚫려 있는)타일
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (map[y][x] == 1) {
                continue;
            }
            if (x == 0 && y == 0) {
                arr[idx].x = x;
                arr[idx].y = y;
                arr[idx].type = start;
                map_vertex[y][x] = idx;
                idx++;
            }
            else if (x == WIDTH - 1 && y == HEIGHT - 1) {
                arr[idx].x = x;
                arr[idx].y = y;
                arr[idx].type = end;
                map_vertex[y][x] = idx;
                idx++;
            }
            else {
                updown = bothsides = 0;
                for (i = 0; i < 4; ++i) {
                    dirX = direction[i][X];
                    dirY = direction[i][Y];
                    if (isMove(x + dirX, y + dirY)) {
                        i > 1 ? updown += 1 : bothsides += 1;
                    }
                }
                if (!(updown == 2 && bothsides == 0 || updown == 0 && bothsides == 2)) {
                    arr[idx].x = x;
                    arr[idx].y = y;
                    arr[idx].type = vertex;
                    map_vertex[y][x] = idx;
                    idx++;
                }
            }
        }
    }

    // 그래프 초기화
    for (int i = 0; i < idx; i++) {
        for (int j = 0; j < idx; j++) {
            graph[i][j] = INF;
        }
    }

    //
    for (int i = 0; i < idx; i++) {
        x = arr[i].x;
        y = arr[i].y;
        for (int j = 0; j < 4; j++) {
            weight = 1;
            dirX = direction[j][X];
            dirY = direction[j][Y];

            while (isMove(x + dirX * weight, y + dirY * weight)) {
                //printf("현재 위치: (%d, %d)\n", y + dirY * weight, x + dirX * weight);
                k = map_vertex[y + dirY * weight][x + dirX * weight];
                if (k > 0) {
                    //printf("정점 %d %d 가중치 %d \n",i, k, weight);
                    graph[i][k] = weight;
                    graph[k][i] = weight;
                    break;
                }
                weight++;
                if(weight > HEIGHT){
                    perror("오류");
                    return 0;
                }
            }
        }
    }

    for (int i = 0; i < idx; i++) {
        for (int j = 0; j < idx; j++) {
            printf("%d ", graph[i][j]);
        }
        printf("\n");
    }

    
    // 시작점과 목적지의 인덱스 찾기 
    // 나중에 목적지 인덱스 까지 도달하면 경로 찾기 종료하게 만들 예정, 따라서 지금안씀

    /*int startIdx = -1, endIdx = -1;
    for (int i = 0; i < idx; i++) {
        if (arr[i].type == start) {
            startIdx = i;
        }
        else if (arr[i].type == end) {
            endIdx = i;
        }
    }*/

    // 다익스트라 알고리즘 적용
    if (startIdx != -1 && endIdx != -1) {
        shortestPath(idx, 0);
    }
    else {
        printf("시작점 또는 목적지를 찾을 수 없습니다.\n");
    }
}

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
