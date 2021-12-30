#pragma once
#include <raylib.h>
#include <raygui.h>
#define RAYGUI_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <ctime>
#include <queue>
#include <algorithm>

using namespace std;

const int WIDTH_WINDOW = 800, HEIGHT_WINDOW = 600;
const int MAX_WIDTH_TABLE = 24, MAX_HEIGHT_TABLE = 16;
const int MIN_WIDTH_TABLE = 2, MIN_HEIGHT_TABLE = 2;
const int BOMB = 9; // a specific value for bomb, closed box, flagged box

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
    int framesCounter;
    int mousex, mousey; // position of click
    GameScreen screen;
    Program() : framesCounter(0), screen(LOGO) {}
};

struct Table
{
    int wbomb;                                   // the width of the table that the user are playing
    int hbomb;                                   // height of table
    int vbomb;                                   // number of boms generated
    bool isActive;                               // the current table is activated or not, if not, create it !
    int grid[MAX_HEIGHT_TABLE][MAX_WIDTH_TABLE]; // the information of cells (blank = 0, number = 1..8, or bomb)
    // replace sgrid with these two can be longer but it would be more well-defined and accessible
    bool opened[MAX_HEIGHT_TABLE][MAX_WIDTH_TABLE];  // whether or not a cell is opened
    bool flagged[MAX_HEIGHT_TABLE][MAX_WIDTH_TABLE]; // whether or not a cell is flagged

    Table()
    {
        fill(&opened[0][0], &opened[0][0] + sizeof(opened), false);
        fill(&flagged[0][0], &flagged[0][0] + sizeof(flagged), false);
    }
    bool isSafe(int, int);
};

struct User
{
    Table tab;
    int score; // current score of the user
    int hiscore;
    int timer;        // real time (in seconds)
    int timerCounter; // FPS (1/60 seconds)
    bool win;

    User() : score(0), timer(0), timerCounter(0) {}
    void load_input();
    void save_output();
    void create(const int &, const int &, const int &);
    void timerCount();
    void generate();
    void BFS(const int &, const int &);
};

void windowicon();
