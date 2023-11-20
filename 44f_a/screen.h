#pragma once

void ScreenInit();
void ScreenFlipping();
void ScreenClear();
void ScreenRelease();
void ScreenPrint(int x, int y, const char* format, ...);
void Render();
void SetColor(int color);