#pragma once

#include <raylib.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <utility>
#include <fstream>

const int WIDTH = 800, HEIGHT = 600;
const int MAX_GRID_WIDTH = 24, MAX_GRID_HEIGHT = 16;
const int MIN_GRID_WIDTH = 2, MIN_GRID_HEIGHT = 2;

const Color PRIMARY = {189, 74, 74, 255}, SECONDARY = {252, 105, 85, 255}, TRITARY = {229, 155, 95, 255};

typedef enum GameScreen
{
    LOGO = 0,
    TITLE,
    DIFFICULTY,
    CUSTOM,
    GAMEPLAY,
    WINLOSE,
    SETTINGS
} GameScreen;

struct Program
{
    GameScreen currentScreen;
    int framesCounter, mousex, mousey;
    bool pause;

    Program() : framesCounter(0), pause(false), currentScreen(LOGO) {}
};

struct Table
{
    int wbomb, hbomb, vbomb;
    int grid[MAX_GRID_HEIGHT][MAX_GRID_WIDTH], sgrid[MAX_GRID_HEIGHT][MAX_GRID_WIDTH];

    bool isSafe(int, int);
};

struct User
{
    Table tab;
    int score, hiscore, timer, timerCounter;
    bool win, gen;

    User() : gen(true) {}

    void input();
    void output();
    void generate();
    void timerCount();
    void bfs(int, int);
    void create(int, int, int);
};

void windowicon();
