#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <fstream>
#include "minesweeper.h"
const int WIDTH = 800, HEIGHT = 600;

// Các screen: LOGO, TITLE, DIFFICULTY, CUSTOM, GAMEPLAY, WINLOSE, SETTINGS
/*typedef enum GameScreen
{
    LOGO = 0,
    TITLE,
    DIFFICULTY,
    CUSTOM,
    GAMEPLAY,
    WINLOSE,
    SETTINGS
} GameScreen;*/
GameScreen currentScreen = LOGO;

int framesCounter = 0, mousex, mousey;
int wbomb, hbomb, vbomb, score, hiscore, timer, timerCounter;
int grid[16][24];
int sgrid[16][24];
bool win, gen = true, pause = false;

Color PRIMARY = {189, 74, 74, 255}, SECONDARY = {252, 105, 85, 255}, TRITARY = {229, 155, 95, 255};

int main()
{
    // Tạo window
    InitWindow(WIDTH, HEIGHT, "Minesweeper by Yang Tuấn Anh & Từ Cảnh Minh");

    // Tạo window icon
    windowicon();

    // Đặt FPS cho game loop
    SetTargetFPS(60);

    // OST
    InitAudioDevice();
    Music music = LoadMusicStream("..\\music\\song.mp3");
    PlayMusicStream(music);

    // GUI Style cho RayGui
    GuiLoadStyle("..\\include\\candy.rgs");

    // Load dữ liệu từ file .dat
    std::fstream save;
    save.open("..\\src\\savefile.dat", std::ios::in);
    if (save)
    {
        save >> wbomb >> hbomb >> vbomb >> score >> hiscore >> timer >> timerCounter;
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 24; j++)
                save >> grid[i][j];
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 24; j++)
                save >> sgrid[i][j];
        save >> win >> gen;
    }
    save.close();

    while (!WindowShouldClose())
    {
        // - - - UPDATE - - - //

        // Pause và play nhạc
        UpdateMusicStream(music);
        if (IsKeyPressed(KEY_P))
        {
            pause = !pause;

            if (pause)
                PauseMusicStream(music);
            else
                ResumeMusicStream(music);
        }
        switch (currentScreen)
        {
        case LOGO:
        {
            // Đợi 2s rồi chuyển qua title
            framesCounter++;
            if (framesCounter > 120)
                currentScreen = TITLE;
        }
        break;
        case TITLE:
        {
            // Bấm New Game để qua DIFFICULTY
            if (GuiButton(Rectangle({250, 250, WIDTH - 250 * 2, 30}), "New Game"))
                currentScreen = DIFFICULTY;
            // Bấm Continue để qua GAMEPLAY nếu chưa cần tạo mới game
            if (GuiButton(Rectangle({250, 290, WIDTH - 250 * 2, 30}), "Continue") && !gen)
                currentScreen = GAMEPLAY;
            // Bấm Settings để qua SETTINGS
            if (GuiButton(Rectangle({250, 330, WIDTH - 250 * 2, 30}), "Settings"))
                currentScreen = SETTINGS;
            // Bấm Save & Exit Game để load dữ liệu vào file .dat và đóng window
            if (GuiButton(Rectangle({250, 370, WIDTH - 250 * 2, 30}), "Save & Exit Game"))
            {
                save.open("..\\src\\savefile.dat", std::ios::trunc | std::ios::out);
                save << wbomb << " " << hbomb << " " << vbomb << " " << score << " " << hiscore << " " << timer << " " << timerCounter;
                save << "\n";
                for (int i = 0; i < 16; i++)
                {
                    for (int j = 0; j < 24; j++)
                        save << grid[i][j] << " ";
                    save << "\n";
                }
                for (int i = 0; i < 16; i++)
                {
                    for (int j = 0; j < 24; j++)
                        save << sgrid[i][j] << " ";
                    save << "\n";
                }
                save << win << " "
                     << " " << gen;
                save.close();
                CloseWindow();
            }
        }
        break;
        case DIFFICULTY:
        {
            // Bấm EASY để tạo wbomb=8, hbomb=8, vbomb=10, tạo mới game, chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 250, WIDTH - 250 * 2, 30}), "EASY"))
                create(8, 8, 10, true, GAMEPLAY, wbomb, hbomb, vbomb, gen, currentScreen);

            // Bấm INTERMEDIATE để tạo wbomb=16, hbomb=12, vbomb=50, tạo mới game, chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 290, WIDTH - 250 * 2, 30}), "INTERMEDIATE"))
                create(16, 12, 50, true, GAMEPLAY, wbomb, hbomb, vbomb, gen, currentScreen);

            // Bấm HARD để tạo wbomb=24, hbomb=16, vbomb=100, tạo mới game, chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 330, WIDTH - 250 * 2, 30}), "HARD"))
                create(24, 16, 100, true, GAMEPLAY, wbomb, hbomb, vbomb, gen, currentScreen);

            // Bấm CUSTOM để chuyển qua CUSTOM
            if (GuiButton(Rectangle({250, 370, WIDTH - 250 * 2, 30}), "CUSTOM"))
                currentScreen = CUSTOM;

            // Bấm BACK để chuyển qua BACK
            if (GuiButton(Rectangle({250, 410, WIDTH - 250 * 2, 30}), "BACK"))
                currentScreen = TITLE;
        }
        break;
        case CUSTOM:
        {
            // Bấm CREATE để chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 250, WIDTH - 250 * 2, 30}), "CREATE"))
                currentScreen = GAMEPLAY;

            // 3 thanh slider để chỉnh giá trị wbomb, hbomb, vbomb
            GuiSpinner(Rectangle({250, 310, WIDTH - 250 * 2, 20}), "width", &wbomb, 2, 24, false);
            GuiSpinner(Rectangle({250, 340, WIDTH - 250 * 2, 20}), "height", &hbomb, 2, 16, false);
            GuiSpinner(Rectangle({250, 370, WIDTH - 250 * 2, 20}), "bombs", &vbomb, 2, (wbomb * hbomb) / 2, false);
        }
        break;
        case GAMEPLAY:
        {
            // Chạy timer
            timerCount(timer, timerCounter);

            // Tạo mới game
            if (gen)
                generate(grid, sgrid, wbomb, hbomb, vbomb, score, timer, timerCounter, gen);

            // Lấy vị trí click chuột và toạ độ hoá trên ma trận grid
            Vector2 mousePos = GetMousePosition();
            mousex = (mousePos.x - (WIDTH - 32 * wbomb) / 2) / 32, mousey = (mousePos.y - (HEIGHT - 32 * hbomb) / 2) / 32 - 1;

            // Nếu click trái vào ma trận
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isSafe(mousey, mousex, wbomb, hbomb))
            {
                // Ko phải bomb thì score++
                if (grid[mousey][mousex] < 9 && sgrid[mousey][mousex] == 10)
                    score++;
                sgrid[mousey][mousex] = grid[mousey][mousex];

                // Nếu là ô trống thi bfs các ô trống xung quanh
                if (sgrid[mousey][mousex] == 0)
                    bfs(grid, sgrid, wbomb, hbomb, mousey, mousex);

                // Nếu là bomb thì end game
                if (sgrid[mousey][mousex] == 9)
                    winlose(false, timer, currentScreen, win, score);
            }
            // Nếu click chuột phải
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && isSafe(mousey, mousex, wbomb, hbomb))
                // Đổi giữa đặt cờ và ko đặt cờ
                if (sgrid[mousey][mousex] == 10 || sgrid[mousey][mousex] == 11)
                    sgrid[mousey][mousex] = 21 - sgrid[mousey][mousex];

            // Bấm BACK để chuyển qua TITLE
            if (GuiButton(Rectangle({740, 10, 50, 30}), "BACK"))
                currentScreen = TITLE;

            // Bấm CHECK để dò số bomb đã đặt cờ, nếu đủ thì end game và win
            if (GuiButton(Rectangle({740, 50, 50, 30}), "CHECK"))
            {
                int cnt = 0;
                for (int i = 0; i < hbomb; i++)
                    for (int j = 0; j < wbomb; j++)
                        if (grid[i][j] == 9 && sgrid[i][j] == 11)
                            cnt++;
                if (cnt == vbomb)
                    winlose(true, timer, currentScreen, win, score);
            }
        }
        break;
        case WINLOSE:
        {
            // Bấm BACK để chuyển qua TITLE, cho phép tạo mới game
            if (GuiButton(Rectangle({250, 280, WIDTH - 250 * 2, 30}), "BACK"))
            {
                currentScreen = TITLE;
                gen = true;
            }
        }
        break;
        case SETTINGS:
        {
            // Đặt hiscore
            hiscore = std::max(score, hiscore);

            // Reset Data, cho phép tạo mới game
            if (GuiButton(Rectangle({250, 430, WIDTH - 250 * 2, 30}), "RESET DATA"))
            {
                wbomb = hbomb = vbomb = timer = timerCounter = score = hiscore = 0;
                gen = true;
            }

            // Bấm BACK để chuyển qua TITLE
            if (GuiButton(Rectangle({250, 470, WIDTH - 250 * 2, 30}), "BACK"))
                currentScreen = TITLE;
        }
        break;
        default:
            break;
        }

        // - - - DRAWING - - - //
        BeginDrawing();

        ClearBackground({255, 245, 225, 225});
        DrawCircle(40, 20, 50, {189, 74, 74, 255 / 2});
        DrawCircle(20, 40, 100, {252, 105, 85, 255 / 2});
        DrawCircle(WIDTH - 40, HEIGHT - 20, 50, {189, 74, 74, 255 / 2});
        DrawCircle(WIDTH - 20, HEIGHT - 40, 100, {252, 105, 85, 255 / 2});
        switch (currentScreen)
        {
        case LOGO:
        {
            DrawText("LOGO SCREEN", 20, 20, 40, PRIMARY);
            DrawText("WAIT for 2 SECONDS...", 290, 220, 20, PRIMARY);
        }
        break;
        case TITLE:
        {
            DrawText("MINESWEEPER", 20, 20, 40, PRIMARY);
            DrawText("CS161 Finals Project by Yang Tuan Anh & Tu Canh Minh", 120, 220, 20, PRIMARY);
        }
        break;
        case DIFFICULTY:
        {
            DrawText("DIFFICULTY SCREEN", 20, 20, 40, PRIMARY);
            DrawText("PRESS EASY, INTERMEDIATE or HARD to GAMEPLAY SCREEN", 100, 220, 20, PRIMARY);
        }
        break;
        case CUSTOM:
        {
            DrawText("CUSTOM SCREEN", 20, 20, 40, PRIMARY);
            DrawText("PRESS CREATE to GAMEPLAY SCREEN", 200, 220, 20, PRIMARY);
        }
        break;
        case GAMEPLAY:
        {
            DrawText("GAMEPLAY SCREEN", 20, 20, 40, PRIMARY);

            // In các thông số score, timer, vbomb
            DrawText(TextFormat("SCORE: %i", score), 450, 30, 16, TRITARY);
            DrawText(TextFormat("TIMER: %i", timer), 550, 30, 16, TRITARY);
            DrawText(TextFormat("BOMBS: %i", vbomb), 650, 30, 16, TRITARY);

            // In các ô ma trận dựa theo sgrid
            for (int i = 0; i < hbomb; i++)
                for (int j = 0; j < wbomb; j++)
                {
                    float posx = (WIDTH - 32 * wbomb) / 2 + 32 * j, posy = 72 + (HEIGHT - 72 - 32 * hbomb) / 2 + 32 * i;

                    sgrid[i][j] >= 10 ? DrawRectangle(posx, posy, 32, 32, PRIMARY) : DrawRectangle(posx, posy, 32, 32, SECONDARY);

                    if (sgrid[i][j] == 11)
                        DrawText("?", posx + 8, posy + 8, 16, RAYWHITE);
                    if (sgrid[i][j] == 9)
                        DrawText("*", posx + 8, posy + 8, 16, RAYWHITE);
                    if (0 < sgrid[i][j] && sgrid[i][j] < 9)
                        DrawText(TextFormat("%d", grid[i][j]), posx + 8, posy + 8, 16, RAYWHITE);

                    DrawRectangleLines(posx, posy, 32, 32, SECONDARY);
                }
        }
        break;
        case WINLOSE:
        {
            // In Score, Hiscore, Win/Lose
            DrawText("WINLOSE SCREEN", 20, 20, 40, PRIMARY);
            DrawText(TextFormat("SCORE: %i", score), 20, 60, 20, TRITARY);
            DrawText(TextFormat("HISCORE: %i", hiscore), 20, 90, 20, TRITARY);
            DrawText((win ? "WIN!" : "LOSE"), 380, 220, 20, SECONDARY);
            DrawText("PRESS BACK to TITLE SCREEN", 230, 250, 20, PRIMARY);
        }
        break;
        case SETTINGS:
        {
            // In các thông số Hiscore, score, timer, wbomb, hbomb, vbomb
            DrawText("SETTINGS SCREEN", 20, 20, 40, PRIMARY);
            DrawText("PRESS P to PLAY/PAUSE MUSIC", 220, 190, 20, PRIMARY);
            DrawText("PRESS BACK to TITLE SCREEN", 230, 220, 20, PRIMARY);
            DrawText(TextFormat("HISCORE: %i", hiscore, "%c;"), 350, 250, 20, TRITARY);
            DrawText(TextFormat("SCORE: %i", score), 350, 280, 20, TRITARY);
            DrawText(TextFormat("TIMER: %i", timer), 350, 310, 20, TRITARY);
            DrawText(TextFormat("BOMBS: %i", vbomb), 350, 340, 20, TRITARY);
            DrawText(TextFormat("WIDTH: %i", wbomb), 350, 370, 20, TRITARY);
            DrawText(TextFormat("HEIGHT: %i", hbomb), 350, 400, 20, TRITARY);
        }
        break;
        default:
            break;
        }

        EndDrawing();
    }
    UnloadMusicStream(music);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}