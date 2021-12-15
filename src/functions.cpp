#include "minesweeper.h"
#include <raylib.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <utility>
bool isSafe(int a, int b, int wbomb, int hbomb)
{
    return (a >= 0 && a < hbomb && b >= 0 && b < wbomb);
}
void windowicon()
{
    Image icon = LoadImage("..\\images\\logo.png");
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);
}
void generate(int grid[16][24], int sgrid[16][24], int wbomb, int hbomb, int vbomb, int &score, int &timer, int &timerCounter, bool &gen)
{
    srand(time(0));
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 24; j++)
        {
            grid[i][j] = 0;
            sgrid[i][j] = 10;
        }
    int cnt = vbomb;
    while (cnt > 0)
    {
        for (int i = 0; i < hbomb; i++)
            for (int j = 0; j < wbomb; j++)
                if (rand() % 5 == 0 && cnt > 0 && grid[i][j] == 0)
                {
                    grid[i][j] = 9;
                    cnt--;
                }
    }

    for (int i = 0; i < hbomb; i++)
        for (int j = 0; j < wbomb; j++)
        {
            if (grid[i][j] == 9)
                continue;
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (dx == 0 && dy == 0)
                        continue;
                    if (grid[i + dx][j + dy] == 9)
                        grid[i][j]++;
                }
        }
    score = 0;
    timer = 0;
    timerCounter = 0;
    gen = false;
}
void timerCount(int &timer, int &timerCounter)
{
    timerCounter++;
    if (timerCounter % 60 == 0)
    {
        timer++;
        timerCounter = 0;
    }
}
void bfs(int grid[16][24], int sgrid[16][24], int wbomb, int hbomb, int mousey, int mousex)
{
    std::queue<std::pair<int, int>> q;
    q.push({mousey, mousex});
    while (!q.empty())
    {
        std::pair<int, int> curr = q.front();
        sgrid[curr.first][curr.second] = grid[curr.first][curr.second];
        for (int dx = -1; dx <= 1; dx++)
            for (int dy = -1; dy <= 1; dy++)
                if (grid[curr.first + dx][curr.second + dy] < 9 && sgrid[curr.first + dx][curr.second + dy] == 10 && isSafe(curr.first + dx, curr.second + dy, wbomb, hbomb))
                {
                    sgrid[curr.first + dx][curr.second + dy] = grid[curr.first + dx][curr.second + dy];
                    if (grid[curr.first + dx][curr.second + dy] == 0)
                        q.push({curr.first + dx, curr.second + dy});
                }
        q.pop();
    }
}
void create(int a, int b, int c, bool d, GameScreen e, int &wbomb, int &hbomb, int &vbomb, bool &gen, GameScreen &currentScreen)
{
    wbomb = a;
    hbomb = b;
    vbomb = c;
    gen = d;
    currentScreen = e;
}
void winlose(bool res, int timer, GameScreen &currentScreen, bool &win, int &score)
{
    currentScreen = WINLOSE;
    win = res;
    score *= timer;
}