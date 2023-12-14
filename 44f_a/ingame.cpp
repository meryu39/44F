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
int life = 5;

int allenemy = 0;

bool isPlaying = true;
bool wavetime = false;
int wave = 0;
bool endwave = false;
int selectedTurret = 0;
bool turretSelected = false;

int map[HEIGHT][WIDTH];
int map_copy[HEIGHT][WIDTH];
int vertex[HEIGHT][WIDTH]; //정점만 담은 이차원 배열

int player_x = 19;
int player_y = 19;

int numDestroyEnemies = 0;

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
    bool Damage;
    bool del;
} enemy;

typedef struct turret {
    int x;
    int y;
    int type;
    int attackPower;
    int attackRange;
    int attackCycle;

}Turret;

typedef struct level {
    int ARLevel = 0;
    int SMGLevel = 0;
    int SGLevel = 0;
    int SRLevel = 0;
}Level;


enemy enemies[MAX_ENEMIES];
int numEnemies = 0;

Level turretLevel;

Turret turreties[MAX];
int numTurreties = 0;

int distance;
int range;

bool barricadeCaution = false;

void InitializeEnemies();
void spawnEnemy(int x, int y, int hp) {
    enemies[numEnemies].x = x;
    enemies[numEnemies].y = y;
    enemies[numEnemies].hp = hp;
    enemies[numEnemies].islife = true;
    enemies[numEnemies].Damage = false;
    enemies[numEnemies].del = false;
    numEnemies++;

}
void spawnTurret(int x, int y, int attackPower, int attackRange, int type) {
    turreties[numTurreties].x = x;
    turreties[numTurreties].y = y;
    turreties[numTurreties].type = type;
    turreties[numTurreties].attackPower = attackPower;
    turreties[numTurreties].attackRange = attackRange;
    numTurreties++;

}

void levelup(int type) {
    switch (type) {
    case AR:
        if (turretLevel.ARLevel == MAXLEVEL ||
            money < 20 * pow(1.2, turretLevel.ARLevel)) return;
        
        money -= 20 * pow(1.2, turretLevel.ARLevel);
        turretLevel.ARLevel++;
        for (int i = 0; i < numTurreties; i++) {
            if (turreties->type == AR) continue;
            turreties[i].attackPower = turretLevel.ARLevel + ARAttack;
        }
        break;
    case SMG:
        if (turretLevel.SMGLevel == MAXLEVEL ||
            money < 20 * pow(1.2, turretLevel.SMGLevel)) return;

        money -= 20 * pow(1.2, turretLevel.SMGLevel);
        turretLevel.SMGLevel++;
        for (int i = 0; i < numTurreties; i++) {
            if (turreties->type != SMG) continue;
            turreties[i].attackPower = turretLevel.SMGLevel + SMGAttack;
        }
        break;
    case SR:
        if (turretLevel.SRLevel == MAXLEVEL ||
            money < 20 * pow(1.2, turretLevel.SRLevel)) return;

        money -= 20 * pow(1.2, turretLevel.SRLevel);
        turretLevel.SRLevel++;
        for (int i = 0; i < numTurreties; i++) {
            if (turreties->type == SR) continue;
            turreties[i].attackPower = turretLevel.SRLevel + SRAttack;
        }
        break;
    case SG:
        if(turretLevel.SGLevel == MAXLEVEL ||
            money < 20 * pow(1.2, turretLevel.SGLevel)) return;

        money -= 20 * pow(1.2, turretLevel.SGLevel);
        turretLevel.SGLevel++;
        for (int i = 0; i < numTurreties; i++) {
            if (turreties->type == SG) continue;
            turreties[i].attackPower = turretLevel.SGLevel + SGAttack;
        }
        break;
    }
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

    turretLevel.ARLevel = 0;
    turretLevel.SMGLevel = 0;
    turretLevel.SGLevel = 0; 
    turretLevel.SRLevel = 0;

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

                    if (enemies[i].hp <= 0 && !enemies[i].del) {
                        enemies[i].islife = false;
                        //ScreenPrint(90, 50, "%d번째 죽음 %d",i, enemies[i].islife);
                        enemies[i].del = true;
                        numDestroyEnemies++; 
                        money = money + 50;

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
                        //ap_copy[currentY + dirY][currentX + dirX] = 4;
                        if (currentY + dirY == 19 && currentX + dirX == 19 && !enemies[i].Damage) {
                            enemies[i].Damage = true;
                            life -= 1;
                            if (life < 0) {
                                isPlaying = false;
                            }
                        }
                        break;
                    }
                }
            }
            previousTime = currentTime;
        }
    }


void selectTurret() {
    int currentattack = 0;
    while (!turretSelected) {
        int selectedTurret = rand() % 4;
        turretSelected = true;

        switch (selectedTurret) {
        case 0:
            ScreenPrint(57, 7, "  AR  ");
            ScreenFlipping();
            ScreenPrint(57, 7, "  AR  ");
            map[player_y][player_x] = AR;
            currentattack = turretLevel.ARLevel + ARAttack;
            spawnTurret(player_x, player_y, currentattack, 3, AR);
            break;
        case 1:
            ScreenPrint(57, 7, "  SMG ");
            ScreenFlipping();
            ScreenPrint(57, 7, "  SMG ");
            map[player_y][player_x] = SMG;
            currentattack = turretLevel.SMGLevel + SMGAttack;
            spawnTurret(player_x, player_y, currentattack, 2, SMG);
            break;
        case 2:
            ScreenPrint(57, 7, "  SR  ");
            ScreenFlipping();
            ScreenPrint(57, 7, "  SR  ");
            map[player_y][player_x] = SR;
            currentattack = turretLevel.SRLevel + SRAttack;
            spawnTurret(player_x, player_y, currentattack, 5, SR);
            break;
        case 3:
            ScreenPrint(57, 7, "  SG  ");
            ScreenFlipping();
            ScreenPrint(57, 7, "  SG  ");
            map[player_y][player_x] = SG;
            currentattack = turretLevel.SGLevel + SGAttack;
            spawnTurret(player_x, player_y, currentattack, 1, SG);
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
        case KEY1:
            levelup(AR);
            break;
        case KEY2:
            levelup(SMG);
            break;
        case KEY3:
            levelup(SR);
            break;
        case KEY4:
            levelup(SG);
            break;
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
                barricadeCaution = true;
            }
            break;
        case F:
            wavetime = true;
            InitializeEnemies();
            wave++;
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

void InitializeEnemies() {
    numEnemies = 0;
    numDestroyEnemies = 0;
    allenemy = 0;
}

void UI() {
    ScreenPrint(4, 28, "보유 포탑 : %3d", numTurreties);
    ScreenPrint(4, 30, "남은 적의 수(필드) : %3d", numEnemies - numDestroyEnemies);
    
    ScreenPrint(4, 32, "나온 적의 수 : %3d",  allenemy);

    for (int i = 0; i < 20; i++) {
        ScreenPrint(50, 5 + i, "|");
    }

    if (wavetime) { 
        ScreenPrint(70, 5, "%d wave", wave);
        ScreenPrint(68, 8, "웨이브 단계"); 
    }else{ 
        ScreenPrint(68, 8, "설치 단계  "); 
    }

    ScreenPrint(55, 12, "보유 골드: %5d", money);

    ScreenPrint(55, 14, "남은 라이프 : %d", life);

    if(barricadeCaution)    ScreenPrint(54, 10, " 바리게이트위에 설치되지 않았습니다");
    else ScreenPrint(54, 10, "                                        ");

    ScreenPrint(55, 21, "포탑 강화 ");
    ScreenPrint(60, 23, "1. AR");
    ScreenPrint(70, 23, "2. SMG");
    ScreenPrint(80, 23, "3. SR");
    ScreenPrint(90, 23, "4. SG");

    ScreenPrint(60, 25, "레벨 : %2d", turretLevel.ARLevel);
    ScreenPrint(70, 25, "레벨 : %2d", turretLevel.SMGLevel);
    ScreenPrint(80, 25, "레벨 : %2d", turretLevel.SGLevel);
    ScreenPrint(90, 25, "레벨 : %2d", turretLevel.SRLevel);


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

    int nextIdx = endIdx;
    int k = 0;

    while(nextIdx != -1) {
        map_copy[arr[nextIdx].y][arr[nextIdx].x] = PATH;
        nextIdx = parentList[nextIdx];
    }
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
            allenemy++;
            if (allenemy == 11) {
                endwave = true;


            }
            // Update the spawn time
            previousSpawnTime = currentTime;
            
            
        }
    }

}

void findwave() {
    if (endwave) {
        ScreenPrint(4, 36, "적 에네미 %d", numEnemies);
        if (numEnemies - numDestroyEnemies == 0) {
            ScreenPrint(4, 34, "웨이브는 %d", wavetime);

            wavetime = false;
        }
    }
}

void GameControl() {
    InitGame();
    ScreenInit();
    while(isPlaying){
        CurTime = clock();
        UI();

        if (!wavetime) {
            InstallStep();
        }
        else {
            //if(checkEnemy() == 0) wavetime = false;
            spawnEnemyWave(wave);
            
            collider();
            Render();
            moveEnemies();
            findwave();
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