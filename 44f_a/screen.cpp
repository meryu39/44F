#include "screen.h"
#define _CRT_SECURE_NO_WARNINGS
static char buffer[1024];
static int g_nScreenIndex;
static HANDLE g_hScreen[2];

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