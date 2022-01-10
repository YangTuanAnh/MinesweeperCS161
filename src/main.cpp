#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "minesweeper.h"

int main()
{
    // Tạo window
    InitWindow(WIDTH, HEIGHT, "Minesweeper by Yang Tuấn Anh & Tứ Cảnh Minh");
    // Tạo window icon
    windowicon();
    // Đặt FPS cho game loop
    SetTargetFPS(FPS);
    // OST
    InitAudioDevice();
    Music music = LoadMusicStream("..\\music\\song.mp3");
    PlayMusicStream(music);
    // GUI Style cho RayGui
    GuiLoadStyle("..\\include\\candy.rgs");
    // Tạo var cho chương trình và data người dùng
    Program currProgram;
    User currUser;
    // Input
    currUser.input();

    while (!WindowShouldClose())
    {
        // - - - UPDATE - - - //

        // Pause và play nhạc
        UpdateMusicStream(music);
        if (IsKeyPressed(KEY_P))
        {
            currProgram.pause ^= 1;
            currProgram.pause ? PauseMusicStream(music) : ResumeMusicStream(music);
        }
        switch (currProgram.currentScreen)
        {
        case LOGO:
        {
            // Đợi 2s rồi chuyển qua title
            currProgram.framesCounter++;
            if (currProgram.framesCounter > FPS * 2)
                currProgram.currentScreen = TITLE;
        }
        break;
        case TITLE:
        {
            // Bấm New Game để qua DIFFICULTY
            if (GuiButton(Rectangle({250, 250, WIDTH - 250 * 2, 30}), "New Game"))
                currProgram.currentScreen = DIFFICULTY;
            // Bấm Continue để qua GAMEPLAY nếu chưa cần tạo mới game
            if (GuiButton(Rectangle({250, 290, WIDTH - 250 * 2, 30}), "Continue") && !currUser.gen)
                currProgram.currentScreen = GAMEPLAY;
            // Bấm Settings để qua SETTINGS
            if (GuiButton(Rectangle({250, 330, WIDTH - 250 * 2, 30}), "Settings"))
                currProgram.currentScreen = SETTINGS;
            // Bấm Save & Exit Game để load dữ liệu vào file .dat và đóng window
            if (GuiButton(Rectangle({250, 370, WIDTH - 250 * 2, 30}), "Save & Exit Game"))
                currUser.output();
        }
        break;
        case DIFFICULTY:
        {
            // Bấm EASY để tạo wbomb=8, hbomb=8, vbomb=10, tạo mới game, chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 250, WIDTH - 250 * 2, 30}), "EASY"))
            {
                currUser.create(8, 8, 10);
                currProgram.currentScreen = GAMEPLAY;
            }
            // Bấm INTERMEDIATE để tạo wbomb=16, hbomb=12, vbomb=50, tạo mới game, chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 290, WIDTH - 250 * 2, 30}), "INTERMEDIATE"))
            {
                currUser.create(16, 12, 50);
                currProgram.currentScreen = GAMEPLAY;
            }
            // Bấm HARD để tạo wbomb=24, hbomb=16, vbomb=100, tạo mới game, chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 330, WIDTH - 250 * 2, 30}), "HARD"))
            {
                currUser.create(24, 16, 100);
                currProgram.currentScreen = GAMEPLAY;
            }
            // Bấm CUSTOM để chuyển qua CUSTOM
            if (GuiButton(Rectangle({250, 370, WIDTH - 250 * 2, 30}), "CUSTOM"))
                currProgram.currentScreen = CUSTOM;
            // Bấm BACK để chuyển qua BACK
            if (GuiButton(Rectangle({250, 410, WIDTH - 250 * 2, 30}), "BACK"))
                currProgram.currentScreen = TITLE;
        }
        break;
        case CUSTOM:
        {
            // Bấm CREATE để chuyển qua GAMEPLAY
            if (GuiButton(Rectangle({250, 250, WIDTH - 250 * 2, 30}), "CREATE"))
                currProgram.currentScreen = GAMEPLAY;
            // 3 thanh slider để chỉnh giá trị wbomb, hbomb, vbomb
            GuiSpinner(Rectangle({250, 310, WIDTH - 250 * 2, 20}), "width", &currUser.tab.wbomb, MIN_GRID_WIDTH, MAX_GRID_WIDTH, false);
            GuiSpinner(Rectangle({250, 340, WIDTH - 250 * 2, 20}), "height", &currUser.tab.hbomb, MIN_GRID_HEIGHT, MAX_GRID_HEIGHT, false);
            GuiSpinner(Rectangle({250, 370, WIDTH - 250 * 2, 20}), "bombs", &currUser.tab.vbomb, 2, (currUser.tab.wbomb * currUser.tab.hbomb) / 2, false);
        }
        break;
        case GAMEPLAY:
        {
            // Chạy timer
            currUser.timerCount();
            // Tạo mới game
            if (currUser.gen)
                currUser.generate();
            // Lấy vị trí click chuột và toạ độ hoá trên ma trận grid
            Vector2 mousePos = GetMousePosition();
            currProgram.mousex = (mousePos.x - (WIDTH - 32 * currUser.tab.wbomb) / 2) / 32;
            currProgram.mousey = (mousePos.y - (HEIGHT - 32 * currUser.tab.hbomb) / 2) / 32 - 1;
            // Nếu click trái vào ma trận
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                currUser.tab.isSafe(currProgram.mousey, currProgram.mousex))
            {
                // Ko phải bomb thì score++
                if (currUser.tab.grid[currProgram.mousey][currProgram.mousex] < BOMB &&
                    currUser.tab.sgrid[currProgram.mousey][currProgram.mousex] == CLOSED)
                    currUser.score++;
                currUser.tab.sgrid[currProgram.mousey][currProgram.mousex] = currUser.tab.grid[currProgram.mousey][currProgram.mousex];
                // Nếu là ô trống thi bfs các ô trống xung quanh
                if (currUser.tab.sgrid[currProgram.mousey][currProgram.mousex] == 0)
                    currUser.bfs(currProgram.mousey, currProgram.mousex);
                // Nếu là bomb thì end game
                if (currUser.tab.sgrid[currProgram.mousey][currProgram.mousex] == BOMB)
                {
                    currProgram.currentScreen = WINLOSE;
                    currUser.win = false;
                    currUser.score *= currUser.timer;
                }
                int cnt = 0;
                for (int i = 0; i < currUser.tab.hbomb; i++)
                    for (int j = 0; j < currUser.tab.wbomb; j++)
                        if (currUser.tab.grid[i][j] == BOMB && currUser.tab.sgrid[i][j] == FLAGGED)
                            cnt++;
                if (cnt == currUser.tab.vbomb)
                {
                    currProgram.currentScreen = WINLOSE;
                    currUser.win = true;
                    currUser.score *= currUser.timer;
                }
            }
            // Nếu click chuột phải
            else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) &&
                     currUser.tab.isSafe(currProgram.mousey, currProgram.mousex))
                // Đổi giữa đặt cờ và ko đặt cờ
                if (currUser.tab.sgrid[currProgram.mousey][currProgram.mousex] == CLOSED ||
                    currUser.tab.sgrid[currProgram.mousey][currProgram.mousex] == FLAGGED)
                    currUser.tab.sgrid[currProgram.mousey][currProgram.mousex] ^= 1;
            // Bấm BACK để chuyển qua TITLE
            if (GuiButton(Rectangle({740, 20, 50, 30}), "BACK"))
                currProgram.currentScreen = TITLE;
        }
        break;
        case WINLOSE:
        {
            // Bấm BACK để chuyển qua TITLE, cho phép tạo mới game
            if (GuiButton(Rectangle({250, 280, WIDTH - 250 * 2, 30}), "BACK"))
            {
                currProgram.currentScreen = TITLE;
                currUser.gen = true;
            }
        }
        break;
        case SETTINGS:
        {
            // Đặt hiscore
            currUser.hiscore = std::max(currUser.score, currUser.hiscore);
            // Reset Data, cho phép tạo mới game
            if (GuiButton(Rectangle({250, 430, WIDTH - 250 * 2, 30}), "RESET DATA"))
            {
                currUser.tab.wbomb = currUser.tab.hbomb = currUser.tab.vbomb =
                    currUser.timer = currUser.timerCounter = currUser.score = currUser.hiscore = 0;
                currUser.gen = true;
            }
            // Bấm BACK để chuyển qua TITLE
            if (GuiButton(Rectangle({250, 470, WIDTH - 250 * 2, 30}), "BACK"))
                currProgram.currentScreen = TITLE;
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
        switch (currProgram.currentScreen)
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
            DrawText(TextFormat("SCORE: %i", currUser.score), 450, 30, 16, TRITARY);
            DrawText(TextFormat("TIMER: %i", currUser.timer), 550, 30, 16, TRITARY);
            DrawText(TextFormat("BOMBS: %i", currUser.tab.vbomb), 650, 30, 16, TRITARY);
            // In các ô ma trận dựa theo sgrid
            for (int i = 0; i < currUser.tab.hbomb; i++)
                for (int j = 0; j < currUser.tab.wbomb; j++)
                {
                    float posx = (WIDTH - 32 * currUser.tab.wbomb) / 2 + 32 * j;
                    float posy = 72 + (HEIGHT - 72 - 32 * currUser.tab.hbomb) / 2 + 32 * i;
                    // Vẽ nền của table
                    if (currUser.tab.sgrid[i][j] >= CLOSED)
                        DrawRectangle(posx, posy, 32, 32, PRIMARY);
                    else
                        DrawRectangle(posx, posy, 32, 32, SECONDARY);
                    // Vẽ kí tự của table
                    if (currUser.tab.sgrid[i][j] == FLAGGED)
                        DrawText("?", posx + 8, posy + 8, 16, RAYWHITE);
                    if (currUser.tab.sgrid[i][j] == BOMB)
                        DrawText("*", posx + 8, posy + 8, 16, RAYWHITE);
                    if (0 < currUser.tab.sgrid[i][j] && currUser.tab.sgrid[i][j] < BOMB)
                        DrawText(TextFormat("%d", currUser.tab.grid[i][j]), posx + 8, posy + 8, 16, RAYWHITE);
                    // Vẽ grid cho table
                    DrawRectangleLines(posx, posy, 32, 32, SECONDARY);
                }
        }
        break;
        case WINLOSE:
        {
            // In Score, Hiscore, Win/Lose
            DrawText("WINLOSE SCREEN", 20, 20, 40, PRIMARY);
            DrawText(TextFormat("SCORE: %i", currUser.score), 20, 60, 20, TRITARY);
            DrawText(TextFormat("HISCORE: %i", currUser.hiscore), 20, 90, 20, TRITARY);
            DrawText((currUser.win ? "WIN!" : "LOSE"), 380, 220, 20, SECONDARY);
            DrawText("PRESS BACK to TITLE SCREEN", 230, 250, 20, PRIMARY);
        }
        break;
        case SETTINGS:
        {
            // In các thông số Hiscore, score, timer, wbomb, hbomb, vbomb
            DrawText("SETTINGS SCREEN", 20, 20, 40, PRIMARY);
            DrawText("PRESS P to PLAY/PAUSE MUSIC", 220, 190, 20, PRIMARY);
            DrawText("PRESS BACK to TITLE SCREEN", 230, 220, 20, PRIMARY);
            DrawText(TextFormat("HISCORE: %i", currUser.hiscore, "%c;"), 350, 250, 20, TRITARY);
            DrawText(TextFormat("SCORE: %i", currUser.score), 350, 280, 20, TRITARY);
            DrawText(TextFormat("TIMER: %i", currUser.timer), 350, 310, 20, TRITARY);
            DrawText(TextFormat("BOMBS: %i", currUser.tab.vbomb), 350, 340, 20, TRITARY);
            DrawText(TextFormat("WIDTH: %i", currUser.tab.wbomb), 350, 370, 20, TRITARY);
            DrawText(TextFormat("HEIGHT: %i", currUser.tab.hbomb), 350, 400, 20, TRITARY);
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