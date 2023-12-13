#include "ingame.h"
#include "screen.h"

typedef struct Node {
    int x;
    int y;
    int type;
} node; //정점의 위치와 타입를 담은 자료형


char FPSTextInfo[1024];
clock_t CurTime, OldTime;

int money = 5500;
int HP = 100;

bool wavetime = false;
int wave = 0;

int selectedTurret = 0;
bool turretSelected = false;

//int map[WIDTH][HEIGHT] = {
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
//    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
//};

int map[HEIGHT][WIDTH];
int map_copy[HEIGHT][WIDTH];
int vertex[HEIGHT][WIDTH]; //정점만 담은 이차원 배열

int player_x = 0;
int player_y = 0;

int monster_x = 0;
int monster_y = 0;

int startIdx = -1, endIdx = -1;

node arr[MAX];
int graph[MAX][MAX];

int costList[MAX];
int parentList[MAX];
int visitList[MAX];

int direction[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

bool isInstalStep = true;
bool isPlaying = true;

bool isMove(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        // 이동 가능한 경우
        return map[y][x] == 0;
    }
    // 범위를 벗어나거나 이동 불가능한 경우
    return false;
}

void InitMap() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            map[y][x] = 0;
            map_copy[y][x] = map[y][x];
        }
    }
}

void selectTurret() {
    while (!turretSelected) {
        int key = _getch();
        if (key == SPACEBAR) { // Spacebar
            turretSelected = true;
        }
        else if (key == ARROWKEY) { // Arrow keys
            key = _getch();
            switch (key) {
            case LEFT: // Left
                if (selectedTurret > 0) {
                    selectedTurret--;
                }
                break;
            case RIGHT: // Right
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

void move() {
    if (_kbhit()) {
        int key = _getch();
        switch (key) {
        case SPACEBAR: // Spacebar
            if (money >= 100&&map[player_y][player_x] == EMPTY) {
                map[player_y][player_x] = WALL;
                money -= 100;
            }
            break;
        case E: // 'e' key
            if (map[player_y][player_x] == WALL) {
                selectTurret();
                break;
            }
            else {
                ScreenPrint(45, 9, "바리게이트가 설치되지 않았습니다");
                ScreenFlipping();
            }
            break;
        case F:
            isInstalStep = false;
            break;
        case ARROWKEY: // Arrow keys
            key = _getch();
            switch (key) {
            case UP: // Up
                if (player_y != 0) {
                    player_y--;
                }
                break;
            case LEFT: // Left
                if (player_x != 0) {
                    player_x--;
                }
                break;
            case RIGHT: // Right
                if (player_x != (WIDTH - 1)) {
                    player_x++;
                }
                break;
            case DOWN: // Down
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

void UI() {
    ScreenPrint(45, 1, "WAVE %d", wave);
    ScreenPrint(45, 3, "현재 골드 %d", money);

    if (!wavetime) {
        ScreenPrint(45, 8, "포탑을 선택하려면 'e' 키를 누르고, 화살표 키로 종류를 선택하세요.");
    }

    ScreenPrint(45, 6, "AR  SMG  SR  SG");
    ScreenPrint(45, 7, "선택한 포탑: ");
}

void Render() {
    ScreenClear();

    if (CurTime - OldTime >= 100) {
        int index = 0;

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (y == player_y && x == player_x) {

                    sprintf(FPSTextInfo + index, "★ ");
                }
                else if (y == monster_y && x == monster_x) {
                    sprintf(FPSTextInfo + index, "M ");
                }
                else if (map_copy[y][x] == EMPTY) {
                    sprintf(FPSTextInfo + index, "□ ");
                }
                else if (map_copy[y][x] == WALL) {
                    sprintf(FPSTextInfo + index, "※ ");
                }
                else if (map_copy[y][x] == PATH) {
                    sprintf(FPSTextInfo + index, "● ");
                }
                else if (map_copy[y][x] == 4) {
                    sprintf(FPSTextInfo + index, "4 ");
                }
                else if (map_copy[y][x] == 5) {
                    sprintf(FPSTextInfo + index, "5 ");
                }
                else if (map_copy[y][x] == 6) {
                    sprintf(FPSTextInfo + index, "6 ");
                }
                else if (map_copy[y][x] == 7) {
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

void Dijkstra(int startNodeIdx, int goalNodeIdx) {

    for (int i = 0; i < MAX; ++i) {
        costList[i] = graph[startNodeIdx][i];
        parentList[i] = -1;
        visitList[i] = 0;
    }

    costList[startNodeIdx] = 0;

    for (int iteration = 0; iteration < MAX; iteration++) {
        int curMinimumCost = INF;
        int curNodeIdx = -1;

        // 최소 비용을 가진 노드 찾기
        for (int i = 0; i < MAX; i++) {
            if (visitList[i]) continue;

            if (curMinimumCost > costList[i]&&!visitList[i]) {
                curMinimumCost = costList[i];
                curNodeIdx = i;
            }
        }
        if (curNodeIdx == -1) break; // 모든 노드를 방문했을 때 종료
        visitList[curNodeIdx] = 1;

        // 인접 노드들에 대한 최소 비용 갱신
        for (int adjNodeIdx = 0; adjNodeIdx < MAX; adjNodeIdx++) {

            int weight = graph[curNodeIdx][adjNodeIdx];

            // 간선이 없으면 스킵
            if (weight == 0) continue;
            
            int newCost = costList[curNodeIdx] + weight;

            if (costList[adjNodeIdx] > newCost) {
                costList[adjNodeIdx] = newCost;
                parentList[adjNodeIdx] = curNodeIdx;
            }
        }
    }
}

void monsterPath() {
    int y, x;
    int dirX, dirY;
    int bothsides = 0;
    int updown = 0;

    int i, j, k;

    int idx = 0;
    int weight = 0;

    //지도 초기화
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            map_copy[y][x] = map[y][x];
            vertex[y][x] = -1;
        }
    }

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (map[y][x] == WALL) {
                continue;
            }
            if (x == 0 && y == 0) {
                arr[idx].x = x;
                arr[idx].y = y;
                arr[idx].type = START;
                vertex[y][x] = idx;
                idx++;
            }
            else if (x == WIDTH - 1 && y == HEIGHT - 1) {
                arr[idx].x = x;
                arr[idx].y = y;
                arr[idx].type = END;
                vertex[y][x] = idx;
                idx++;
            }
            else {
                arr[idx].x = x;
                arr[idx].y = y;
                arr[idx].type = VERTEX;
                vertex[y][x] = idx;
                idx++;
            }
        }
    }

    // 그래프 초기화
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            graph[i][j] = INF;
        }
    }

    for (int i = 0; i < idx; i++) {
        x = arr[i].x;
        y = arr[i].y;
        for (int j = 0; j < 4; j++) {
            weight = 1;
            dirX = direction[j][X];
            dirY = direction[j][Y];

            while (isMove(x + dirX, y + dirY)) {
                k = vertex[y + dirY][x + dirX];
                if(k != -1)
                {
                    graph[i][k] = 1;
                    graph[k][i] = 1;
                    break;
                }
            }
        }
    }
    // 시작점과 목적지의 인덱스 찾기

    for (int i = 0; i < idx; i++) {
        if (arr[i].type == START) {
            startIdx = i;
        }
        else if (arr[i].type == END) {
            endIdx = i;
        }
    }

    // 다익스트라 알고리즘 적용
    if (startIdx != -1 && endIdx != -1) {
        Dijkstra(startIdx, endIdx);
    }
    else {
        printf("시작점 또는 목적지를 찾을 수 없습니다.\n");
        exit(0);
    }
    PrintPath(endIdx);
}

void PrintPath(int idx) {

    //int Dirindex[MAX][2] = { 0 };  이건 나중에 쓸듯
    int nextIdx = endIdx;
    int k = 0;

    while(nextIdx != -1) {
        map_copy[arr[nextIdx].y][arr[nextIdx].x] = PATH;
        nextIdx = parentList[nextIdx];
    }
    /*for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            switch (map_copy[y][x])
            {
            case EMPTY:
                printf("□");
                break;
            case WALL:
                printf("■");
                break;
            case PATH:
                printf("★");
            }
        }
        printf("\n");
    }*/
}

void InstalStep() {
    CurTime = clock();
    monsterPath();
    Render();
    UI();
    move();
}

void WaveStep() {

}

void GameControl() {
    if (isInstalStep) {
        InstalStep();
    }
    else {
        isPlaying = false;
    }
}

int main() {
    CurTime = OldTime = clock();
    memset(FPSTextInfo, '\0', 1024);
    ScreenInit();
    InitMap();
    printf("게임시작");
    _getch();

    while (isPlaying) {
        //GameControl();
        CurTime = clock();
        monsterPath();
        Render();
        UI();
        move();
    }
    ScreenRelease();
    return 0;
}