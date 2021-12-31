#include "minesweeper.h"

bool Table::isSafe(int a, int b)
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
void User::input()
{
    std::fstream save;
    save.open("..\\src\\savefile.dat", std::ios::in);
    if (save)
    {
        save >> tab.wbomb >> tab.hbomb >> tab.vbomb >> score >> hiscore >> timer >> timerCounter;
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 24; j++)
                save >> tab.grid[i][j];
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 24; j++)
                save >> tab.sgrid[i][j];
        save >> win >> gen;
    }
    save.close();
}
void User::output()
{
    std::fstream save;
    save.open("..\\src\\savefile.dat", std::ios::trunc | std::ios::out);
    save << tab.wbomb << " " << tab.hbomb << " " << tab.vbomb << " " << score << " " << hiscore << " " << timer << " " << timerCounter;
    save << "\n";
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 24; j++)
            save << tab.grid[i][j] << " ";
        save << "\n";
    }
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 24; j++)
            save << tab.sgrid[i][j] << " ";
        save << "\n";
    }
    save << win << " "
         << " " << gen;
    save.close();
    CloseWindow();
}
void User::generate()
{
    srand(time(0));
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 24; j++)
        {
            tab.grid[i][j] = 0;
            tab.sgrid[i][j] = 10;
        }
    int cnt = tab.vbomb;
    while (cnt > 0)
    {
        for (int i = 0; i < tab.hbomb; i++)
            for (int j = 0; j < tab.wbomb; j++)
                if (rand() % 5 == 0 && cnt > 0 && tab.grid[i][j] == 0)
                {
                    tab.grid[i][j] = 9;
                    cnt--;
                }
    }

    for (int i = 0; i < tab.hbomb; i++)
        for (int j = 0; j < tab.wbomb; j++)
        {
            if (tab.grid[i][j] == 9)
                continue;
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (dx == 0 && dy == 0)
                        continue;
                    if (tab.grid[i + dx][j + dy] == 9)
                        tab.grid[i][j]++;
                }
        }
    score = 0;
    timer = 0;
    timerCounter = 0;
    gen = false;
}
void User::timerCount()
{
    timerCounter++;
    if (timerCounter % FPS == 0)
    {
        timer++;
        timerCounter = 0;
    }
}
void User::bfs(int mousey, int mousex)
{
    std::queue<std::pair<int, int>> q;
    q.push({mousey, mousex});
    while (!q.empty())
    {
        std::pair<int, int> curr = q.front();
        q.pop();
        tab.sgrid[curr.first][curr.second] = tab.grid[curr.first][curr.second];
        for (int dx = -1; dx <= 1; dx++)
            for (int dy = -1; dy <= 1; dy++)
                if (tab.grid[curr.first + dx][curr.second + dy] < 9 &&
                    tab.sgrid[curr.first + dx][curr.second + dy] == 10 &&
                    tab.isSafe(curr.first + dx, curr.second + dy))
                {
                    tab.sgrid[curr.first + dx][curr.second + dy] = tab.grid[curr.first + dx][curr.second + dy];
                    if (tab.grid[curr.first + dx][curr.second + dy] == 0)
                        q.push({curr.first + dx, curr.second + dy});
                }
    }
}
void User::create(int a, int b, int c)
{
    tab.wbomb = a;
    tab.hbomb = b;
    tab.vbomb = c;
}