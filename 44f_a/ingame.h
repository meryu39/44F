#pragma once
#include "GameFoundation.h"

void selectTurret();
void move();
void move_monster();
void UI();

void monsterPath();
void PrintPath(int idx);
void Dijkstra(int startNodeIdx, int goalNodeIdx);
void InitGame();

void PrintMain();
void InitializeEnemies();