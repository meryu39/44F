#include "ingame.h"
#include "screen.h"
#define MAX_ENEMIES 5

typedef struct Node {
    int x;
    int y;
    int type;
} node; //정점의 위치와 타입를 담은 자료형

char FPSTextInfo[1024];
clock_t CurTime, OldTime;

int money = 5500;
int life = 5;

bool isPlaying = true;
bool wavetime = false;
int wave = 0;

int selectedTurret = 0;
bool turretSelected = false;

int map[HEIGHT][WIDTH];
int map_copy[HEIGHT][WIDTH];
int vertex[HEIGHT][WIDTH]; //정점만 담은 이차원 배열

int player_x = 19;
int player_y = 19;



int startIdx = -1, endIdx = -1;

node arr[MAX];
int graph[MAX][MAX];

int costList[MAX];
int parentList[MAX];
int visitList[MAX];

int direction[4][2] = { {1, 0},{0, 1}, {-1, 0}, {0, -1} };

typedef struct Enemy {
    int x;
    int y;
    int hp;
    bool islife;
} enemy;

typedef struct turret {
    int x;
    int y;
    int attackPower;
    int attackRange;
    int attackCycle;
}Turret;


enemy enemies[MAX_ENEMIES];
int numEnemies = 0;

Turret turreties[MAX];
int numTurreties = 0;

int distance;
int range;

void spawnEnemy(int x, int y, int hp) {
    enemies[numEnemies].x = x;
    enemies[numEnemies].y = y;
    enemies[numEnemies].hp = hp;
    enemies[numEnemies].islife = true;
    numEnemies++;

}
void spawnTurret(int x, int y, int attackPower, int attackRange) {
    turreties[numTurreties].x = x;
    turreties[numTurreties].y = y;
    turreties[numTurreties].attackPower = attackPower;
    turreties[numTurreties].attackRange = attackRange;
    numTurreties++;

}
bool isMove(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        // 이동 가능한 경우
        return map[y][x] == 0;
    }
    // 범위를 벗어나거나 이동 불가능한 경우
    return false;
}

void InitGame() {
    life = 5;
    money = 10000;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            map[y][x] = 0;
            map_copy[y][x] = map[y][x];
        }
    }
}
void collider() {

    static DWORD previousCollisionTime = 0;
    DWORD currentTime = GetTickCount64();

    if (currentTime - previousCollisionTime >= 1000) {
        for (int i = 0; i < numEnemies; i++) {
            int enemyX = enemies[i].x;
            int enemyY = enemies[i].y;

            for (int j = 0; j < numTurreties; j++) {
                range = turreties[j].attackRange;
                distance = abs(turreties[j].x - enemyX) + abs(turreties[j].y - enemyY);

                if (distance <= range) {
                    enemies[i].hp -= turreties[j].attackPower;

                    if (enemies[i].hp <= 0) {
                        enemies[i].islife = false;
                        //ScreenPrint(90, 50, "%d번째 죽음 %d",i, enemies[i].islife);

                    }
                }
            }
        }

        previousCollisionTime = currentTime;
    }
}
void moveEnemies() {

        static DWORD previousTime = 0;
        DWORD currentTime = GetTickCount64();

        if (currentTime - previousTime >= 500) {
            for (int i = 0; i < numEnemies; i++) {
                int currentX = enemies[i].x;
                int currentY = enemies[i].y;

                for (int j = 0; j < 4; j++) {
                    int dirX = direction[j][X];
                    int dirY = direction[j][Y];

                    int nextX = currentX + dirX;
                    int nextY = currentY + dirY;

                    if (isMove(currentX + dirX, currentY + dirY) &&
                        map_copy[currentY + dirY][currentX + dirX] == PATH) {

                        enemies[i].x = currentX + dirX;
                        enemies[i].y = currentY + dirY;


                        map_copy[currentY][currentX] = PATH;
                        map_copy[currentY + dirY][currentX + dirX] = 4;



                        break;
                    }
                }
            }
            previousTime = currentTime;
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
            }
        }

        switch (selectedTurret) {
        case 0:
            ScreenPrint(57, 7, "  AR  ");
            map[player_y][player_x] = 4;
            spawnTurret(player_x, player_y, 3, 3);
            
            break;
        case 1:
            ScreenPrint(57, 7, "  SMG ");
            map[player_y][player_x] = 5;
            spawnTurret(player_x, player_y, 1, 2);
            break;
        case 2:
            ScreenPrint(57, 7, "  SR  ");
            map[player_y][player_x] = 6;
            spawnTurret(player_x, player_y, 5, 5);
            break;
        case 3:
            ScreenPrint(57, 7, "  SG  ");
            map[player_y][player_x] = 7;
            spawnTurret(player_x, player_y, 100, 1);
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
            if (player_y == 0 && player_x == 0 || player_y == HEIGHT - 1 && player_x == WIDTH - 1)
            {
                break;
            }
            if (money >= 100 && map[player_y][player_x] == EMPTY) {
                map[player_y][player_x] = WALL;
                money -= 100;
            }
            else if (map[player_y][player_x] == WALL) {
                map[player_y][player_x] = EMPTY;
                money -= 50;
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
            wavetime = true;
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


void UI() {

    ScreenPrint(4, 28, "보유 포탑");
    for (int i = 0; i < 20; i++) {
        ScreenPrint(50, 5 + i, "|");
    }
    ScreenPrint(70, 5, "%d wave", wave);

    ScreenPrint(68, 8, "웨이브 단계");
    //ScreenPrint(68, 8, "설치 단계  "); 단계에 따라 둘이 출력 번갈아가면서

    ScreenPrint(55, 12, "보유 골드: %4d", money);


    ScreenPrint(55, 23, "포탑 강화 ");
    ScreenPrint(60, 25, "1. AR");
    ScreenPrint(70, 25, "2. SMG");
    ScreenPrint(80, 25, "3. SR");
    ScreenPrint(90, 25, "4. SG   ");
    // 해당 포탑 강화시 밑의 N이 올라감
    ScreenPrint(60, 27, "레벨 %d"); //AR
    ScreenPrint(70, 27, "레벨 %d"); //SMG
    ScreenPrint(80, 27, "레벨 %d"); // SR
    ScreenPrint(90, 27, "레벨 %d"); //SG


    //
   /* ScreenPrint(45, 9, "웨이브 %d", wave);

    ScreenPrint(45, 3, "현재 골드 %d", money);

    if (!wavetime) {
        ScreenPrint(45, 8, "포탑을 선택하려면 'e' 키를 누르고, 화살표 키로 종류를 선택하세요.");
    }

    ScreenPrint(45, 6, "AR  SMG  SR  SG");
    ScreenPrint(45, 7, "선택한 포탑: ");*/

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
                else if (map_copy[y][x] == EMPTY) {
                    sprintf(FPSTextInfo + index, "□ ");
                }
                else if (map_copy[y][x] == WALL) {
                    sprintf(FPSTextInfo + index, "※ ");
                }
                else if (map_copy[y][x] == PATH) {
                    sprintf(FPSTextInfo + index, "● ");
                }
                else if (map_copy[y][x] == 3) {
                    sprintf(FPSTextInfo + index, "M ");
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
                for (int i = 0; i < numEnemies; i++) {
                    if (enemies[i].islife) {
                        if (y == enemies[i].y && x == enemies[i].x) {
                            sprintf(FPSTextInfo + index, "M ");
                        }
                    }
                }
                index += 2;
            }

            sprintf(FPSTextInfo + index, "\n");
            index++;
        }

        OldTime = CurTime;
    }

    ScreenPrint(0, 5, FPSTextInfo);
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
            if (map[y][x] != EMPTY) {
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

void InstallStep() {

    monsterPath();
    Render();
    move();
}
void spawnEnemyWave(int wave) {
    static DWORD previousSpawnTime = 0;
    DWORD currentTime = GetTickCount64();

    if (currentTime - previousSpawnTime >= 1000) {
        if (numEnemies <= 10) {  

            int spawnX = 0;
            int spawnY = 0;
            int enemyHP = 100 + wave * 100;
            


            spawnEnemy(spawnX, spawnY, enemyHP);

            // Update the spawn time
            previousSpawnTime = currentTime;
        }
    }
}
void WaveStep() {
    

}


void GameControl() {
    InitGame();
    ScreenInit();
    while(isPlaying){
        CurTime = clock();
        move();
        UI();
        monsterPath();

        if (!wavetime) {
            InstallStep();
        }
        else {
            //if(checkEnemy() == 0) wavetime = false;
            
            spawnEnemyWave(wave);
            
            collider();
            Render();
            moveEnemies();
            

        }
    }
}

int main() {
    CurTime = OldTime = clock();
    memset(FPSTextInfo, '\0', 1024);

    ScreenInit();
    int p = 0;
    
    while (1) {
        p = MainSelected();
        switch (p)
        {
        case 1:
            ScreenRelease();

            GameControl();

        case 2:
            //instruction();
            break;
        case 3:
            //Ranking();
            break;
        case 4:
            ScreenRelease();
            return 0;
            break;
        default:
            break;
        }
        p = NULL;
    }
    ScreenRelease();
    return 0;
}