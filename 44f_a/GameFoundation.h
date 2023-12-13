#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>
#include <math.h>

#define SPACEBAR 32
#define E 101
#define F 102
#define ARROWKEY 224

#define KEY1 49
#define KEY2 50
#define KEY3 51
#define KEY4 52

#define MAXLEVEL 20

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80

#define EMPTY 0 //타일의 정보
#define WALL 1
#define PATH 2

#define AR 4
#define ARAttack 20
#define SMG 5
#define SMGAttack 10
#define SR 6
#define SRAttack 50
#define SG 7
#define SGAttack 40

#define INF 99999 //이어지지 않은 정점은 무한대로

#define X 0
#define Y 1

#define WIDTH 20
#define HEIGHT 20
#define MAX HEIGHT*WIDTH

#define MONSTER_SPEED 1000 // 몬스터 이동 간격

#define BUFFER_SIZE 1024

enum NodeType {
    Void = 0, START = 1, VERTEX = 8, END = 9
};
typedef struct {
    int key;
} element;