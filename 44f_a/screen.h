#pragma once
#include "GameFoundation.h"

void ScreenInit();
void ScreenFlipping();
void ScreenClear();
void ScreenClearAll();
void ScreenRelease();
void ScreenPrint(int x, int y, const char* format, ...);
void SetColor(int color);

int MainSelected();
void print_main();