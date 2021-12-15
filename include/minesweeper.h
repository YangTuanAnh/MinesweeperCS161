#pragma once
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
bool isSafe(int a, int b, int wbomb, int hbomb);
void windowicon();
void generate(int grid[16][24], int sgrid[16][24], int wbomb, int hbomb, int vbomb, int &score, int &timer, int &timerCounter, bool &gen);
void timerCount(int &timer, int &timerCounter);
void bfs(int grid[16][24], int sgrid[16][24], int wbomb, int hbomb, int mousey, int mousex);
void create(int a, int b, int c, bool d, GameScreen e, int &wbomb, int &hbomb, int &vbomb, bool &gen, GameScreen &currentScreen);
void winlose(bool res, int timer, GameScreen &currentScreen, bool &win, int &score);