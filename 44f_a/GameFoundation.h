#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>

#define SPACEBAR 32
#define E 101
#define F 102
#define ARROWKEY 224

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80

#define EMPTY 0 //Ÿ���� ����
#define WALL 1
#define PATH 2

#define INF 99999 //�̾����� ���� ������ ���Ѵ��

#define X 0
#define Y 1

#define WIDTH 20
#define HEIGHT 20
#define MAX HEIGHT*WIDTH

#define MONSTER_SPEED 1000 // ���� �̵� ����

#define BUFFER_SIZE 1024

enum NodeType {
    Void = 0, START = 1, VERTEX = 8, END = 9
};
typedef struct {
    int key;
} element;