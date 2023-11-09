#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>

#define SCR_WIDTH 60
#define SCR_HEIGHT 30
void main() {
	const short x = 10, y = 20;
	const COORD  pos = { x,y }; //{x,SCR_HEIGHT - y -1} 하면 아래에서부터
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); //pos위치에 출력
	printf(">-O-<");

	while (true) {
		if (kbhit() != 0) {
			char c = getch();
			printf("Your input is %c \n", c);
		}
		else {
			printf("No Input ");
		}
		Sleep(10);
		system("cls");

	}
}