#include "minesweeper.h"

using namespace std;

void windowicon()
{
    Image icon = LoadImage("..\\images\\logo.png");
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);
}

bool Table::isSafe(int a, int b)
{
    return (a >= 0 && a < hbomb && b >= 0 && b < wbomb);
}

void User::load_input()
{
    // Load dữ liệu từ file .dat
    std::fstream save;
    save.open("..\\src\\savefile.dat", std::ios::in);
    if (save)
    {
        save >> tab.wbomb >> tab.hbomb >> tab.vbomb >> score >> hiscore >> timer >> timerCounter;
        for (int i = 0; i < MAX_HEIGHT_TABLE; i++)
            for (int j = 0; j < MAX_WIDTH_TABLE; j++)
                save >> tab.grid[i][j];
        for (int i = 0; i < MAX_HEIGHT_TABLE; i++)
            for (int j = 0; j < MAX_WIDTH_TABLE; j++)
                save >> tab.opened[i][j];
        for (int i = 0; i < MAX_HEIGHT_TABLE; i++)
            for (int j = 0; j < MAX_WIDTH_TABLE; j++)
                save >> tab.flagged[i][j];
        save >> win >> tab.isActive;
    }
    save.close();
}
void User::save_output()
{
    std::fstream save;
    save.open("..\\src\\savefile.dat", std::ios::trunc | std::ios::out);
    save << tab.wbomb << ' ' << tab.hbomb << ' ' << tab.vbomb << ' ' << score << ' ' << hiscore << ' ' << timer << ' '
            << timerCounter << '\n';
    for (int i = 0; i < MAX_HEIGHT_TABLE; i++)
    {
        for (int j = 0; j < MAX_WIDTH_TABLE; j++)
            save << tab.grid[i][j] << ' ';
        save << "\n";
    }
    for (int i = 0; i < MAX_HEIGHT_TABLE; i++)
    {
        for (int j = 0; j < MAX_WIDTH_TABLE; j++)
            save << tab.opened[i][j] << ' ';
        save << "\n";
    }
    for (int i = 0; i < MAX_HEIGHT_TABLE; i++)
    {
        for (int j = 0; j < MAX_WIDTH_TABLE; j++)
            save << tab.flagged[i][j] << ' ';
        save << "\n";
    }
    save << win << ' ' << tab.isActive;
    save.close();
}
void User::create(const int &new_width, const int &new_height, const int &num_bomb)
{
    tab.wbomb = new_width;
    tab.hbomb = new_height;
    tab.vbomb = num_bomb;
    tab.isActive = true;
}
void User::timerCount()
{
    timerCounter++;
    if (timerCounter % 60 == 0)
    {
        // done 1s
        timer++;
        timerCounter = 0;
    }
}
void User::generate()
{
    srand(time(NULL));
    for (int i = 0; i < MAX_HEIGHT_TABLE; i++)
    {
        for (int j = 0; j < MAX_WIDTH_TABLE; j++)
        {
            tab.grid[i][j] = 0;
            tab.opened[i][j] = false;
            tab.flagged[i][j] = false;
        }
    }
    int bomb_left = tab.vbomb;
    while (bomb_left > 0)
    {
        for (int i = 0; i < tab.hbomb; i++)
            for (int j = 0; j < tab.wbomb; j++)
                if (rand() % 5 == 0 && bomb_left > 0 && tab.grid[i][j] == 0)
                {
                    tab.grid[i][j] = BOMB;
                    bomb_left--;
                }
    }

    for (int i = 0; i < tab.hbomb; i++)
    {
        for (int j = 0; j < tab.wbomb; j++)
        {
            if (tab.grid[i][j] == BOMB)
                continue;
            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (dx == 0 && dy == 0)
                        continue;
                    if (tab.isSafe(i + dx, j + dy) && tab.grid[i + dx][j + dy] == BOMB)
                        tab.grid[i][j]++;
                }
            }
        }
    }
    tab.isActive = false;
    score = 0;
    timer = 0;
    timerCounter = 0;
}
void User::BFS(const int &msy, const int &msx)
{
    queue<pair<int, int>> Q;
    Q.push({msy, msx});
    while (Q.size())
    {
        pair<int, int> curr = Q.front();
        Q.pop(); // it's common practice to pop immediately in BFS algorithm
        for (int dx = -1; dx <= 1; dx++)
            for (int dy = -1; dy <= 1; dy++)
            {
                int new_x = curr.first + dx;
                int new_y = curr.second + dy;
                if (tab.isSafe(new_x, new_y) && tab.grid[new_x][new_y] <= 8 && !tab.flagged[new_x][new_y] && !tab.opened[new_x][new_y])
                {
                    tab.opened[new_x][new_y] = true;
                    if (tab.grid[new_x][new_y] == 0)
                        Q.push({new_x, new_y});
                }
            }
    }
}



