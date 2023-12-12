#include "ingame.h"
#include "screen.h"

typedef struct Node {
    int x;
    int y;
    int type;
} node; //정점의 위치와 타입를 담은 자료형

enum NodeType {
    START = 1, VERTEX = 2, END = 9
};
typedef struct {
    int key;
} element;

char FPSTextInfo[128];
clock_t CurTime, OldTime;

int money = 500;
int HP = 100;

bool wavetime = false;
int wave = 0;

int selectedTurret = 0;
bool turretSelected = false;

int map[WIDTH][HEIGHT] = {
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
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
};
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
                ScreenPrint(45, 9, "바리게이트가 설치되지 않았습니다");
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

void Dijkstra(int startNodeIdx, int goalNodeIdx) {

    // 초기화
    for (int i = 0; i < MAX; ++i) {
        costList[i] = INF;
        parentList[i] = -1;
        visitList[i] = 0;
    }

    costList[startNodeIdx] = 0;

    for (int iteration = 0; iteration < MAX - 1; ++iteration) {
        int curMinimumCost = INF;
        int curNodeIdx = -1;

        // 최소 비용을 가진 노드 찾기
        for (int i = 0; i < MAX; ++i) {
            if (visitList[i]) continue;

            if (curMinimumCost > costList[i]) {
                curMinimumCost = costList[i];
                curNodeIdx = i;
            }
        }
        if (curNodeIdx == -1) break; // 모든 노드를 방문했을 때 종료
        visitList[curNodeIdx] = 1;

        // 인접 노드들에 대한 최소 비용 갱신
        for (int adjNodeIdx = 0; adjNodeIdx < MAX; ++adjNodeIdx) {
            const int weight = graph[curNodeIdx][adjNodeIdx];

            // 간선이 없으면 스킵
            if (0 == weight) continue;

            const int newCost = costList[curNodeIdx] + weight;

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
    int path[MAX][2];

    //지도 초기화
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            map_copy[y][x] = map[x][y];
            vertex[y][x] = 0;
        }
    }

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (map[y][x] == 1) {
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
    for (int i = 0; i < idx; i++) {
        for (int j = 0; j < idx; j++) {
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

            while (isMove(x + dirX * weight, y + dirY * weight)) {
                k = vertex[y + dirY * weight][x + dirX * weight];
                if (k > 0) {
                    graph[i][k] = weight;
                    graph[k][i] = weight;
                    break;
                }
                weight++;
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
    }
    PrintPath(idx);
}

void PrintPath(int idx) {

    int Dirindex[MAX][2] = { 0 };
    int nextIdx = endIdx;

    for (int i = 0; i < idx; i++) {
        if (nextIdx == -1) break;
        map_copy[arr[nextIdx].y][arr[nextIdx].x] = 2;
        nextIdx = parentList[nextIdx];
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            switch (map_copy[y][x])
            {
            case 0:
                printf("□");
                break;
            case 1:
                printf("■");
                break;
            case 2:
                printf("★");
            }
        }
        printf("\n");
    }
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