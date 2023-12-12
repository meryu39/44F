#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>

#define SPACEBAR 32
#define E 101
#define ARROWKEY 224

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80

#define EMPTY 0 //타일의 정보
#define WALL 1

#define INF 9999 //이어지지 않은 정점은 무한대로

#define X 0
#define Y 1

#define WIDTH 20
#define HEIGHT 20
#define MAX HEIGHT*WIDTH

#define MONSTER_SPEED 1000 // 몬스터 이동 간격

#define BUFFER_SIZE 1024