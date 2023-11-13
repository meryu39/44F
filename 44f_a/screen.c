#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <sysinfoapi.h>
#define WIDTH 20
#define HEIGHT 20


#define MONSTER_SPEED 1000 // 몬스터 이동 간격 



char buffer1[HEIGHT][WIDTH];
char buffer2[HEIGHT][WIDTH];
char(*front_buffer)[WIDTH] = buffer1;
char(*back_buffer)[WIDTH] = buffer2;

int player_x = 0;
int player_y = 0;


int monster_x = 0;
int monster_y = 0;
//player 정보
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

void print_Map() {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            gotoxy(i * 2, j);
            if (j == player_y && i == player_x) {
                printf("★"); // 플레이어 그리기
            }
        
            else if (j == monster_y && i == monster_x) {
                printf("M");
             }

            else if (map[j][i] == 0) {
                printf("□"); // 맵 그리기
            }
            else if (map[j][i] == 3) {
                printf("※"); //바리게이트

            }
            else if (map[j][i] == 4) {
                printf("4"); //대포그리기

            }
            else if (map[j][i] == 5) {
                printf("5"); //대포그리기

            }
            else if (map[j][i] == 6) {
                printf("6"); //대포그리기

            }
            else if (map[j][i] == 7) {
                printf("7"); //대포그리기

            }
        }
    }
}

void move() {
    int site = getch();
    switch (site) {
    case 32: // 스페이스 바
        if (money >= 100) { // 해당 위치를 3으로 설정
            map[player_y][player_x] = 3;
            money -= 100;
        }
        break;
    case 101: //e키 누르면 
        if (map[player_y][player_x] == 3) {
            selectTurret();
            break;
        }
        else if (map[player_y][player_x] != 3) {

            gotoxy(45, 9);
            printf("바리게이트가 설치되지 않았습니다");
        }
        else {
            gotoxy(45, 9);
            printf("재화가 부족합니다");
        }
        
    case 224: // 화살표 키
        site = getch();
        switch (site) {
        case 72: // 위
            if (player_y != 0) {
                player_y--;
            }
            break;
        case 75: // 왼쪽
            if (player_x != 0) {
                player_x--;
            }
            break;
        case 77: // 오른쪽
            if (player_x != (WIDTH - 1)) {
                player_x++;
            }
            break;
        case 80: // 아래
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

void move_monster() {
    static DWORD previousTime = 0;
    DWORD currentTime = GetTickCount64();

    //시간경과에 따라 몬스터이동 
    if (currentTime - previousTime >= MONSTER_SPEED) {
        previousTime = currentTime;
        monster_x++;

        if (monster_x >= WIDTH) {
            monster_x = 0;
        }
    }
}
void selectTurret() {
    
    while (!turretSelected) {
        int key = getch();
        if (key == 32) { // 스페이스바
            turretSelected = true;
        }
        else if (key == 224) { // 화살표 키
            key = getch();
            switch (key) {
            case 75: // 좌
                if (selectedTurret > 0) {
                    selectedTurret--;
                }
                break;
            case 77: // 우
                if (selectedTurret < 3) {
                    selectedTurret++;
                }
                break;
            default:
                break;
            }
        }
        // UI에 선택된 포탑 종류 표시
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
void UI() {
    gotoxy(45, 1);
    printf("WAVE %d",  wave);

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
void Log() {
    gotoxy(45, 10);
    printf("%d", turretSelected);
}
/*
bool getbari() { //바리게이트 설치해도 되는 여부

}

*/
int main() {
    
    while (1) {
        print_Map();
        move();
        move_monster();
        swap_buffers();
        UI();
    }
    return 0;
}
