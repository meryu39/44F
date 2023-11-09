#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "screen.h"

#define WIDTH 20
#define HEIGHT 20
#define SIZE 20

#define MAX_X 38
#define MAX_Y 19

char buffer1[HEIGHT][WIDTH];
char buffer2[HEIGHT][WIDTH];

char* front_buffer = buffer1;
char* back_buffer = buffer2;

int player_x = 0;
int player_y = 0;
int site = 0;



void setXY(int x, int y) {
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void swap_buffers() {
	unsigned char* temp = front_buffer;
	front_buffer = back_buffer;
	back_buffer = temp;
}

void print_Map(char* buffer) {
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			setXY(i * 2, j);
			if (buffer[i * WIDTH + j] == 0) {
				printf("□");
			}
			else if (buffer[i * WIDTH + j] == 1) {
				printf(" ");
			}
		}
	}
}

void gotoxy(int x, int y) {
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void move() {
	site = getch();
    switch (site) {
    case 224:                           // arrow keys
        site = getch();
        switch (site) {
        case 72:                    // top
            if (player_y != 0) {
                player_y--;
            }

            break;

        case 75:                    // left

            if (player_x != 0) {

                player_x -= 2;

            }

            break;

        case 77:                    // right

            if (player_x != MAX_X) {

                player_x += 2;

            }

            break;

        case 80:                    // bottom

            if (player_y != MAX_Y) {

                player_y++;

            }

            break;

        default:

            break;

        }

    default:

        break;

    }
}

void Screen() {
	swap_buffers();
	print_Map(front_buffer);
}

int map[WIDTH][HEIGHT] ={
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

void main() {
	while (1) {
		Screen();
        gotoxy(player_x, player_y);
        move();
        printf("★");
		
	}
}


